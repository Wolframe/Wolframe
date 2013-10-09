**
**requires:LUA
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><task><title>job 1</title><key>A123</key><customernumber>324</customernumber></task><task><title>job 2</title><key>V456</key><customernumber>567</customernumber></task><employee><firstname>Julia</firstname><surname>Tegel-Sacher</surname><phone>098 765 43 21</phone></employee><issuedate>13.5.2006</issuedate></assignment><assignment><task><title>job 3</title><key>A456</key><customernumber>567</customernumber></task><task><title>job 4</title><key>V789</key><customernumber>890</customernumber></task><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><issuedate>13.5.2006</issuedate></assignment></assignmentlist>**config
--input-filter textwolf --output-filter textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf  --module ../../src/modules/cmdbind/lua/mod_command_lua --module ../../src/modules/ddlcompiler//simpleform/mod_ddlcompiler_simpleform --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/normalize//string/mod_normalize_string --program simpleform_range_trim.wnmp --program employee_assignment_print.sfrm --cmdprogram ddlform_iterator_scope.lua run

**file:simpleform_range_trim.wnmp
int=string:trim,number:integer(10);
uint=string:trim,number:unsigned(10);
float=string:trim,number:float(10,10);
currency=number:fixedpoint(13,2);
percent_1=number:fixedpoint(5,1);
**file: ddlform_iterator_scope.lua
function print_tree( itr)
	for v,t in itr do
		output:print( v,t)
	end
	output:closetag()
end

function run_employee( itr)
	local employee = form("Employee"):fill( itr)
	output:print( employee:get())
end

function run_task( itr)
	for v,t in itr do
		if t == "title" then
			if not v then
				output:opentag( t)
				print_tree( scope(itr))
			else
				output:print( v,t)
			end
		elseif t == "key" then
			if not v then
				output:opentag( t)
				print_tree( scope(itr))
			else
				output:print( v,t)
			end
		elseif t == "customernumber" then
			if not v then
				output:opentag( t)
				print_tree( scope(itr))
			else
				output:print( v,t)
			end
		else
			error( "unknown element " .. tostring(t) .. " " .. tostring(v))
		end
	end
end

function run_assignment( itr)
	for v,t in itr do
		if t == "task" then
			output:opentag( t)
			run_task( scope( itr))
			output:closetag()
		elseif t == "issuedate" then
			if not v then
				output:opentag( t)
				print_tree( scope(itr))
			else
				output:print( v,t)
			end
		elseif t == "employee" then
			output:opentag( t)
			run_employee( scope( itr))
			output:closetag()
		else
			error( "unknown element " .. tostring(t) .. " " .. tostring(v))
		end
	end
end

function run_assignmentlist( itr)
	for v,t in itr do
		if t == "assignment" then
			output:opentag( t)
			run_assignment( scope( itr))
			output:closetag()
		else
			error( "unknown element " .. tostring(t) .. " " .. tostring(v))
		end
	end
end

function run()
	r = form("employee_assignment_print")
	r:fill( input:table())
	itr = r:get()
	for v,t in itr do
		if t == "assignmentlist" then
			output:opentag( "assignmentlist")
			run_assignmentlist( scope( itr))
			output:closetag()
		else
			error( "unknown element " .. tostring(t) .. " " .. tostring(v))
		end
	end
end

**file: employee_assignment_print.sfrm
FORM Employee
{
	firstname string
	surname string
	phone string
}

FORM employee_assignment_print
{
	assignmentlist
	{
		assignment []
		{
			task []
			{
				title string
				key string
				customernumber int
			}
			employee Employee
			issuedate string
		}
	}
}

**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><task><title>job 1</title><key>A123</key><customernumber>324</customernumber></task><task><title>job 2</title><key>V456</key><customernumber>567</customernumber></task><employee><firstname>Julia</firstname><surname>Tegel-Sacher</surname><phone>098 765 43 21</phone></employee><issuedate>13.5.2006</issuedate></assignment><assignment><task><title>job 3</title><key>A456</key><customernumber>567</customernumber></task><task><title>job 4</title><key>V789</key><customernumber>890</customernumber></task><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><issuedate>13.5.2006</issuedate></assignment></assignmentlist>
**end

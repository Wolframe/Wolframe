**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
{
	"assignmentlist":	{
		"assignment":	[{
				"task":	[{
						"title":	"job 1",
						"key":	"A123",
						"customernumber":	"324"
					}, {
						"title":	"job 2",
						"key":	"V456",
						"customernumber":	"567"
					}],
				"employee":	{
					"firstname":	"Julia",
					"surname":	"Tegel-Sacher",
					"phone":	"098 765 43 21"
				},
				"issuedate":	"13.5.2006"
			}, {
				"task":	[{
						"title":	"job 3",
						"key":	"A456",
						"customernumber":	"567"
					}, {
						"title":	"job 4",
						"key":	"V789",
						"customernumber":	"890"
					}],
				"employee":	{
					"firstname":	"Jakob",
					"surname":	"Stegelin",
					"phone":	"012 345 67 89"
				},
				"issuedate":	"13.5.2006"
			}]
	}
}**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson -c wolframe.conf run

**file:wolframe.conf
LoadModules
{
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform
	module ../../src/modules/normalize/number/mod_normalize_number
	module ../../src/modules/normalize/string/mod_normalize_string
}
Processor
{
	program		normalize.wnmp
	program		form.sfrm
	cmdhandler
	{
		lua
		{
			program script.lua
		}
	}
}
**file:normalize.wnmp
int=trim,integer(10 );
uint=trim,unsigned(10);
float=trim, floatingpoint( 10,10);
currency=fixedpoint(13, 2);
percent_1=fixedpoint(5 ,1);
**file:form.sfrm
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
**file:script.lua
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
**requires:DISABLED NETBSD

**output
{
	"assignmentlist":	{
		"assignment":	[{
				"task":	[{
						"title":	"job 1",
						"key":	"A123",
						"customernumber":	"324"
					}, {
						"title":	"job 2",
						"key":	"V456",
						"customernumber":	"567"
					}],
				"employee":	{
					"firstname":	"Julia",
					"surname":	"Tegel-Sacher",
					"phone":	"098 765 43 21"
				},
				"issuedate":	"13.5.2006"
			}, {
				"task":	[{
						"title":	"job 3",
						"key":	"A456",
						"customernumber":	"567"
					}, {
						"title":	"job 4",
						"key":	"V789",
						"customernumber":	"890"
					}],
				"employee":	{
					"firstname":	"Jakob",
					"surname":	"Stegelin",
					"phone":	"012 345 67 89"
				},
				"issuedate":	"13.5.2006"
			}]
	}
}
**end

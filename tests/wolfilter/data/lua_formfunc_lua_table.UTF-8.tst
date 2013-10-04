**
**requires:LUA
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><task><title>job 1</title><key>A123</key><customernumber>324</customernumber></task><task><title>job 2</title><key>V456</key><customernumber>567</customernumber></task><employee><firstname>Julia</firstname><surname>Tegel-Sacher</surname><phone>098 765 43 21</phone></employee><issuedate>13.5.2006</issuedate></assignment><assignment><task><title>job 3</title><key>A456</key><customernumber>567</customernumber></task><task><title>job 4</title><key>V789</key><customernumber>890</customernumber></task><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><issuedate>13.5.2006</issuedate></assignment></assignmentlist>**config
--input-filter textwolf --output-filter textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf  --module ../../src/modules/cmdbind/lua/mod_command_lua --program program_formfunc.lua --module ../wolfilter/modules//employee_assignment_convert//mod_employee_assignment_convert --cmdprogram formfunc_lua_table.lua run

**file:program_formfunc.lua
function toupper_table( tb)
	for i,v in pairs(tb)
	do
		if type(v) == "table" then
			tb[ i] = toupper_table(v)
		else
			tb[ i] = string.upper(v)
		end
	end
	return tb
end

function toupper_formfunc( inp)
	return toupper_table( inp:table())
end
**file: formfunc_lua_table.lua

function run()
	res = formfunction( "toupper_formfunc")( input:table())
	output:print( res:table())
end
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><issuedate>13.5.2006</issuedate><employee><firstname>JULIA</firstname><phone>098 765 43 21</phone><surname>TEGEL-SACHER</surname></employee><task><customernumber>324</customernumber><key>A123</key><title>JOB 1</title></task><task><customernumber>567</customernumber><key>V456</key><title>JOB 2</title></task></assignment><assignment><issuedate>13.5.2006</issuedate><employee><firstname>JAKOB</firstname><phone>012 345 67 89</phone><surname>STEGELIN</surname></employee><task><customernumber>567</customernumber><key>A456</key><title>JOB 3</title></task><task><customernumber>890</customernumber><key>V789</key><title>JOB 4</title></task></assignment></assignmentlist>
**end

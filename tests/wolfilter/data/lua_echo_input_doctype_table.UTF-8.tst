**
**requires:LUA
**input
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE assignmentlist PUBLIC "-//ABC//DTD XML 1.0 plus Software 2.0 plus ABC 1.1//EN" "http://www.wolframe.org/example-doc/employee_assignment_print.dtd"><assignmentlist><assignment><task><title>job 1</title><key>A123</key><customernumber>324</customernumber></task><task><title>job 2</title><key>V456</key><customernumber>567</customernumber></task><employee><firstname>Julia</firstname><surname>Tegel-Sacher</surname><phone>098 765 43 21</phone></employee><issuedate>13.5.2006</issuedate></assignment><assignment><task><title>job 4</title><key>V789</key><customernumber>890</customernumber></task><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><issuedate>13.5.2006</issuedate></assignment></assignmentlist>**config
--input-filter xml:textwolf --output-filter xml:textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf  --module ../../src/modules/cmdbind/lua/mod_command_lua --module ../../src/modules/ddlcompiler//simpleform/mod_ddlcompiler_simpleform --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/normalize//string/mod_normalize_string --program simpleform_range_trim.normalize --program employee_assignment_print.simpleform --cmdprogram echo_input_doctype_table.lua run

**file:simpleform_range_trim.normalize
int=string:trim,number:integer(10);
uint=string:trim,number:unsigned(10);
float=string:trim,number:float(10,10);
**file: echo_input_doctype_table.lua
function run()
	type = input:doctype()
	output:as( filter(), type)
	output:print( input:table())
end
**file: employee_assignment_print.simpleform
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
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE assignmentlist PUBLIC "-//ABC//DTD XML 1.0 plus Software 2.0 plus ABC 1.1//EN" "http://www.wolframe.org/example-doc/employee_assignment_print.dtd"><assignmentlist><assignment><issuedate>13.5.2006</issuedate><employee><firstname>Julia</firstname><phone>098 765 43 21</phone><surname>Tegel-Sacher</surname></employee><task><customernumber>324</customernumber><key>A123</key><title>job 1</title></task><task><customernumber>567</customernumber><key>V456</key><title>job 2</title></task></assignment><assignment><issuedate>13.5.2006</issuedate><employee><firstname>Jakob</firstname><phone>012 345 67 89</phone><surname>Stegelin</surname></employee><task><customernumber>890</customernumber><key>V789</key><title>job 4</title></task></assignment></assignmentlist>
**end

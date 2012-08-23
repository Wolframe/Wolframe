**
**requires:LUA
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><task><title>job 1</title><key>A123</key><customernumber>324</customernumber></task><task><title>job 2</title><key>V456</key><customernumber>567</customernumber></task><employee><firstname>Julia</firstname><surname>Tegel-Sacher</surname><phone>098 765 43 21</phone></employee><issuedate>13.5.2006</issuedate></assignment><assignment><task><title>job 3</title><key>A456</key><customernumber>567</customernumber></task><task><title>job 4</title><key>V789</key><customernumber>890</customernumber></task><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><issuedate>13.5.2006</issuedate></assignment></assignmentlist>**config
--input-filter xml:textwolf --output-filter xml:textwolf --form employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script peerformfunc_table.lua run
**file: peerformfunc_table.lua

function run()
	t = input:table()
	r = transaction("echo_transaction")( t)
	output:print( r:table())
end

**file: employee_assignment_print.simpleform
!DOCTYPE "assignmentlist 'http://www.wolframe.org/example-doc/employee_assignment_print.dtd'"
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
		employee
		{
			firstname string
			surname string
			phone string
		}
		issuedate string
	}
}
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><issuedate>13.5.2006</issuedate><employee><phone>098 765 43 21</phone><firstname>Julia</firstname><surname>Tegel-Sacher</surname></employee><task><customernumber>324</customernumber><key>A123</key><title>job 1</title></task><task><customernumber>567</customernumber><key>V456</key><title>job 2</title></task></assignment><assignment><issuedate>13.5.2006</issuedate><employee><phone>012 345 67 89</phone><firstname>Jakob</firstname><surname>Stegelin</surname></employee><task><customernumber>567</customernumber><key>A456</key><title>job 3</title></task><task><customernumber>890</customernumber><key>V789</key><title>job 4</title></task></assignment></assignmentlist>
**end

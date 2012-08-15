**
**requires:LUA
**input
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE assignmentlist PUBLIC "-//ABC//DTD XML 1.0 plus Software 2.0 plus ABC 1.1//EN" "http://www.wolframe.org/example-doc/employee_assignment_print.dtd"><assignmentlist><assignment><task><title>job 1</title><key>A123</key><customernumber>324</customernumber></task><task><title>job 2</title><key>V456</key><customernumber>567</customernumber></task><employee><firstname>Julia</firstname><surname>Tegel-Sacher</surname><phone>098 765 43 21</phone></employee><issuedate>13.5.2006</issuedate></assignment><assignment><task><title>job 3</title><key>A456</key><customernumber>567</customernumber></task><task><title>job 4</title><key>V789</key><customernumber>890</customernumber></task><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><issuedate>13.5.2006</issuedate></assignment></assignmentlist>**config
--input-filter xml:textwolf --output-filter xml:textwolf --form employee_assignment_print.simpleform --script echo_input_doctype_form.lua run
**file: echo_input_doctype_form.lua
function run()
	f = input:form()
	output:as( filter(), f:doctype())
	output:print( f:get())
end
**output
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE assignmentlist SYSTEM "http://www.wolframe.org/example-doc/employee_assignment_print.dtd"><assignmentlist><assignment><task><title>job 1</title><key>A123</key><customernumber>324</customernumber></task><task><title>job 2</title><key>V456</key><customernumber>567</customernumber></task><employee><firstname>Julia</firstname><surname>Tegel-Sacher</surname><phone>098 765 43 21</phone></employee><issuedate>13.5.2006</issuedate></assignment><assignment><task><title>job 3</title><key>A456</key><customernumber>567</customernumber></task><task><title>job 4</title><key>V789</key><customernumber>890</customernumber></task><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><issuedate>13.5.2006</issuedate></assignment></assignmentlist>
**end
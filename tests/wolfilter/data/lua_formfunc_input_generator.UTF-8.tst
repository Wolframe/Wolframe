**
**requires:LUA
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><task><title>job 1</title><key>A123</key><customernumber>324</customernumber></task><task><title>job 2</title><key>V456</key><customernumber>567</customernumber></task><employee><firstname>Julia</firstname><surname>Tegel-Sacher</surname><phone>098 765 43 21</phone></employee><issuedate>13.5.2006</issuedate></assignment><assignment><task><title>job 3</title><key>A456</key><customernumber>567</customernumber></task><task><title>job 4</title><key>V789</key><customernumber>890</customernumber></task><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><issuedate>13.5.2006</issuedate></assignment></assignmentlist>**config
--input-filter xml:textwolf --output-filter xml:textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf  --module ../../src/modules/cmdbind/lua/mod_command_lua --module ../wolfilter/modules//employee_assignment_convert//mod_employee_assignment_convert --cmdprogram formfunc_input_generator.lua run
**file: formfunc_input_generator.lua

function run()
	r = formfunction("employee_assignment_convert")( input:get())
	output:print( r:get())
end
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><issuedate>8647993</issuedate><employee><firstname>juulliiaa</firstname><surname>teeggeell-saacchheerr</surname><phone>901 234 56 78</phone></employee><task><key>a876</key><title>jjoobb 8</title><customernumber>325</customernumber></task><task><key>v543</key><title>jjoobb 7</title><customernumber>568</customernumber></task></assignment><assignment><issuedate>8647993</issuedate><employee><firstname>jaakkoobb</firstname><surname>stteeggeelliinn</surname><phone>987 654 32 10</phone></employee><task><key>a543</key><title>jjoobb 6</title><customernumber>568</customernumber></task><task><key>v210</key><title>jjoobb 5</title><customernumber>891</customernumber></task></assignment></assignmentlist>
**end

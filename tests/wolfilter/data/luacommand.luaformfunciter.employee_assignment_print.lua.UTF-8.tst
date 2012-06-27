**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><issuedate>13.5.2006</issuedate><employee><firstname>Julia</firstname><phone>098 765 43 21</phone><surname>Tegel-Sacher</surname></employee><task><customernumber>324</customernumber><title>job 1</title><key>A123</key></task><task><customernumber>567</customernumber><title>job 2</title><key>V456</key></task></assignment><assignment><issuedate>13.5.2006</issuedate><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><task><customernumber>567</customernumber><title>job 3</title><key>A456</key></task><task><customernumber>890</customernumber><title>job 4</title><key>V789</key></task></assignment></assignmentlist>
**file: echo_formfunc_generator.lua

function run()
	f = filter( "xml:textwolf")

	-- a W3C conform XML parser returns by default also tokens containing only spaces.
	-- these we eliminate to be able to create a table:
	f.empty = false

	input:as(f)
	output:as(f)

	r = formfunction("employee_assignment_convert")( input:get())
	for v,t in r:get() do
		output:print( v,t)
	end
end
**config
--script echo_formfunc_generator.lua run xml:textwolf
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><issuedate>8647993</issuedate><employee><firstname>juulliiaa</firstname><surname>teeggeell-saacchheerr</surname><phone>901 234 56 78</phone></employee><task><key>a876</key><title>jjoobb 8</title><customernumber>325</customernumber></task><task><key>v543</key><title>jjoobb 7</title><customernumber>568</customernumber></task></assignment><assignment><issuedate>8647993</issuedate><employee><firstname>jaakkoobb</firstname><surname>stteeggeelliinn</surname><phone>987 654 32 10</phone></employee><task><key>a543</key><title>jjoobb 6</title><customernumber>568</customernumber></task><task><key>v210</key><title>jjoobb 5</title><customernumber>891</customernumber></task></assignment></assignmentlist>
**end

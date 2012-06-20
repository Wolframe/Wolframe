**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><issuedate>13.5.2006</issuedate><employee><firstname>Julia</firstname><phone>098 765 43 21</phone><surname>Tegel-Sacher</surname></employee><task><customernumber>324</customernumber><title>job 1</title><key>A123</key></task><task><customernumber>567</customernumber><title>job 2</title><key>V456</key></task></assignment><assignment><issuedate>13.5.2006</issuedate><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><task><customernumber>567</customernumber><title>job 3</title><key>A456</key></task><task><customernumber>890</customernumber><title>job 4</title><key>V789</key></task></assignment></assignmentlist>
**file: echo_input_generator.lua
function run()
	f = filter( "xml:textwolf")

	-- a W3C conform XML parser returns by default also tokens containing only spaces.
	-- these we eliminate to be able to create a table:
	f.empty = false

	input:as(f)
	output:as(f)

	output:print( input:get())
end

**config
--script echo_input_generator.lua run xml:textwolf
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><issuedate>13.5.2006</issuedate><employee><firstname>Julia</firstname><phone>098 765 43 21</phone><surname>Tegel-Sacher</surname></employee><task><customernumber>324</customernumber><title>job 1</title><key>A123</key></task><task><customernumber>567</customernumber><title>job 2</title><key>V456</key></task></assignment><assignment><issuedate>13.5.2006</issuedate><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><task><customernumber>567</customernumber><title>job 3</title><key>A456</key></task><task><customernumber>890</customernumber><title>job 4</title><key>V789</key></task></assignment></assignmentlist>
**end

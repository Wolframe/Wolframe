function run()
	f = filter( "xml:textwolf")

	-- a W3C conform XML parser returns by default also tokens containing only spaces.
	-- these we eliminate to be able to create a table:
	f.empty = false

	input:as( f)
	type = input:doctype()
	logger.print( "INFO", "DOCTYPE ", type)
	output:as( f, 'assignmentlist')
	output:as( f, 'assignmentlist "http://www.wolframe.org/example-doc/employee_assignement_print.dtd"')
	output:as( f, 'assignmentlist "-//ABC//DTD XML 1.0 plus Software 2.0 plus ABC 1.1//EN" "http://www.wolframe.org/example-doc/employee_assignement_print.dtd"')
	output:as( f, type)
	output:print( input:get())
end


function run()
	f = filter( "xml:textwolf")

	-- a W3C conform XML parser returns by default also tokens containing only spaces.
	-- these we eliminate to be able to create a table:
	f.empty = false

	input:as(f)
	output:as(f)

	t = input:table()
	r = formfunction("employee_assignment_convert")( t)
	output:print( r:table())
end


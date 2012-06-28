
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

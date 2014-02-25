
function run()
	r = provider.form("employee_assignment_print")
	r:fill( input:table())
	for v,t in r:get() do
		output:print( v,t)
	end
end

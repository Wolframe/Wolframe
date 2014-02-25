
function run()
	t = input:table()
	r = provider.formfunction("employee_assignment_convert")( t)
	output:print( r:table())
end


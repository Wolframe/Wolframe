
function run()
	r = provider.form("employee_assignment_print")
	r:fill( input:get())
	output:print( r:get())
end


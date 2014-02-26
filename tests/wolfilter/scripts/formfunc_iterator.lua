
function run()
	r = provider.formfunction("employee_assignment_convert")( input:get())
	for v,t in r:get() do
		output:print( v,t)
	end
end

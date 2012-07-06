function run()
	f = input:form()
	output:as( filter(), f:doctype())
	output:print( f:get())
end

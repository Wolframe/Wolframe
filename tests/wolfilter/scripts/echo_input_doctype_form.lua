function run()
	f = input:form()
	output:as( filter(), f:name())
	output:print( f:get())
end

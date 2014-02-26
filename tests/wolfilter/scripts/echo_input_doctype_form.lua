function run()
	f = input:form()
	output:as( provider.filter(), f:name())
	output:print( f:get())
end

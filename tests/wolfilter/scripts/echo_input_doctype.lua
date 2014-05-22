function run()
	output:as( provider.filter(), input:metadata())
	output:print( input:get())
end

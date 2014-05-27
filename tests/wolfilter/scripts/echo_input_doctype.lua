function run()
	output:as( provider.filter(), input:metadata(), input:doctype())
	output:print( input:get())
end

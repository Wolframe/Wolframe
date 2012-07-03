function run()
	type = input:doctype()
	output:as( filter(), type)
	output:print( input:get())
end

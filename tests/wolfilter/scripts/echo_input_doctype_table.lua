function run()
	type = input:doctype()
	output:as( provider.filter(), type)
	output:print( input:table())
end

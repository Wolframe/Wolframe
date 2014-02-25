function run()
	type = input:doctype()
	output:as( provider.filter(), type)
	inp = input:table()
	inp_ = input:table()
	output:print( inp)
end

function run()
	type = input:doctype()
	output:as( filter(), type)
	inp = input:table()
	inp_ = input:table()
	output:print( inp)
end

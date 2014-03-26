function run()
	local doctype = input:doctype()
	output:as( provider.filter(), doctype)
	inp = input:table()
	inp_ = input:table()
	output:print( inp)
end

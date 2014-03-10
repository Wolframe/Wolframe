function run()
	f = provider.filter()
	input:as( provider.filter("blob"))
	t = input:value()
	d = provider.document( t)
	d:as( f)
	output:print( d:table())
end

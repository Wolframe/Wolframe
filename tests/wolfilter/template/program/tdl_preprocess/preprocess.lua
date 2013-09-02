function run( inp )
	it = inp:table()
	getData = formfunction("getData")
	res = getData( it)
	rt = res:table()
	return rt
end


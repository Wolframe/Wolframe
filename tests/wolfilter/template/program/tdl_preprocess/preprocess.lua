function run( inp )
	it = inp:table()
	logger.printc( "INPUT ", it)
	getData = formfunction("getData")
	res = getData( it)
	logger.printc( "RESULT ", res)
	rt = res:table()
	logger.printc( "OUTPUT ", rt)
	return rt
end


function run()
	type = input:doctype()
	logger.printc( "DOC TYPE ", type)
	local t = input:table()
	logger.printc( "TABLE 1 ", t)
	t = input:table()
	logger.printc( "TABLE 2 ", t)
	output:print( t)
end

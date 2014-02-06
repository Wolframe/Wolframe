
function GetIdPair( inp_)
	logger.printc( "START function GetIdPair")
	rt = inp_:table().place
	logger.printc( "INPUT ", rt)
	rt.a = 123
	rt.b = 987
	return rt
end


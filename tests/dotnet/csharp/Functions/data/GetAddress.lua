
function GetAddress( inp_)
	logger.printc( "START function GetAddress")
	rt = inp_:table().place
	logger.printc( "INPUT ", rt)
	rt.street = string.lower(rt.street)
	logger.printc( "STREET ", rt.street)
	rt.country = string.lower(rt.country)
	logger.printc( "COUNTRY ", rt.street)
	logger.printc( "RETURN ", rt)
	return rt
end

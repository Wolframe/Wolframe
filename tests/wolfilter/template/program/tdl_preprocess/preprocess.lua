function run( inp )
	logger.printc( "RUN 1")
	it = inp:table()
	logger.printc( "RUN 2")
	getData = provider.formfunction("getData")
	logger.printc( "RUN 3")
	res = getData( it)
	logger.printc( "RUN 4")
	rt = res:table()
	logger.printc( "RUN 5")
	insertWords = provider.formfunction("insertWords")
	logger.printc( "RUN 6")
	insertWords( { data = rt } )
	logger.printc( "RUN 7")
	getDataFiltered = provider.formfunction("getDataFiltered")
	logger.printc( "RUN 8")
	resfiltered = getDataFiltered( it)
	logger.printc( "RUN 9")
	resfilteredtab = resfiltered:table()
	logger.printc( "RUN 10")
	for k,v in ipairs(  resfilteredtab['person']) do
		table.insert( rt.person, v)
	end
	logger.printc( "RUN 11")
	return rt
end

function luanorm( inp )
	local function luanorm_table( tb )
		local rt = {}
		for k,v in pairs( tb) do
			if type(v) == "table" then
				rt[ k] = luanorm_table( v)
			else
				if k == "id" or k == "tag" then
					rt[ k] = tonumber(v) + 100
				else
					local nf = provider.type( "normname")
					rt[ k] = nf( v)
				end
			end
		end
		return rt
	end
	local intb = inp:table()
	local outtb = luanorm_table( intb)
	return outtb
end

function addSuffixToName( inp)
	rec = inp:table()
	for i,v in ipairs( rec["person"]) do
		v[ "prename"] = v[ "prename"] .. v[ "id"]
	end
	return rec
end



function run( inp )
	it = inp:table()
	getData = provider.formfunction("getData")
	res = getData( it)
	rt = res:table()
	insertWords = provider.formfunction("insertWords")
	insertWords( { data = rt } )
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
					local nf = provider.normalizer( "normname")
					rt[ k] = nf( v)
				end
			end
		end
		return rt
	end
	local intb = inp:table()
	logger.printc( "INPUT ", intb)
	local outtb = luanorm_table( intb)
	logger.printc( "OUTPUT ", outtb)
	return outtb
end


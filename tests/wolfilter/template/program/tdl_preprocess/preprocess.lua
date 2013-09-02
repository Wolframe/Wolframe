function run( inp )
	it = inp:table()
	getData = provider.formfunction("getData")
	res = getData( it)
	rt = res:table()
	return rt
end

function luanorm( tb)
	rt = {}
	nf = provider.normalizer( "normname")
	for k,v in pairs(tb) do
		if type(v) == "table" then
			rt[ k] = luanorm( v)
		else
			rt[ k] = nf( v)
		end
	end
end


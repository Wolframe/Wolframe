function run( )
	-- parsing and echoing the header
	f = filter( "XML:Header")
	input:as( f)
	output:as( f)
	f = filter( "XML:UTF-8")

	for c,t in input:get() do
		if t == 'encoding' then
			f = filter( "XML:" + c)
		end
		output:print( c,t)
	end
	-- parsing and echoing the content
	input:as( f)
	output:as( f)
	for c,t in input:get() do
		output:print( c,t)
	end
end

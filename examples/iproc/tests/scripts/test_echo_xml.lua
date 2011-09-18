function run( )
	-- parsing and echoing the header
	f = filter( "XML:textwolf")

	-- parsing and echoing the content
	input:as( f)
	output:as( f)
	for c,t in input:get() do
		output:print( c, t)
	end
end

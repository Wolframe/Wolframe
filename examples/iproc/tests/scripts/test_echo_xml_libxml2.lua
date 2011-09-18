function run( )
	f = filter( "xml:libxml2")

	input:as( f)
	output:as( f)

	for c,t in input:get() do
		output:print( c, t)
	end
end

output_encoding = "UTF-8"

function run( )
	inpf = filter( "XML:libxml")
	outf = filter( "XML:" .. output_encoding)

	input:as( inpf)
	output:as( outf)

	for c,t in input:get() do
		if not c then
			output:println( c, t)
		else
			output:print( c, t)
		end
	end
end

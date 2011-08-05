output_encoding = "UTF-8"

function run( )
	-- parsing and echoing the header
	f = filter( "XML:Header")
	input:as( f)
	output:as( f)
	inpf = filter( "XML:UTF-8")
	outf = filter( "XML:" .. output_encoding)

	for c,t in input:get() do
		if t == 'encoding' then
			output:print( output_encoding, 'encoding')
			inpf = filter( "XML:" .. c)
		else
			output:print( c, t)
		end
	end

	-- parsing and echoing the content as defined in output_encoding
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

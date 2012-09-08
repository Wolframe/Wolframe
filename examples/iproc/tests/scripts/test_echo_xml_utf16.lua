function run( )
	inpf = filter( "XML:textwolf")
	outf = filter( "XML:textwolf;UTF-16BE")
	inpf.empty = false

	-- parsing and echoing the content as defined
	input:as( inpf)
	output:as( outf)
	for c,t in input:get() do
		output:print( c, t)
	end
end


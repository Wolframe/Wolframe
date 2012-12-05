function run( )
	inpf = filter( "XML:textwolf")
	outf = filter( "XML:textwolf", "UTF-8")
	inpf.empty = false

	-- parsing and echoing the content as defined
	input:as( inpf)
	output:as( outf)
logger.printc( "BLA");
	for c,t in input:get() do
logger.printc( "ITR ", c, " ", t);
		output:print( c, t)
	end
end


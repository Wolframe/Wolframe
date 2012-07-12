function run()
	f = filter( "line:UTF-8")
	input:as( f)
	output:as( f)
	stack = {}
	for line in input:get() do
		for w in string.gmatch( line, "%S+") do
			logger.print( "INFO", "GET ", w)
			if w == '+' then
				op1=bcdnumber( stack[ #stack -1])
				op2=bcdnumber( stack[ #stack])
				logger.print( "INFO", "PLUS ", op1, op2)
				res = op1 + op2
				output:print( res)
			elseif w == '-' then
				op1=bcdnumber( stack[ #stack -1])
				op2=bcdnumber( stack[ #stack])
				logger.print( "INFO", "MINUS ", op1, op2)
				res = op1 + op2
				output:print( res)
			else
				table.insert( stack, w)
			end
		end
	end
end


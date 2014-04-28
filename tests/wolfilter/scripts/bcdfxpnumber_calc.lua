function run()
	f = provider.filter( "line", "UTF-8")
	input:as( f)
	output:as( f)
	bigfxp = provider.type( "bigfxp")

	stack = {}
	for line in input:get() do
		for w in string.gmatch( line, "%S+") do
			if w == '+' then
				op1 = bigfxp( stack[ #stack -1])
				op2 = bigfxp( stack[ #stack])
				res = op1 + op2
				output:print( res)
			elseif w == '-' then
				op1 = bigfxp( stack[ #stack -1])
				op2 = bigfxp( stack[ #stack])
				res = op1 - op2
				output:print( res)
			elseif w == '*' then
				op1 = bigfxp( stack[ #stack -1])
				op2 = bigfxp( stack[ #stack])
				res = op1 * op2
				output:print( res)
			elseif w == '/' then
				op1 = bigfxp( stack[ #stack -1])
				op2 = bigfxp( stack[ #stack])
				op1.divide( op2, 3);
				output:print( res)
			elseif w == '==' then
				op1 = bigfxp( stack[ #stack -1])
				op2 = bigfxp( stack[ #stack])
				if op1 == op2 then
					res = '1'
				else
					res = '0'
				end
				output:print( res)
			elseif w == '!=' then
				op1 = bigfxp( stack[ #stack -1])
				op2 = bigfxp( stack[ #stack])
				if op1 ~= op2 then
					res = '1'
				else
					res = '0'
				end
				output:print( res)
			elseif w == '>=' then
				op1 = bigfxp( stack[ #stack -1])
				op2 = bigfxp( stack[ #stack])
				if op1 >= op2 then
					res = '1'
				else
					res = '0'
				end
				output:print( res)
			elseif w == '<=' then
				op1 = bigfxp( stack[ #stack -1])
				op2 = bigfxp( stack[ #stack])
				if op1 <= op2 then
					res = '1'
				else
					res = '0'
				end
				output:print( res)
			elseif w == '<' then
				op1 = bigfxp( stack[ #stack -1])
				op2 = bigfxp( stack[ #stack])
				if op1 < op2 then
					res = '1'
				else
					res = '0'
				end
				output:print( res)
			elseif w == '>' then
				op1 = bigfxp( stack[ #stack -1])
				op2 = bigfxp( stack[ #stack])
				if op1 > op2 then
					res = '1'
				else
					res = '0'
				end
				output:print( res)
			else
				table.insert( stack, w)
			end
		end
	end
end



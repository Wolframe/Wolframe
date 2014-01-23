**
**requires:TEXTWOLF
**requires:LUA
**input
2005-04-01 67 +
1968-11-18 8975 +
1989-07-09 1 +
1973-11-18 113 -
2013-04-27 1 -
1767-12-01 1873-05-13 diff
2014-07-07 1942-03-04 diff
2014-07-07 2014-07-06 ==
2014-07-07 2014-07-07 ==
2014-07-07 2014-07-08 ==
2014-07-07 2014-07-06 !=
2014-07-07 2014-07-07 !=
2014-07-07 2014-07-08 !=
2014-07-07 2014-07-06 >=
2014-07-07 2014-07-07 >=
2014-07-07 2014-07-08 >=
2014-07-07 2014-07-06 <=
2014-07-07 2014-07-07 <=
2014-07-07 2014-07-08 <=
2014-07-07 2014-07-06 >
2014-07-07 2014-07-07 >
2014-07-07 2014-07-08 >
2014-07-07 2014-07-06 <
2014-07-07 2014-07-07 <
2014-07-07 2014-07-08 <
**output
2005-06-07
1993-06-15
1989-07-10
1973-07-28
2013-04-26
-38514
26423
0
1
0
1
0
1
1
1
0
0
1
1
1
0
0
0
0
1
**file:date_calc.lua

function run()
	f = provider.filter( "line", "UTF-8")
	input:as( f)
	output:as( f)
	stack = {}
	date = provider.type( "dta:date")

	for line in input:get() do
		for w in string.gmatch( line, "%S+") do
			if w == '+' then
				op1 = date( stack[ #stack -1])
				op2 = stack[ #stack]
				res = op1 + op2
				output:print( res)
			elseif w == '-' then
				op1 = date( stack[ #stack -1])
				op2 = stack[ #stack]
				res = op1 - op2
				output:print( res)
			elseif w == 'diff' then
				op1 = date( stack[ #stack -1])
				op2 = date( stack[ #stack])
				res = op1 - op2
				output:print( res)
			elseif w == '==' then
				op1 = date( stack[ #stack -1])
				op2 = date( stack[ #stack])
				if op1 == op2 then
					res = '1'
				else
					res = '0'
				end
				output:print( res)
			elseif w == '!=' then
				op1 = date( stack[ #stack -1])
				op2 = date( stack[ #stack])
				if op1 ~= op2 then
					res = '1'
				else
					res = '0'
				end
				output:print( res)
			elseif w == '>=' then
				op1 = date( stack[ #stack -1])
				op2 = date( stack[ #stack])
				if op1 >= op2 then
					res = '1'
				else
					res = '0'
				end
				output:print( res)
			elseif w == '<=' then
				op1 = date( stack[ #stack -1])
				op2 = date( stack[ #stack])
				if op1 <= op2 then
					res = '1'
				else
					res = '0'
				end
				output:print( res)
			elseif w == '<' then
				op1 = date( stack[ #stack -1])
				op2 = date( stack[ #stack])
				if op1 < op2 then
					res = '1'
				else
					res = '0'
				end
				output:print( res)
			elseif w == '>' then
				op1 = date( stack[ #stack -1])
				op2 = date( stack[ #stack])
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


**config
--module ../../src/modules/filter/line/mod_filter_line
--module ../../src/modules/cmdbind/lua/mod_command_lua
--module ../../src/modules/datatype/datetime/mod_datatype_datetime
--cmdprogram date_calc.lua run
**end

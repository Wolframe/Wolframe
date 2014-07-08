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
DTVAL_DD 20051218 Y2005 M12 D18
DTVAL_SS 19681118045601 Y1968 M11 D18 h4 m56 s1
DTVAL_LL 20060513160018987 Y2006 M5 D13 h16 m0 s18 l987
DTVAL_CC 18760707235961934123 Y1876 M7 D7 h23 m59 s61 l934 c123
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
	datetime = provider.type( "datetime")
	dtval_dd = datetime("20051218")
	dtval_ss = datetime("19681118045601")
	dtval_ll = datetime("20060513160018987")
	dtval_cc = datetime("18760707235961934123")
	output:print( "DTVAL_DD " .. dtval_dd:__tostring() .. " Y" .. dtval_dd:year() .. " M" .. dtval_dd:month() .. " D" .. dtval_dd:day());
	output:print( "DTVAL_SS " .. dtval_ss:__tostring() .. " Y" .. dtval_ss:year() .. " M" .. dtval_ss:month() .. " D" .. dtval_ss:day() .. " h" .. dtval_ss:hour() .. " m" .. dtval_ss:minute() .. " s" .. dtval_ss:second());
	output:print( "DTVAL_LL " .. dtval_ll:__tostring() .. " Y" .. dtval_ll:year() .. " M" .. dtval_ll:month() .. " D" .. dtval_ll:day() .. " h" .. dtval_ll:hour() .. " m" .. dtval_ll:minute() .. " s" .. dtval_ll:second() .. " l" .. dtval_ll:millisecond()); 
	output:print( "DTVAL_CC " .. dtval_cc:__tostring() .. " Y" .. dtval_cc:year() .. " M" .. dtval_cc:month() .. " D" .. dtval_cc:day() .. " h" .. dtval_cc:hour() .. " m" .. dtval_cc:minute() .. " s" .. dtval_cc:second() .. " l" .. dtval_cc:millisecond() .. " c" .. dtval_cc:microsecond()); 
	date = provider.type( "date")

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
--config wolframe.conf run
**file:wolframe.conf
LoadModules
{
	module ../../src/modules/filter/line/mod_filter_line
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../../src/modules/datatype/datetime/mod_datatype_datetime
}
Processor
{
	cmdhandler
	{
		lua
		{
			program date_calc.lua
		}
	}
}
**end

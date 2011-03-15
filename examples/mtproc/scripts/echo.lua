-- readTable returns the table desribed by argument (propertytree,XML,etc..)
-- @param itr :generator function (iterator with its closure):
function readTable( itr)
	local table = {};
	for t,v in itr() do table[t] = v or readTable( itr) end
	return table;
end

config = {};
function readConfig( itr)
	config = readTable( itr);
end

-- does the echo does not expect arguments
function echo()
	filter = require "filter.Char.IsoLatin1";
	input.as filter.get;
	output.as filter.put;
	for ch in input.read() do output.write( ch);
end


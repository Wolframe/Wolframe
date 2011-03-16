-- predefined objects by the system
-- 1) input
-- 2) output
-- 3) "object pool" (not used yet here)


-- readTable returns the table desribed by argument (propertytree,XML,etc..)
-- @param itr :generator function (iterator with its closure):
function readTable( itr)
	local table = {}

	-- index(): generator to assign content that is not labeled with an attribute name. not really needed for config, but XML
	local i = 0
	function index()
		i=i+1
		return i
	end

	-- recursive assignement of values as
	-- 1) tables to tags
	-- 2) values to attributes
	-- 3) content enumerated from 0:
	for t,v in itr() do table[t or index()] = v or readTable( itr) end
	return table
end

config = {}
function readConfig( itr)
	-- read config as table with the content of "/main/sub/A" addressable as config["main"]["sub"]["A"]
	config = readTable( itr)
end

-- does the echo does not expect arguments
function echo()
	-- load the generator with closure type from a loadable module
	filter = require "filter.Char.IsoLatin1"

	-- define the filter functions of I/O
	input.as filter.get
	output.as filter.put

	-- do the echo (no real echo, because not buffering lines)
	for ch in input.read() do output.write( ch) end
end


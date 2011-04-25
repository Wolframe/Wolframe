-- predefined objects by the system
-- 1) input
-- 2) output
-- 3) function filter( name: string)


-- [readTable] returns the table desribed by argument (propertytree,XML,etc..)
-- @param itr :generator function (iterator with its closure):
function readTable( itr)
	local table = {}

	-- index(): generator to assign content that is not labeled with an attribute name. not really needed for config, but XML
	local i = -1
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

function readInput( itr)
	return readTable( itr)
end

-- [run] does the processing (echo input)
-- this example does not expect arguments
function run_echo()
	-- load the generator with closure type from a loadable module
	iof = filter( "filter.char.IsoLatin1")

	-- define the filter functions of I/O
	input.as iof
	output.as iof

	-- do the echo (no real echo, because not buffering lines)
	for ch in input.get() do output.print( ch) end
end


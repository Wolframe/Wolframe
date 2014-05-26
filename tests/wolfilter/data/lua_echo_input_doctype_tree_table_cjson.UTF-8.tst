**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
{
	"-doctype" : "trees",
	"mtree": {
		"-id": "1",
		"name": "eins",
		"node": [
			{
				"-id": "11",
				"name": "Eins eins"
			},
			{
				"-id": "12",
				"name": "Eins zwei",
				"node": {
					"-id": "121",
					"name": "Eins zwei eins"
				}
			},
			{
				"-id": "13",
				"name": "Eins drei"
			}
		]
	},
	"btree": {
		"-id": "1",
		"name": "eins",
		"left": {
			"-id": "11",
			"name": "Eins eins"
		},
		"right": {
			"-id": "12",
			"name": "Eins zwei",
			"left": {
				"-id": "121",
				"name": "Eins zwei eins"
			}
		}
	}
}**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson -c wolframe.conf run

**file:wolframe.conf
LoadModules
{
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform
	module ../../src/modules/normalize/number/mod_normalize_number
	module ../../src/modules/normalize/string/mod_normalize_string
	module ../../src/modules/datatype/bcdnumber/mod_datatype_bcdnumber
}
Processor
{
	program		normalize.wnmp
	program		form.sfrm
	cmdhandler
	{
		lua
		{
			program script.lua
		}
	}
}
**file:normalize.wnmp
int=integer;
uint=unsigned;
float=floatingpoint ;
currency= bigfxp(   2  );
percent_1=bigfxp(  2  );
normname =convdia,ucname;
**file:form.sfrm
STRUCT MulTreeNode
{
	id		@int
	name		normname
	node		^MulTreeNode[]
}

STRUCT BinTreeNode
{
	id		@int
	name		normname
	left		^BinTreeNode
	right		^BinTreeNode
}

FORM trees
	-root root
{
	mtree	MulTreeNode
	btree	BinTreeNode
}
**file:script.lua
function printTable( tab)
	-- deterministic print of a table (since lua 5.2.1 table keys order is non deterministic)

	keys = {}
	for key,val in pairs( tab) do
		table.insert( keys, key)
	end
	table.sort( keys)

	for i,t in ipairs( keys) do
		local v = tab[ t]

		if type(v) == "table" then
			if v[ #v] then
				-- print array (keys are indices)
				for eidx,elem in ipairs( v) do
					output:opentag( t)
					if type(elem) == "table" then
						printTable( elem)
					else
						output:print( elem)
					end
					output:closetag()
				end
			else
				-- print table (keys are values)
				output:opentag( t)
				printTable( v)
				output:closetag()
			end
		else
			output:opentag( t)
			output:print( v)
			output:closetag()
		end
	end
end

function run()
	local doctype = input:doctype()
	output:as( provider.filter(), doctype)
	printTable( input:table())
end
**output
{
	"-doctype":	"trees",
	"btree":	{
		"id":	"1",
		"left":	{
			"id":	"11",
			"name":	"EINS EINS"
		},
		"name":	"EINS",
		"right":	{
			"id":	"12",
			"left":	{
				"id":	"121",
				"name":	"EINS ZWEI EINS"
			},
			"name":	"EINS ZWEI"
		}
	},
	"mtree":	{
		"id":	"1",
		"name":	"EINS",
		"node":	[{
				"id":	"11",
				"name":	"EINS EINS"
			}, {
				"id":	"12",
				"name":	"EINS ZWEI",
				"node":	{
					"id":	"121",
					"name":	"EINS ZWEI EINS"
				}
			}, {
				"id":	"13",
				"name":	"EINS DREI"
			}]
	}
}
**end

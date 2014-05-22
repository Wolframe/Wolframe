**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
{
	"assignment":	[{
			"task":	[{
					"title":	"job 1",
					"key":	"A123",
					"customernumber":	"324"
				}, {
					"title":	"job 2",
					"key":	"V456",
					"customernumber":	"567"
				}],
			"employee":	{
				"firstname":	"Julia",
				"surname":	"Tegel-Sacher",
				"phone":	"098 765 43 21"
			},
			"issuedate":	"13.5.2006"
		}, {
			"task":	[{
					"title":	"job 3",
					"key":	"A456",
					"customernumber":	"567"
				}, {
					"title":	"job 4",
					"key":	"V789",
					"customernumber":	"890"
				}],
			"employee":	{
				"firstname":	"Jakob",
				"surname":	"Stegelin",
				"phone":	"012 345 67 89"
			},
			"issuedate":	"13.5.2006"
		}]
}**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson -c wolframe.conf run

**file:wolframe.conf
LoadModules
{
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../wolfilter/modules/employee_assignment_convert/mod_employee_assignment_convert
}
Processor
{
	cmdhandler
	{
		lua
		{
			program script.lua
		}
	}
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
	t = input:table()
	r = provider.formfunction("employee_assignment_convert")( t)
	printTable( r:table())
end
**requires:DISABLED NETBSD

**output
{
	"assignment":	[{
			"employee":	{
				"firstname":	"juulliiaa",
				"phone":	"901 234 56 78",
				"surname":	"teeggeell-saacchheerr"
			},
			"issuedate":	"8647993",
			"task":	[{
					"customernumber":	"325",
					"key":	"a876",
					"title":	"jjoobb 8"
				}, {
					"customernumber":	"568",
					"key":	"v543",
					"title":	"jjoobb 7"
				}]
		}, {
			"employee":	{
				"firstname":	"jaakkoobb",
				"phone":	"987 654 32 10",
				"surname":	"stteeggeelliinn"
			},
			"issuedate":	"8647993",
			"task":	[{
					"customernumber":	"568",
					"key":	"a543",
					"title":	"jjoobb 6"
				}, {
					"customernumber":	"891",
					"key":	"v210",
					"title":	"jjoobb 5"
				}]
		}]
}
**end

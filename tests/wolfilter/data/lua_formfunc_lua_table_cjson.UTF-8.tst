**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
{
	"assignmentlist":	{
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
	}
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
	program program_formfunc.lua
	cmdhandler
	{
		lua
		{
			program script.lua
		}
	}
}
**file:program_formfunc.lua
function toupper_table( tb)
	for i,v in pairs(tb)
	do
		if type(v) == "table" then
			tb[ i] = toupper_table(v)
		else
			tb[ i] = string.upper(v)
		end
	end
	return tb
end

function toupper_formfunc( inp)
	return toupper_table( inp:table())
end
**file:script.lua

function run()
	res = provider.formfunction( "toupper_formfunc")( input:table())
	output:print( res:table())
end
**requires:DISABLED NETBSD

**output
{
	"assignmentlist":	{
		"assignment":	[{
				"issuedate":	"13.5.2006",
				"employee":	{
					"firstname":	"JULIA",
					"phone":	"098 765 43 21",
					"surname":	"TEGEL-SACHER"
				},
				"task":	[{
						"customernumber":	"324",
						"key":	"A123",
						"title":	"JOB 1"
					}, {
						"customernumber":	"567",
						"key":	"V456",
						"title":	"JOB 2"
					}]
			}, {
				"issuedate":	"13.5.2006",
				"employee":	{
					"firstname":	"JAKOB",
					"phone":	"012 345 67 89",
					"surname":	"STEGELIN"
				},
				"task":	[{
						"customernumber":	"567",
						"key":	"A456",
						"title":	"JOB 3"
					}, {
						"customernumber":	"890",
						"key":	"V789",
						"title":	"JOB 4"
					}]
			}]
	}
}
**end

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
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson  --module ../../src/modules/cmdbind/lua/mod_command_lua --module ../wolfilter/modules//employee_assignment_convert//mod_employee_assignment_convert --cmdprogram formfunc_table.lua run

**requires:DISABLED NETBSD

**file: formfunc_table.lua

function run()
	t = input:table()
	r = formfunction("employee_assignment_convert")( t)
	output:print( r:table())
end

**output
{
	"assignmentlist":	{
		"assignment":	[{
				"issuedate":	"8647993",
				"employee":	{
					"firstname":	"juulliiaa",
					"phone":	"901 234 56 78",
					"surname":	"teeggeell-saacchheerr"
				},
				"task":	[{
						"customernumber":	"325",
						"title":	"jjoobb 8",
						"key":	"a876"
					}, {
						"customernumber":	"568",
						"title":	"jjoobb 7",
						"key":	"v543"
					}]
			}, {
				"issuedate":	"8647993",
				"employee":	{
					"firstname":	"jaakkoobb",
					"phone":	"987 654 32 10",
					"surname":	"stteeggeelliinn"
				},
				"task":	[{
						"customernumber":	"568",
						"title":	"jjoobb 6",
						"key":	"a543"
					}, {
						"customernumber":	"891",
						"title":	"jjoobb 5",
						"key":	"v210"
					}]
			}]
	}
}
**end

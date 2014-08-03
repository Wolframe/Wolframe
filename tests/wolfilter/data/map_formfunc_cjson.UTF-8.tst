**
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
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson --module ../../src/modules/doctype/json/mod_doctype_json --module ../wolfilter/modules//employee_assignment_convert//mod_employee_assignment_convert employee_assignment_convert

**requires:DISABLED NETBSD

**output
{
	"assignment":	[{
			"issuedate":	"8647993",
			"employee":	{
				"firstname":	"juulliiaa",
				"surname":	"teeggeell-saacchheerr",
				"phone":	"901 234 56 78"
			},
			"task":	[{
					"key":	"a876",
					"title":	"jjoobb 8",
					"customernumber":	"325"
				}, {
					"key":	"v543",
					"title":	"jjoobb 7",
					"customernumber":	"568"
				}]
		}, {
			"issuedate":	"8647993",
			"employee":	{
				"firstname":	"jaakkoobb",
				"surname":	"stteeggeelliinn",
				"phone":	"987 654 32 10"
			},
			"task":	[{
					"key":	"a543",
					"title":	"jjoobb 6",
					"customernumber":	"568"
				}, {
					"key":	"v210",
					"title":	"jjoobb 5",
					"customernumber":	"891"
				}]
		}]
}
**end

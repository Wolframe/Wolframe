**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
{
	"-doctype":	"employee_assignment_print",
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
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson --module ../../src/modules/doctype/json/mod_doctype_json -c wolframe.conf run

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
			filter cjson
		}
	}
}
**file:normalize.wnmp
iNt=integer( 10);
uint=unsigneD(10 );
float=fLoatingpoint(10,  10);
currency=bIgfXp( 2);
percent_1=Bigfxp( 2);
**file:form.sfrm
FORM Employee
{
	firstname string
	surname string
	phone string
}

FORM employee_assignment_print
	-root assignmentlist
{
	assignment []
	{
		task []
		{
			title string
			key string
			customernumber int
		}
		employee Employee
		issuedate string
	}
}
**file:script.lua
function run()
	f = input:form()
	output:as( provider.filter(), f:name())
	output:print( f:get())
end
**requires:DISABLED NETBSD

**output
{
	"-doctype":	"employee_assignment_print",
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
**end

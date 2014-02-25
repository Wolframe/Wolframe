**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
{
  "doctype" : "employee_assignment_print",
  "assignmentlist": {
    "assignment": [
      {
        "task": [
          {
            "title": "job 1",
            "key": "A123",
            "customernumber": "324"
          },
          {
            "title": "job 2",
            "key": "V456",
            "customernumber": "567"
          }
        ],
        "employee": {
          "firstname": "Julia",
          "surname": "Tegel-Sacher",
          "phone": "098 765 43 21"
        },
        "issuedate": "13.5.2006"
      },
      {
        "task": {
          "title": "job 4",
          "key": "V789",
          "customernumber": "890"
        },
        "employee": {
          "firstname": "Jakob",
          "surname": "Stegelin",
          "phone": "012 345 67 89"
        },
        "issuedate": "13.5.2006"
      }
    ]
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
int=trim,integer(10 );
uint=trim,unsigned(10);
float=trim, floatingpoint( 10,10);
currency=fixedpoint(13, 2);
percent_1=fixedpoint(5 ,1);
**file:form.sfrm
FORM Employee
{
	firstname string
	surname string
	phone string
}

FORM employee_assignment_print
{
	assignmentlist
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
}
**file:script.lua
function run()
	type = input:doctype()
	output:as( provider.filter(), type)
	output:print( input:table())
end
**requires:DISABLED NETBSD

**output
{
	"doctype":	"employee_assignment_print",
	"assignmentlist":	{
		"assignment":	[{
				"issuedate":	"13.5.2006",
				"employee":	{
					"firstname":	"Julia",
					"phone":	"098 765 43 21",
					"surname":	"Tegel-Sacher"
				},
				"task":	[{
						"customernumber":	"324",
						"key":	"A123",
						"title":	"job 1"
					}, {
						"customernumber":	"567",
						"key":	"V456",
						"title":	"job 2"
					}]
			}, {
				"issuedate":	"13.5.2006",
				"employee":	{
					"firstname":	"Jakob",
					"phone":	"012 345 67 89",
					"surname":	"Stegelin"
				},
				"task":	[{
						"customernumber":	"890",
						"key":	"V789",
						"title":	"job 4"
					}]
			}]
	}
}
**end

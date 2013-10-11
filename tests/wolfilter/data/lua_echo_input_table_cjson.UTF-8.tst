**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
{
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
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson  --module ../../src/modules/cmdbind/lua/mod_command_lua --cmdprogram echo_input_table.lua run
**file: echo_input_table.lua
function run()
	t = input:table()
	output:print( t)
end
**output
{
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
				"task":	{
					"customernumber":	"890",
					"key":	"V789",
					"title":	"job 4"
				}
			}]
	}
}
**end

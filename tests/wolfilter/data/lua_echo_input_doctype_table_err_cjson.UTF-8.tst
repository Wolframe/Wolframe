**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**exception
try to read input twice
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
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson  --module ../../src/modules/cmdbind/lua/mod_command_lua --module ../../src/modules/ddlcompiler//simpleform/mod_ddlcompiler_simpleform --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/normalize//string/mod_normalize_string --program simpleform_range_trim.wnmp --program employee_assignment_print.sfrm --cmdprogram echo_input_doctype_table_err.lua run

**file:simpleform_range_trim.wnmp
int=string:trim,number:integer(10);
uint=string:trim,number:unsigned(10);
float=string:trim,number:float(10,10);
currency=number:fixedpoint(13,2);
percent_1=number:fixedpoint(5,1);
**requires:DISABLED NETBSD

**file: echo_input_doctype_table_err.lua
function run()
	type = input:doctype()
	output:as( filter(), type)
	inp = input:table()
	inp_ = input:table()
	output:print( inp)
end
**file: employee_assignment_print.sfrm
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

**output
**end

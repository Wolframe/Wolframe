**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
{
  "doctype" : "trees",
  "root": {
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
  }
}**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson  --module ../../src/modules/cmdbind/lua/mod_command_lua --module ../../src/modules/ddlcompiler//simpleform/mod_ddlcompiler_simpleform --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/normalize//string/mod_normalize_string --program simpleform_string.wnmp --program trees.sfrm --cmdprogram echo_input_doctype_table.lua run

**file:simpleform_string.wnmp
int=integer;
uint=unsigned;
float=float ;
currency= fixedpoint(13,2);
percent_1=fixedpoint( 5 ,1);
normname =convdia,ucname;
**file: echo_input_doctype_table.lua
function run()
	type = input:doctype()
	output:as( filter(), type)
	output:print( input:table())
end
**file: trees.sfrm
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
{
	root
	{
		mtree	MulTreeNode
		btree	BinTreeNode
	}
}


**output
{
	"doctype":	"trees",
	"root":	{
		"mtree":	{
			"id":	"1",
			"name":	"EINS",
			"node":	[{
					"id":	"11",
					"name":	"EINS EINS"
				}, {
					"id":	"12",
					"name":	"EINS ZWEI",
					"node":	[{
							"id":	"121",
							"name":	"EINS ZWEI EINS"
						}]
				}, {
					"id":	"13",
					"name":	"EINS DREI"
				}]
		},
		"btree":	{
			"id":	"1",
			"right":	{
				"id":	"12",
				"name":	"EINS ZWEI",
				"left":	{
					"id":	"121",
					"name":	"EINS ZWEI EINS"
				}
			},
			"name":	"EINS",
			"left":	{
				"id":	"11",
				"name":	"EINS EINS"
			}
		}
	}
}
**end

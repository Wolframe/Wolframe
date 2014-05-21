**
**requires:CJSON
**requires:TEXTWOLF
**input
{
    "doctype" : "CDCatalog",
    "CD": [
      {
        "TITLE": "Empire Burlesque",
        "ARTIST": "Bob Dylan",
        "RELEASE": {
          "COUNTRY": "USA",
          "COMPANY": "Columbia",
          "PRICE": "10.90",
          "YEAR": "1985"
        }
      },
      {
        "TITLE": "El Rei el Cantinero",
        "ARTIST": "Jose Alfredo Jimenez",
        "RELEASE": {
          "COUNTRY": "Mexico",
          "COMPANY": "RCA",
          "PRICE": "15.90",
          "YEAR": "1958"
        }
      },
      {
        "TITLE": "Still got the blues",
        "ARTIST": "Gary Moore",
        "RELEASE": {
          "COUNTRY": "UK",
          "COMPANY": "Virgin records",
          "PRICE": "10.20",
          "YEAR": "1990"
        }
      },
      {
        "TITLE": "Eros",
        "ARTIST": "Eros Ramazzotti",
        "RELEASE": {
          "COUNTRY": "EU",
          "COMPANY": "BMG",
          "PRICE": "9.90",
          "YEAR": "1997"
        }
      },
      {
        "TITLE": "One night only",
        "ARTIST": "Bee Gees",
        "RELEASE": {
          "COUNTRY": "UK",
          "COMPANY": "Polydor",
          "PRICE": "10.90",
          "YEAR": "1998"
        }
      },
      {
        "TITLE": "Sylvias Mother",
        "ARTIST": "Dr.Hook",
        "RELEASE": {
          "COUNTRY": "UK",
          "COMPANY": "CBS",
          "PRICE": "8.10",
          "YEAR": "1973"
        }
      },
      {
        "TITLE": "Romanza",
        "ARTIST": "Andrea Bocelli",
        "RELEASE": {
          "COUNTRY": "EU",
          "COMPANY": "Polydor",
          "PRICE": "10.80",
          "YEAR": "1996"
        }
      }
    ]
}**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson -c wolframe.conf getCDCatalog

**file:wolframe.conf
LoadModules
{
	module ./../../src/modules/cmdbind/directmap/mod_command_directmap
	module ./../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform
	module ./../../src/modules/normalize/number/mod_normalize_number
	module ./../../src/modules/normalize/string/mod_normalize_string
	module ./../wolfilter/modules/cmdbind/mylang/mod_command_mylang
}
Processor
{
	program ../wolfilter/template/program/mylang/example.mlg
	program ../wolfilter/template/program/mylang/example.sfrm
	program ../wolfilter/template/program/mylang/example.wnmp
	
	cmdhandler
	{
		directmap
		{
			program ../wolfilter/template/program/mylang/example.dmap
			filter cjson
		}
	}
}
**output
{
    "doctype":  "CDCatalog",
    "CD":  [{
        "TITLE":  "EMPIRE BURLESQUE",
        "ARTIST":  "BOB DYLAN",
        "RELEASE":  {
          "COUNTRY":  "USA",
          "COMPANY":  "COLUMBIA",
          "PRICE":  "10.90",
          "YEAR":  "1986"
        }
      }, {
        "TITLE":  "EL REI EL CANTINERO",
        "ARTIST":  "JOSE ALFREDO JIMENEZ",
        "RELEASE":  {
          "COUNTRY":  "MEXICO",
          "COMPANY":  "RCA",
          "PRICE":  "15.90",
          "YEAR":  "1959"
        }
      }, {
        "TITLE":  "STILL GOT THE BLUES",
        "ARTIST":  "GARY MOORE",
        "RELEASE":  {
          "COUNTRY":  "UK",
          "COMPANY":  "VIRGIN RECORDS",
          "PRICE":  "10.20",
          "YEAR":  "1991"
        }
      }, {
        "TITLE":  "EROS",
        "ARTIST":  "EROS RAMAZZOTTI",
        "RELEASE":  {
          "COUNTRY":  "EU",
          "COMPANY":  "BMG",
          "PRICE":  "9.90",
          "YEAR":  "1998"
        }
      }, {
        "TITLE":  "ONE NIGHT ONLY",
        "ARTIST":  "BEE GEES",
        "RELEASE":  {
          "COUNTRY":  "UK",
          "COMPANY":  "POLYDOR",
          "PRICE":  "10.90",
          "YEAR":  "1999"
        }
      }, {
        "TITLE":  "SYLVIAS MOTHER",
        "ARTIST":  "DR.HOOK",
        "RELEASE":  {
          "COUNTRY":  "UK",
          "COMPANY":  "CBS",
          "PRICE":  "8.10",
          "YEAR":  "1974"
        }
      }, {
        "TITLE":  "ROMANZA",
        "ARTIST":  "ANDREA BOCELLI",
        "RELEASE":  {
          "COUNTRY":  "EU",
          "COMPANY":  "POLYDOR",
          "PRICE":  "10.80",
          "YEAR":  "1997"
        }
      }]
}
**end

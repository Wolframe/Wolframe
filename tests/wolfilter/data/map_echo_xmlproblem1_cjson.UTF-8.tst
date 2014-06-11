**
**requires:CJSON
**requires:TEXTWOLF
**input
{
	"-doctype" : "CategoryHierarchyRequest",
	"-id": "1"
}**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson --module ../../src/modules/doctype/json/mod_doctype_json - 
**output
{
	"id":	"1"
}
**end

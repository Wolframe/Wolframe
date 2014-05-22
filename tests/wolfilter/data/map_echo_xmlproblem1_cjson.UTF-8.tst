**
**requires:CJSON
**requires:TEXTWOLF
**input
{
	"doctype" : "CategoryHierarchyRequest",
	"category": { "-id": "1" }
}**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson - 
**output
{
	"id":	"1"
}
**end

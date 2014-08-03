**
**requires:LUA
**requires:LIBXML2
**input
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE person SYSTEM 'Person'>
<person><name>Hugi Turbo</name><address>Gurkenstrasse 7a 3145 Gumligen</address></person>**config
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2 --module ../../src/modules/doctype/xml/mod_doctype_xml -c wolframe.conf run

**file:wolframe.conf
LoadModules
{
	module ./../../src/modules/cmdbind/lua/mod_command_lua
}
Processor
{
	cmdhandler
	{
		lua
		{
			program no_operation.lua
			filter libxml2
		}
	}
}
**file:no_operation.lua
function run( input )
end
**output
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE person SYSTEM "Person"><person/>
**end

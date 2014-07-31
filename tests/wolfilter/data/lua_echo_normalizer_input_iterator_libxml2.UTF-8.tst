**
**requires:LUA
**requires:LIBXML2
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc>
	<item>Im Französischen gibt es Buchstaben wie "ç" "è" "à" "ë" (e tréma)</item>
	<item>Im Deutschen sind es die Umlate wie "Ü" "Ä" "Ö"</item>
</doc>**config
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2 --module ../../src/modules/doctype/xml/mod_doctype_xml -c wolframe.conf run

**file:wolframe.conf
LoadModules
{
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../../src/modules/normalize/string/mod_normalize_string
}
Processor
{
	program normalize.wnmp
	cmdhandler
	{
		lua
		{
			program script.lua
			filter libxml2
		}
	}
}
**file:script.lua
function run()
	for v,t in input:get()
	do
		if not t and v then
			output:print( provider.type("name")(v),t)
		else
			output:print( v,t)
		end
	end
end
**file:normalize.wnmp
name=ucname;

**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><item>IM FRANZOSISCHEN GIBT ES BUCHSTABEN WIE C E A E E TREMA</item><item>IM DEUTSCHEN SIND ES DIE UMLATE WIE U A O</item></doc>
**end

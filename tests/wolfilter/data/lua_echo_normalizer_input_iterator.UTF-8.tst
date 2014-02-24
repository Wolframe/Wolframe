**
**requires:LUA
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc>
	<item>Im Französischen gibt es Buchstaben wie "ç" "è" "à" "ë" (e tréma)</item>
	<item>Im Deutschen sind es die Umlate wie "Ü" "Ä" "Ö"</item>
</doc>**config
--input-filter textwolf --output-filter textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf -c wolframe.conf run
**requires:TEXTWOLF
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
		}
	}
}
**file:script.lua
function run()
	for v,t in input:get()
	do
		if not t and v then
			output:print( normalizer("name")(v),t)
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

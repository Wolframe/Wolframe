**
**requires:LUA
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc>
	<item>Im Französischen gibt es Buchstaben wie "ç" "è" "à" "ë" (e tréma)</item>
	<item>Im Deutschen sind es die Umlate wie "Ü" "Ä" "Ö"</item>
</doc>**config
--input-filter xml:textwolf --output-filter xml:textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf  --module ../../src/modules/cmdbind/lua/mod_command_lua --module ../../src/modules/normalize//string/mod_normalize_string --program types.normalize --cmdprogram echo_normalizer_input_iterator.lua run

**file:types.normalize
name=string:ucname;

**file: echo_normalizer_input_iterator.lua
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

**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><item>IM FRANZOSISCHEN GIBT ES BUCHSTABEN WIE C E A E E TREMA</item><item>IM DEUTSCHEN SIND ES DIE UMLATE WIE U A O</item></doc>
**end

**
**requires:LUA
**requires:LIBXML2
**input
< ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U C S - 2 L E "   s t a n d a l o n e = " n o " ? > 
 < ! D O C T Y P E   p e r s o n   S Y S T E M   ' P e r s o n ' > 
 < p e r s o n > < n a m e > H u g i   T u r b o < / n a m e > < a d d r e s s > G u r k e n s t r a s s e   7 a   3 1 4 5   G u m l i g e n < / a d d r e s s > < / p e r s o n > **config
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
< ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U C S - 2 L E "   s t a n d a l o n e = " n o " ? > 
 < ! D O C T Y P E   p e r s o n   S Y S T E M   " P e r s o n " > < p e r s o n / > 
 **end

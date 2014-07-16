**
**requires:LIBXML2
**input
< ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 L E "   s t a n d a l o n e = " n o " ? > 
 < ! D O C T Y P E   d o c   S Y S T E M   " U s e r D a t a . s i m p l e f o r m " > < d o c > < d a t a > b l a < / d a t a > < c o m m a n d > e x e c < / c o m m a n d > < / d o c > **config
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2 --module ../../src/modules/doctype/xml/mod_doctype_xml -c wolframe.conf echoUserData

**file:wolframe.conf
LoadModules
{
	module ./../../src/modules/cmdbind/directmap/mod_command_directmap
	module ./../../src/modules/cmdbind/lua/mod_command_lua
	module ./../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform
}
Processor
{
	program echo_input_table.lua
	program userdata.sfrm
	cmdhandler
	{
		directmap
		{
			program test.dmap
			filter libxml2
		}
	}
}
**file: test.dmap
COMMAND (echo UserData) CALL run CONTEXT {uname=UserName, host=RemoteHost} RETURN UserData;
**file:userdata.sfrm
FORM UserData
	-root doc
{
	uname string
	host string
	data string
	command string
}
**file:echo_input_table.lua
function run( input)
	return input:get()
end

**output
< ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 L E "   s t a n d a l o n e = " n o " ? > 
 < ! D O C T Y P E   d o c   S Y S T E M   " U s e r D a t a . s i m p l e f o r m " > < d o c > < u n a m e > w o l f i l t e r < / u n a m e > < h o s t > 1 2 3 . 1 2 3 . 1 2 3 . 1 2 3 < / h o s t > < d a t a > b l a < / d a t a > < c o m m a n d > e x e c < / c o m m a n d > < / d o c > 
 **end

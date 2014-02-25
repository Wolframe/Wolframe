**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
{ 
     " d o c t y p e "   :   " e m p l o y e e _ a s s i g n m e n t _ p r i n t " , 
     " a s s i g n m e n t l i s t " :   { 
         " a s s i g n m e n t " :   [ 
             { 
                 " t a s k " :   [ 
                     { 
                         " t i t l e " :   " j o b   1 " , 
                         " k e y " :   " A 1 2 3 " , 
                         " c u s t o m e r n u m b e r " :   " 3 2 4 " 
                     } , 
                     { 
                         " t i t l e " :   " j o b   2 " , 
                         " k e y " :   " V 4 5 6 " , 
                         " c u s t o m e r n u m b e r " :   " 5 6 7 " 
                     } 
                 ] , 
                 " e m p l o y e e " :   { 
                     " f i r s t n a m e " :   " J u l i a " , 
                     " s u r n a m e " :   " T e g e l - S a c h e r " , 
                     " p h o n e " :   " 0 9 8   7 6 5   4 3   2 1 " 
                 } , 
                 " i s s u e d a t e " :   " 1 3 . 5 . 2 0 0 6 " 
             } , 
             { 
                 " t a s k " :   { 
                     " t i t l e " :   " j o b   4 " , 
                     " k e y " :   " V 7 8 9 " , 
                     " c u s t o m e r n u m b e r " :   " 8 9 0 " 
                 } , 
                 " e m p l o y e e " :   { 
                     " f i r s t n a m e " :   " J a k o b " , 
                     " s u r n a m e " :   " S t e g e l i n " , 
                     " p h o n e " :   " 0 1 2   3 4 5   6 7   8 9 " 
                 } , 
                 " i s s u e d a t e " :   " 1 3 . 5 . 2 0 0 6 " 
             } 
         ] 
     } 
 } **config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson -c wolframe.conf run

**file:wolframe.conf
LoadModules
{
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform
	module ../../src/modules/normalize/number/mod_normalize_number
	module ../../src/modules/normalize/string/mod_normalize_string
}
Processor
{
	program		normalize.wnmp
	program		form.sfrm
	cmdhandler
	{
		lua
		{
			program script.lua
		}
	}
}
**file:normalize.wnmp
int=trim,integer(10 );
uint=trim,unsigned(10);
float=trim, floatingpoint( 10,10);
currency=fixedpoint(13, 2);
percent_1=fixedpoint(5 ,1);
**file:form.sfrm
FORM Employee
{
	firstname string
	surname string
	phone string
}

FORM employee_assignment_print
{
	assignmentlist
	{
		assignment []
		{
			task []
			{
				title string
				key string
				customernumber int
			}
			employee Employee
			issuedate string
		}
	}
}
**file:script.lua
function run()
	type = input:doctype()
	output:as( provider.filter(), type)
	output:print( input:table())
end
**requires:DISABLED NETBSD

**output
{ 
 	 " d o c t y p e " : 	 " e m p l o y e e _ a s s i g n m e n t _ p r i n t " , 
 	 " a s s i g n m e n t l i s t " : 	 { 
 	 	 " a s s i g n m e n t " : 	 [ { 
 	 	 	 	 " i s s u e d a t e " : 	 " 1 3 . 5 . 2 0 0 6 " , 
 	 	 	 	 " e m p l o y e e " : 	 { 
 	 	 	 	 	 " f i r s t n a m e " : 	 " J u l i a " , 
 	 	 	 	 	 " p h o n e " : 	 " 0 9 8   7 6 5   4 3   2 1 " , 
 	 	 	 	 	 " s u r n a m e " : 	 " T e g e l - S a c h e r " 
 	 	 	 	 } , 
 	 	 	 	 " t a s k " : 	 [ { 
 	 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 3 2 4 " , 
 	 	 	 	 	 	 " k e y " : 	 " A 1 2 3 " , 
 	 	 	 	 	 	 " t i t l e " : 	 " j o b   1 " 
 	 	 	 	 	 } ,   { 
 	 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 5 6 7 " , 
 	 	 	 	 	 	 " k e y " : 	 " V 4 5 6 " , 
 	 	 	 	 	 	 " t i t l e " : 	 " j o b   2 " 
 	 	 	 	 	 } ] 
 	 	 	 } ,   { 
 	 	 	 	 " i s s u e d a t e " : 	 " 1 3 . 5 . 2 0 0 6 " , 
 	 	 	 	 " e m p l o y e e " : 	 { 
 	 	 	 	 	 " f i r s t n a m e " : 	 " J a k o b " , 
 	 	 	 	 	 " p h o n e " : 	 " 0 1 2   3 4 5   6 7   8 9 " , 
 	 	 	 	 	 " s u r n a m e " : 	 " S t e g e l i n " 
 	 	 	 	 } , 
 	 	 	 	 " t a s k " : 	 [ { 
 	 	 	 	 	 	 " c u s t o m e r n u m b e r " : 	 " 8 9 0 " , 
 	 	 	 	 	 	 " k e y " : 	 " V 7 8 9 " , 
 	 	 	 	 	 	 " t i t l e " : 	 " j o b   4 " 
 	 	 	 	 	 } ] 
 	 	 	 } ] 
 	 } 
 } 
 **end

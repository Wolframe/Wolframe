**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
{ 
     " d o c t y p e "   :   " t r e e s " , 
     " r o o t " :   { 
         " m t r e e " :   { 
             " - i d " :   " 1 " , 
             " n a m e " :   " e i n s " , 
             " n o d e " :   [ 
                 { 
                     " - i d " :   " 1 1 " , 
                     " n a m e " :   " E i n s   e i n s " 
                 } , 
                 { 
                     " - i d " :   " 1 2 " , 
                     " n a m e " :   " E i n s   z w e i " , 
                     " n o d e " :   { 
                         " - i d " :   " 1 2 1 " , 
                         " n a m e " :   " E i n s   z w e i   e i n s " 
                     } 
                 } , 
                 { 
                     " - i d " :   " 1 3 " , 
                     " n a m e " :   " E i n s   d r e i " 
                 } 
             ] 
         } , 
         " b t r e e " :   { 
             " - i d " :   " 1 " , 
             " n a m e " :   " e i n s " , 
             " l e f t " :   { 
                 " - i d " :   " 1 1 " , 
                 " n a m e " :   " E i n s   e i n s " 
             } , 
             " r i g h t " :   { 
                 " - i d " :   " 1 2 " , 
                 " n a m e " :   " E i n s   z w e i " , 
                 " l e f t " :   { 
                     " - i d " :   " 1 2 1 " , 
                     " n a m e " :   " E i n s   z w e i   e i n s " 
                 } 
             } 
         } 
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
int=integer;
uint=unsigned;
float=floatingpoint ;
currency= fixedpoint(13,2);
percent_1=fixedpoint( 5 ,1);
normname =convdia,ucname;
**file:form.sfrm
STRUCT MulTreeNode
{
	id		@int
	name		normname
	node		^MulTreeNode[]
}

STRUCT BinTreeNode
{
	id		@int
	name		normname
	left		^BinTreeNode
	right		^BinTreeNode
}

FORM trees
{
	root
	{
		mtree	MulTreeNode
		btree	BinTreeNode
	}
}
**file:script.lua
function run()
	type = input:doctype()
	output:as( provider.filter(), type)
	output:print( input:table())
end
**output
{ 
 	 " d o c t y p e " : 	 " t r e e s " , 
 	 " r o o t " : 	 { 
 	 	 " m t r e e " : 	 { 
 	 	 	 " i d " : 	 " 1 " , 
 	 	 	 " n a m e " : 	 " E I N S " , 
 	 	 	 " n o d e " : 	 [ { 
 	 	 	 	 	 " i d " : 	 " 1 1 " , 
 	 	 	 	 	 " n a m e " : 	 " E I N S   E I N S " 
 	 	 	 	 } ,   { 
 	 	 	 	 	 " i d " : 	 " 1 2 " , 
 	 	 	 	 	 " n a m e " : 	 " E I N S   Z W E I " , 
 	 	 	 	 	 " n o d e " : 	 [ { 
 	 	 	 	 	 	 	 " i d " : 	 " 1 2 1 " , 
 	 	 	 	 	 	 	 " n a m e " : 	 " E I N S   Z W E I   E I N S " 
 	 	 	 	 	 	 } ] 
 	 	 	 	 } ,   { 
 	 	 	 	 	 " i d " : 	 " 1 3 " , 
 	 	 	 	 	 " n a m e " : 	 " E I N S   D R E I " 
 	 	 	 	 } ] 
 	 	 } , 
 	 	 " b t r e e " : 	 { 
 	 	 	 " i d " : 	 " 1 " , 
 	 	 	 " r i g h t " : 	 { 
 	 	 	 	 " i d " : 	 " 1 2 " , 
 	 	 	 	 " n a m e " : 	 " E I N S   Z W E I " , 
 	 	 	 	 " l e f t " : 	 { 
 	 	 	 	 	 " i d " : 	 " 1 2 1 " , 
 	 	 	 	 	 " n a m e " : 	 " E I N S   Z W E I   E I N S " 
 	 	 	 	 } 
 	 	 	 } , 
 	 	 	 " n a m e " : 	 " E I N S " , 
 	 	 	 " l e f t " : 	 { 
 	 	 	 	 " i d " : 	 " 1 1 " , 
 	 	 	 	 " n a m e " : 	 " E I N S   E I N S " 
 	 	 	 } 
 	 	 } 
 	 } 
 } 
 **end

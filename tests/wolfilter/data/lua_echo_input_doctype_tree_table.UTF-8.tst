**
**requires:LUA
**input
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE root SYSTEM "http://www.wolframe.org/example-doc/trees.dtd">
<root>
<mtree id='1'>
<name>eins</name>
<node id='11'>
<name>Eins eins</name>
</node>
<node id='12'>
<name>Eins zwei</name>
<node id='121'>
<name>Eins zwei eins</name>
</node>
</node>
<node id='13'>
<name>Eins drei</name>
</node>
</mtree>
<btree id='1'>
<name>eins</name>
<left id='11'>
<name>Eins eins</name>
</left>
<right id='12'>
<name>Eins zwei</name>
<left id='121'>
<name>Eins zwei eins</name>
</left>
</right>
</btree>
</root>
**config
--input-filter textwolf --output-filter textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf -c wolframe.conf run
**requires:TEXTWOLF
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
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE root SYSTEM "http://www.wolframe.org/example-doc/trees.dtd"><root><mtree><id>1</id><name>EINS</name><node><id>11</id><name>EINS EINS</name></node><node><id>12</id><name>EINS ZWEI</name><node><id>121</id><name>EINS ZWEI EINS</name></node></node><node><id>13</id><name>EINS DREI</name></node></mtree><btree><id>1</id><right><id>12</id><name>EINS ZWEI</name><left><id>121</id><name>EINS ZWEI EINS</name></left></right><name>EINS</name><left><id>11</id><name>EINS EINS</name></left></btree></root>
**end

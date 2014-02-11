**
**requires:LUA
**requires:LIBXML2
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
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2  --module ../../src/modules/cmdbind/lua/mod_command_lua --module ../../src/modules/ddlcompiler//simpleform/mod_ddlcompiler_simpleform --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/normalize//string/mod_normalize_string --program simpleform_string.wnmp --program trees.sfrm --cmdprogram echo_input_doctype_table.lua run

**file:simpleform_string.wnmp
int=integer;
uint=unsigned;
float=float ;
currency= fixedpoint(13,2);
percent_1=fixedpoint( 5 ,1);
normname =convdia,ucname;
**file: echo_input_doctype_table.lua
function run()
	type = input:doctype()
	output:as( filter(), type)
	output:print( input:table())
end
**file: trees.sfrm
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


**output
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE root SYSTEM "http://www.wolframe.org/example-doc/trees.dtd"><root><mtree><id>1</id><name>EINS</name><node><id>11</id><name>EINS EINS</name></node><node><id>12</id><name>EINS ZWEI</name><node><id>121</id><name>EINS ZWEI EINS</name></node></node><node><id>13</id><name>EINS DREI</name></node></mtree><btree><id>1</id><right><id>12</id><name>EINS ZWEI</name><left><id>121</id><name>EINS ZWEI EINS</name></left></right><name>EINS</name><left><id>11</id><name>EINS EINS</name></left></btree></root>
**end

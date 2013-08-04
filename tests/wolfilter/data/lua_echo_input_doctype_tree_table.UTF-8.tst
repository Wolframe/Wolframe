**
**requires:LUA
**input
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE root SYSTEM "http://www.wolframe.org/example-doc/trees.dtd">
<root>
<mtree id='1'>
<name>eins</name>
<node id='1.1'>
<name>Eins eins</name>
</node>
<node id='1.2'>
<name>Eins zwei</name>
<node id='1.2.1'>
<name>Eins zwei eins</name>
</node>
</node>
<node id='1.3'>
<name>Eins drei</name>
</node>
</mtree>
<btree id='1'>
<name>eins</name>
<left id='1.1'>
<name>Eins eins</name>
</left>
<right id='1.2'>
<name>Eins zwei</name>
<left id='1.2.1'>
<name>Eins zwei eins</name>
</left>
</right>
</btree>
</root>
**config
--input-filter xml:textwolf --output-filter xml:textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf  --module ../../src/modules/cmdbind/lua/mod_command_lua --module ../../src/modules/ddlcompiler//simpleform/mod_ddlcompiler_simpleform --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/normalize//string/mod_normalize_string --program simpleform_string.normalize --program trees.simpleform --cmdprogram echo_input_doctype_table.lua run

**file:simpleform_string.normalize
int=number:integer;
uint=number:unsigned;
float=number:float;
normname=string:convdia,ucname;
**file: echo_input_doctype_table.lua
function run()
	type = input:doctype()
	output:as( filter(), type)
	output:print( input:table())
end
**file: trees.simpleform
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
<!DOCTYPE root SYSTEM "http://www.wolframe.org/example-doc/trees.dtd"><root><mtree><id>1</id><name>EINS</name><node><id>1.1</id><name>EINS EINS</name></node><node><id>1.2</id><name>EINS ZWEI</name><node><id>1.2.1</id><name>EINS ZWEI EINS</name></node></node><node><id>1.3</id><name>EINS DREI</name></node></mtree><btree><id>1</id><right><id>1.2</id><name>EINS ZWEI</name><left><id>1.2.1</id><name>EINS ZWEI EINS</name></left></right><name>EINS</name><left><id>1.1</id><name>EINS EINS</name></left></btree></root>
**end

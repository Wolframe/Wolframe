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
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2 -c wolframe.conf run

**file:wolframe.conf
LoadModules
{
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform
	module ../../src/modules/normalize/number/mod_normalize_number
	module ../../src/modules/normalize/string/mod_normalize_string
	module ../../src/modules/datatype/bcdnumber/mod_datatype_bcdnumber
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
currency= bigfxp(   2  );
percent_1=bigfxp(  2  );
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
	:root root
{
	mtree	MulTreeNode
	btree	BinTreeNode
}
**file:script.lua
function printTable( tab)
	-- deterministic print of a table (since lua 5.2.1 table keys order is non deterministic)

	keys = {}
	for key,val in pairs( tab) do
		table.insert( keys, key)
	end
	table.sort( keys)

	for i,t in ipairs( keys) do
		local v = tab[ t]

		if type(v) == "table" then
			if v[ #v] then
				-- print array (keys are indices)
				for eidx,elem in ipairs( v) do
					output:opentag( t)
					if type(elem) == "table" then
						printTable( elem)
					else
						output:print( elem)
					end
					output:closetag()
				end
			else
				-- print table (keys are values)
				output:opentag( t)
				printTable( v)
				output:closetag()
			end
		else
			output:opentag( t)
			output:print( v)
			output:closetag()
		end
	end
end

function run()
	local doctype = input:doctype()
	output:as( provider.filter(), doctype)
	printTable( input:table())
end
**output
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE root SYSTEM "http://www.wolframe.org/example-doc/trees.dtd"><root><btree><id>1</id><left><id>11</id><name>EINS EINS</name></left><name>EINS</name><right><id>12</id><left><id>121</id><name>EINS ZWEI EINS</name></left><name>EINS ZWEI</name></right></btree><mtree><id>1</id><name>EINS</name><node><id>11</id><name>EINS EINS</name></node><node><id>12</id><name>EINS ZWEI</name><node><id>121</id><name>EINS ZWEI EINS</name></node></node><node><id>13</id><name>EINS DREI</name></node></mtree></root>
**end

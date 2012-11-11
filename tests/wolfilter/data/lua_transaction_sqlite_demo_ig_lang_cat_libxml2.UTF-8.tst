**
**requires:LUA
**requires:LIBXML2
**requires:SQLITE3
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<class name="indogermanic">
	<class name="celtic" status="living">
		<class name="gaulisch" status="dead"/>
		<class name="goidelic" status="dead">
			<class name="old irish" status="dead"/>
			<class name="middle irish" status="dead"/>
			<class name="manx" status="dead"/>
			<class name="irish" status="living"/>
			<class name="scotts gaelic" status="living"/>
		</class>
		<class name="brythonic" status="dead">
			<class name="comish" status="dead"/>
			<class name="welsh" status="living"/>
			<class name="breton" status="living"/>
		</class>
	</class>
	<class name="germanic" status="living">
		<class name="west germanic" status="living">
			<class name="anglo-frisian" status="living">
				<class name="old english" status="dead">
					<class name="middle english" status="dead">
						<class name="english" status="living"/>
					</class>
				</class>
				<class name="old frisian" status="dead">
					<class name="frisian" status="living"/>
				</class>
			</class>
			<class name="german" status="living">
				<class name="low german" status="living">
					<class name="old saxon" status="dead">
						<class name="plattdeutsch" status="dead"/>
					</class>
					<class name="old low franconian" status="dead">
						<class name="dutch" status="living"/>
						<class name="flemish" status="living"/>
						<class name="afrikaans" status="living"/>
						<class name="south african dutch" status="dead"/>
					</class>
				</class>
				<class name="high german" status="living">
					<class name="alemannic" status="living"/>
					<class name="alsatian" status="living"/>
					<class name="bavarian" status="living"/>
					<class name="franconian" status="living"/>
					<class name="german" status="living"/>
					<class name="pensilvania german" status="living"/>
					<class name="swiss" status="living"/>
					<class name="yiddish" status="living"/>
				</class>
			</class>
		</class>
		<class name="east germanic" status="living"/>
		<class name="north germanic" status="living">
			<class name="old west norse" status="living">
				<class name="islandic" status="living"/>
				<class name="faroese" status="living"/>
			</class>
			<class name="old east norse" status="living">
				<class name="norwegian" status="living"/>
				<class name="danish" status="living"/>
				<class name="swedish" status="living"/>
			</class>
		</class>
	</class>
	<class name="italic" status="living">
		<class name="oscan" status="dead"/>
		<class name="umbrian" status="dead"/>
		<class name="old latin" status="living">
			<class name="catalan" status="living"/>
			<class name="french" status="living"/>
			<class name="galician" status="living"/>
			<class name="portuguese" status="living"/>
			<class name="italian" status="living"/>
			<class name="provencal" status="living"/>
			<class name="romansch" status="living"/>
			<class name="romanian" status="living"/>
			<class name="spanish" status="living"/>
		</class>
	</class>
	<class name="slavonic" status="living">
		<class name="west slavic" status="living">
			<class name="chech" status="living"/>
			<class name="polish" status="living"/>
			<class name="slovak" status="living"/>
			<class name="sorbian" status="living"/>
		</class>
		<class name="east slavic" status="living">
			<class name="belarussian" status="living"/>
			<class name="russian" status="living"/>
			<class name="ukrainian" status="living"/>
		</class>
		<class name="south slavic" status="living">
			<class name="bosnian" status="living"/>
			<class name="bulgarian" status="living"/>
			<class name="macedonian" status="living"/>
			<class name="serbo-croatian" status="living"/>
			<class name="slovene" status="living"/>
		</class>
	</class>
	<class name="albanian" status="living"/>
	<class name="armenian" status="living"/>
	<class name="hellenic" status="living">
		<class name="greek" status="living"/>
	</class>
	<class name="baltic" status="living">
		<class name="lettish" status="living"/>
		<class name="latvian" status="living"/>
		<class name="lithuanian" status="living"/>
	</class>
	<class name="hittie" status="dead">
	</class>
	<class name="indo iranian" status="living">
		<class name="iranian" status="living">
			<class name="avestan" status="dead">
				<class name="pashto" status="living"/>
			</class>
			<class name="old persian" status="dead">
				<class name="balushti" status="living"/>
				<class name="kurdish" status="living"/>
				<class name="ossetic" status="living"/>
				<class name="pashto" status="living"/>
				<class name="persian" status="living"/>
			</class>
			<class name="scythian" status="dead"/>
		</class>
		<class name="indic" status="living">
			<class name="sanskrit" status="dead"/>
			<class name="prakrit" status="dead"/>
			<class name="pali" status="dead"/>
			<class name="bengali" status="living"/>
			<class name="bihari" status="living"/>
			<class name="bhili" status="living"/>
			<class name="gujarati" status="living"/>
			<class name="hindi" status="living"/>
			<class name="hindustani" status="living"/>
			<class name="marati" status="living"/>
			<class name="nepali" status="living"/>
			<class name="bahari" status="living"/>
			<class name="punjabi" status="living"/>
			<class name="rajasthani" status="living"/>
			<class name="sindhi" status="living"/>
			<class name="singhalese" status="living"/>
			<class name="urdu" status="living"/>
		</class>
	</class>
	<class name="tocharian" status="dead">
	</class>
</class>**config
--input-filter xml:libxml2 --output-filter xml:libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2  --module ../../src/modules/cmdbind/lua/mod_command_lua --program=transaction_sqlite_demo_ig_lang_cat.lua --program simpleform.normalize --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../wolfilter/modules/database/sqlite3/mod_db_sqlite3test --database 'identifier=testdb,file=test.db,dumpfile=DBDUMP,inputfile=DBDATA' --program=DBPRG.tdl run

**file:simpleform.normalize
int=number:integer;
uint=number:unsigned;
float=number:float;
**file: DBDATA
--
-- Generic tree implementation for SQL databases
-- (Modified Preorder Tree Traversal)
--
-- Note: the parent restriction assumes that references accept NULL
-- Note: if DEFERRED does not work on UNIQUE constraints then the
--       UNIQUE constraint must be dropped
-- Joe Celko example from http://www.ibase.ru/devinfo/DBMSTrees/sqltrees.html

CREATE TABLE tree (
 ID INTEGER PRIMARY KEY AUTOINCREMENT,
 parent INT REFERENCES tree( ID ),
 name TEXT,
 lft INT NOT NULL DEFERRABLE CHECK ( lft > 0 ),
 rgt INT NOT NULL DEFERRABLE CHECK ( rgt > 1 ),
 CONSTRAINT order_check CHECK ( rgt > lft )
);
**file:DBPRG.tdl
--
-- treeAddRoot
--
TRANSACTION treeAddRoot -- (name)
BEGIN
	DO INSERT INTO tree (ID, parent, name, lft, rgt) VALUES (1, 0, $(name), 1, 2);
END

--
-- treeAddNode
--
TRANSACTION treeAddNode -- (parentid, name)
BEGIN
	DO NONEMPTY UNIQUE SELECT rgt FROM tree WHERE ID = $(parentid);
	DO UPDATE tree SET rgt = rgt + 2 WHERE rgt >= $1;
	DO UPDATE tree SET lft = lft + 2 WHERE lft > $1;
	DO INSERT INTO tree (parent, name, lft, rgt) VALUES ($(parentid), $(name), $1, $1+1);
	INTO . DO NONEMPTY UNIQUE SELECT ID from tree WHERE lft = $1;
END

--
-- treeDeleteSubtree
--
TRANSACTION treeDeleteSubtree -- (id)
BEGIN
	DO NONEMPTY SELECT lft,rgt,rgt-lft AS width FROM tree WHERE ID = $(id);
	DO DELETE FROM tree WHERE lft >= $1 AND lft <= $2;
	DO UPDATE tree SET lft = lft-$3 WHERE lft>$2;
	DO UPDATE tree SET rgt = rgt-$3 WHERE rgt>$2;
END

--
-- treeSelectNode       :Get the node
-- treeSelectNodeByName :Get the node by name
--
TRANSACTION treeSelectNode -- (/node/id)
BEGIN
	FOREACH /node INTO . DO NONEMPTY UNIQUE SELECT ID,parent,name FROM tree WHERE ID = $(id);
END
TRANSACTION treeSelectNodeByName -- (/node/name)
BEGIN
	FOREACH /node INTO . DO NONEMPTY UNIQUE SELECT ID,parent,name FROM tree WHERE name = $(name);
END

--
-- treeSelectNodeAndParents       :Get the node and its parents
-- treeSelectNodeAndParentsByName :Get the node and its parents by name
--
TRANSACTION treeSelectNodeAndParents -- (/node/id)
BEGIN
	FOREACH /node INTO /node DO SELECT P2.ID,P2.name FROM tree AS P1, tree AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P1.ID = $(id);
END
TRANSACTION treeSelectNodeAndParentsByName -- (/node/name)
BEGIN
	FOREACH /node INTO /node DO SELECT P2.ID,P2.name FROM tree AS P1, tree AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P1.name = $(name);
END

--
-- treeSelectParents       :Get the parents of a node
-- treeSelectParentsByName :Get the parents of a node by name
--
TRANSACTION treeSelectParents -- (/node/id)
BEGIN
	FOREACH /node INTO /node DO SELECT P2.ID,P2.name FROM tree AS P1, tree AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P1.ID = $(id);
END
TRANSACTION treeSelectParentsByName -- (/node/name)
BEGIN
	FOREACH /node INTO /node DO SELECT P2.ID,P2.name FROM tree AS P1, tree AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P1.name = $(name);
END

--
-- treeSelectNodeAndChildren       :Get the node and its children
-- treeSelectNodeAndChildrenByName :Get the node and its children by name
--
TRANSACTION treeSelectNodeAndChildren -- (/node/id)
BEGIN
	FOREACH /node INTO /node DO SELECT P1.ID,P1.parent,P1.name FROM tree AS P1, tree AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P2.ID = $(id);
END
TRANSACTION treeSelectNodeAndChildrenByName -- (/node/name)
BEGIN
	FOREACH /node INTO /node DO SELECT P1.ID,P1.parent,P1.name FROM tree AS P1, tree AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P2.name = $(name);
END

--
-- treeSelectChildren       :Get the children of a node
-- treeSelectChildrenByName :Get the children of a node by name
--
TRANSACTION treeSelectChildren -- (/node/id)
BEGIN
	FOREACH /node INTO /node DO SELECT P1.ID,P1.name FROM tree AS P1, tree AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P2.ID = $(id);
END
TRANSACTION treeSelectChildrenByName -- (/node/name)
BEGIN
	FOREACH /node INTO /node DO SELECT P1.ID,P1.name FROM tree AS P1, tree AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P2.name = $(name);
END
**outputfile:DBDUMP
**file: transaction_sqlite_demo_ig_lang_cat.lua
idcnt = 0

function insert_tree( parentid, itr)
	local id = idcnt + 1
	for v,t in itr do
		if (t == "name") then
			local name = v
			if idcnt == 0 then
				formfunction( "treeAddRoot")( {name=name} )
			else
				formfunction( "treeAddNode")( {name=name, parentid=parentid} )
			end
			idcnt = idcnt + 1
		end
		if (t == "class") then
			insert_tree( id, scope( itr))
		end
	end
end

function insert_node( parentname, name)
	local parentid = formfunction( "treeSelectNodeByName")( { node={ name=parentname } } ):table().ID
	formfunction( "treeAddNode")( {name=name, parentid=parentid} )
end


function delete_subtree( name)
	local id = formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table().ID
	formfunction( "treeDeleteSubtree")( {id=id} )
end

function select_subtree( name)
	local id = formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table().ID
	local nodear = formfunction( "treeSelectNodeAndChildren")( { node={ id=id } } ):table()["node"] or {}
	output:opentag( "subtree")
	output:print( name, "name")
	output:print( "\n")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
		output:print( "\n")
	end
	output:closetag()
	output:print( "\n")
end

function select_subtree2( name)
	local nodear = formfunction( "treeSelectNodeAndChildrenByName")( { node={ name=name } } ):table()["node"] or {}
	output:opentag( "subtree")
	output:print( name, "name")
	output:print( "\n")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
		output:print( "\n")
	end
	output:closetag()
	output:print( "\n")
end

function select_children( name)
	local id = formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table().ID
	local nodear = formfunction( "treeSelectChildren")( { node={ id=id } } ):table()["node"] or {}
	output:opentag( "children")
	output:print( name, "name")
	output:print( "\n")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
		output:print( "\n")
	end
	output:closetag()
	output:print( "\n")
end

function select_children2( name)
	local nodear = formfunction( "treeSelectChildrenByName")( { node={ name=name } } ):table()["node"] or {}
	output:opentag( "children")
	output:print( name, "name")
	output:print( "\n")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
		output:print( "\n")
	end
	output:closetag()
	output:print( "\n")
end

function select_cover( name)
	local id = formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table().ID
	local nodear = formfunction( "treeSelectNodeAndParents")( { node={ id=id } } ):table()["node"] or {}
	output:opentag( "cover")
	output:print( name, "name")
	output:print( "\n")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
		output:print( "\n")
	end
	output:closetag()
	output:print( "\n")
end

function select_cover2( name)
	local nodear = formfunction( "treeSelectNodeAndParentsByName")( { node={ name=name } } ):table()["node"] or {}
	output:opentag( "cover")
	output:print( name, "name")
	output:print( "\n")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
		output:print( "\n")
	end
	output:closetag()
	output:print( "\n")
end

function select_parents( name)
	local id = formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table().ID
	local nodear = formfunction( "treeSelectParents")( { node={ id=id } } ):table()["node"] or {}
	output:opentag( "parents")
	output:print( name, "name")
	output:print( "\n")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
		output:print( "\n")
	end
	output:closetag()
	output:print( "\n")
end

function select_parents2( name)
	local nodear = formfunction( "treeSelectParentsByName")( { node={ name=name } } ):table()["node"] or {}
	output:opentag( "parents")
	output:print( name, "name")
	output:print( "\n")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
		output:print( "\n")
	end
	output:closetag()
	output:print( "\n")
end

function get_tree( parentid)
	local t = formfunction( "treeSelectNodeAndChildren")( { node = { id=parentid } } ):table()["node"] or {}
	local a = {}
	for i,v in pairs( t) do
		table.insert( a, tonumber( v.ID), { name=v.name, parent=tonumber(v.parent), children = {} } )
	end
	for i,v in pairs( a) do
		if v.parent ~= 0 then
			table.insert( a[ v.parent ].children, i )
		end
	end
	return a
end

function print_tree( tree, nodeid, indent)
	if (indent ~= "") then
		output:print( "\n" .. indent)
	end
	output:opentag( "class")
	output:print( tree[ nodeid].name, "name")
	local n = 0
	for i,v in pairs( tree[ nodeid].children) do
		print_tree( tree, v, indent .. "\t")
		n = n + 1
	end
	if n > 0 then
		output:print( "\n" .. indent)
	end
	output:closetag()
end

function run()
	filter().empty = false
	output:opentag( "result")
	transaction:begin( "insert")
	local itr = input:get()
	for v,t in itr do
		if t == "class" then
			insert_tree( idcnt, scope( itr))
		end
	end
	transaction:commit()
	print_tree( get_tree( 1), 1, "")
	select_subtree( "italic")
	select_subtree( "brythonic")
	select_subtree2( "germanic")
	select_subtree2( "anglo-frisian")
	select_children( "gaulisch")
	select_children( "slavonic")
	select_children2( "east germanic")
	select_children2( "indic")
	select_cover( "italic")
	select_cover( "brythonic")
	select_cover2( "germanic")
	select_cover2( "anglo-frisian")
	select_parents( "gaulisch")
	select_parents( "slavonic")
	select_parents2( "east germanic")
	select_parents2( "indic")
	delete_subtree( "hellenic")
	delete_subtree( "hittie")
	insert_node( "swiss", "bern german")
	insert_node( "swiss", "eastern swiss german")
	insert_node( "swiss", "grison german")
	insert_node( "indogermanic", "hittie")
	delete_subtree( "celtic")
	delete_subtree( "indo iranian")
	output:opentag( "sparsetree")
	print_tree( get_tree( 1), 1, "")
	output:closetag()
	output:closetag()
end

**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<result><class name="indogermanic">
	<class name="celtic">
		<class name="gaulisch"/>
		<class name="goidelic">
			<class name="old irish"/>
			<class name="middle irish"/>
			<class name="manx"/>
			<class name="irish"/>
			<class name="scotts gaelic"/>
		</class>
		<class name="brythonic">
			<class name="comish"/>
			<class name="welsh"/>
			<class name="breton"/>
		</class>
	</class>
	<class name="germanic">
		<class name="west germanic">
			<class name="anglo-frisian">
				<class name="old english">
					<class name="middle english">
						<class name="english"/>
					</class>
				</class>
				<class name="old frisian">
					<class name="frisian"/>
				</class>
			</class>
			<class name="german">
				<class name="low german">
					<class name="old saxon">
						<class name="plattdeutsch"/>
					</class>
					<class name="old low franconian">
						<class name="dutch"/>
						<class name="flemish"/>
						<class name="afrikaans"/>
						<class name="south african dutch"/>
					</class>
				</class>
				<class name="high german">
					<class name="alemannic"/>
					<class name="alsatian"/>
					<class name="bavarian"/>
					<class name="franconian"/>
					<class name="german"/>
					<class name="pensilvania german"/>
					<class name="swiss"/>
					<class name="yiddish"/>
				</class>
			</class>
		</class>
		<class name="east germanic"/>
		<class name="north germanic">
			<class name="old west norse">
				<class name="islandic"/>
				<class name="faroese"/>
			</class>
			<class name="old east norse">
				<class name="norwegian"/>
				<class name="danish"/>
				<class name="swedish"/>
			</class>
		</class>
	</class>
	<class name="italic">
		<class name="oscan"/>
		<class name="umbrian"/>
		<class name="old latin">
			<class name="catalan"/>
			<class name="french"/>
			<class name="galician"/>
			<class name="portuguese"/>
			<class name="italian"/>
			<class name="provencal"/>
			<class name="romansch"/>
			<class name="romanian"/>
			<class name="spanish"/>
		</class>
	</class>
	<class name="slavonic">
		<class name="west slavic">
			<class name="chech"/>
			<class name="polish"/>
			<class name="slovak"/>
			<class name="sorbian"/>
		</class>
		<class name="east slavic">
			<class name="belarussian"/>
			<class name="russian"/>
			<class name="ukrainian"/>
		</class>
		<class name="south slavic">
			<class name="bosnian"/>
			<class name="bulgarian"/>
			<class name="macedonian"/>
			<class name="serbo-croatian"/>
			<class name="slovene"/>
		</class>
	</class>
	<class name="albanian"/>
	<class name="armenian"/>
	<class name="hellenic">
		<class name="greek"/>
	</class>
	<class name="baltic">
		<class name="lettish"/>
		<class name="latvian"/>
		<class name="lithuanian"/>
	</class>
	<class name="hittie"/>
	<class name="indo iranian">
		<class name="iranian">
			<class name="avestan">
				<class name="pashto"/>
			</class>
			<class name="old persian">
				<class name="balushti"/>
				<class name="kurdish"/>
				<class name="ossetic"/>
				<class name="pashto"/>
				<class name="persian"/>
			</class>
			<class name="scythian"/>
		</class>
		<class name="indic">
			<class name="sanskrit"/>
			<class name="prakrit"/>
			<class name="pali"/>
			<class name="bengali"/>
			<class name="bihari"/>
			<class name="bhili"/>
			<class name="gujarati"/>
			<class name="hindi"/>
			<class name="hindustani"/>
			<class name="marati"/>
			<class name="nepali"/>
			<class name="bahari"/>
			<class name="punjabi"/>
			<class name="rajasthani"/>
			<class name="sindhi"/>
			<class name="singhalese"/>
			<class name="urdu"/>
		</class>
	</class>
	<class name="tocharian"/>
</class><subtree name="italic">
<node id="49" name="italic"/>
<node id="50" name="oscan"/>
<node id="51" name="umbrian"/>
<node id="52" name="old latin"/>
<node id="53" name="catalan"/>
<node id="54" name="french"/>
<node id="55" name="galician"/>
<node id="56" name="portuguese"/>
<node id="57" name="italian"/>
<node id="58" name="provencal"/>
<node id="59" name="romansch"/>
<node id="60" name="romanian"/>
<node id="61" name="spanish"/>
</subtree>
<subtree name="brythonic">
<node id="10" name="brythonic"/>
<node id="11" name="comish"/>
<node id="12" name="welsh"/>
<node id="13" name="breton"/>
</subtree>
<subtree name="germanic">
<node id="14" name="germanic"/>
<node id="15" name="west germanic"/>
<node id="16" name="anglo-frisian"/>
<node id="17" name="old english"/>
<node id="18" name="middle english"/>
<node id="19" name="english"/>
<node id="20" name="old frisian"/>
<node id="21" name="frisian"/>
<node id="22" name="german"/>
<node id="23" name="low german"/>
<node id="24" name="old saxon"/>
<node id="25" name="plattdeutsch"/>
<node id="26" name="old low franconian"/>
<node id="27" name="dutch"/>
<node id="28" name="flemish"/>
<node id="29" name="afrikaans"/>
<node id="30" name="south african dutch"/>
<node id="31" name="high german"/>
<node id="32" name="alemannic"/>
<node id="33" name="alsatian"/>
<node id="34" name="bavarian"/>
<node id="35" name="franconian"/>
<node id="36" name="german"/>
<node id="37" name="pensilvania german"/>
<node id="38" name="swiss"/>
<node id="39" name="yiddish"/>
<node id="40" name="east germanic"/>
<node id="41" name="north germanic"/>
<node id="42" name="old west norse"/>
<node id="43" name="islandic"/>
<node id="44" name="faroese"/>
<node id="45" name="old east norse"/>
<node id="46" name="norwegian"/>
<node id="47" name="danish"/>
<node id="48" name="swedish"/>
</subtree>
<subtree name="anglo-frisian">
<node id="16" name="anglo-frisian"/>
<node id="17" name="old english"/>
<node id="18" name="middle english"/>
<node id="19" name="english"/>
<node id="20" name="old frisian"/>
<node id="21" name="frisian"/>
</subtree>
<children name="gaulisch">
</children>
<children name="slavonic">
<node id="63" name="west slavic"/>
<node id="64" name="chech"/>
<node id="65" name="polish"/>
<node id="66" name="slovak"/>
<node id="67" name="sorbian"/>
<node id="68" name="east slavic"/>
<node id="69" name="belarussian"/>
<node id="70" name="russian"/>
<node id="71" name="ukrainian"/>
<node id="72" name="south slavic"/>
<node id="73" name="bosnian"/>
<node id="74" name="bulgarian"/>
<node id="75" name="macedonian"/>
<node id="76" name="serbo-croatian"/>
<node id="77" name="slovene"/>
</children>
<children name="east germanic">
</children>
<children name="indic">
<node id="99" name="sanskrit"/>
<node id="100" name="prakrit"/>
<node id="101" name="pali"/>
<node id="102" name="bengali"/>
<node id="103" name="bihari"/>
<node id="104" name="bhili"/>
<node id="105" name="gujarati"/>
<node id="106" name="hindi"/>
<node id="107" name="hindustani"/>
<node id="108" name="marati"/>
<node id="109" name="nepali"/>
<node id="110" name="bahari"/>
<node id="111" name="punjabi"/>
<node id="112" name="rajasthani"/>
<node id="113" name="sindhi"/>
<node id="114" name="singhalese"/>
<node id="115" name="urdu"/>
</children>
<cover name="italic">
<node id="1" name="indogermanic"/>
<node id="49" name="italic"/>
</cover>
<cover name="brythonic">
<node id="1" name="indogermanic"/>
<node id="2" name="celtic"/>
<node id="10" name="brythonic"/>
</cover>
<cover name="germanic">
<node id="1" name="indogermanic"/>
<node id="14" name="germanic"/>
</cover>
<cover name="anglo-frisian">
<node id="1" name="indogermanic"/>
<node id="14" name="germanic"/>
<node id="15" name="west germanic"/>
<node id="16" name="anglo-frisian"/>
</cover>
<parents name="gaulisch">
<node id="1" name="indogermanic"/>
<node id="2" name="celtic"/>
</parents>
<parents name="slavonic">
<node id="1" name="indogermanic"/>
</parents>
<parents name="east germanic">
<node id="1" name="indogermanic"/>
<node id="14" name="germanic"/>
</parents>
<parents name="indic">
<node id="1" name="indogermanic"/>
<node id="87" name="indo iranian"/>
</parents>
<sparsetree><class name="indogermanic">
	<class name="germanic">
		<class name="west germanic">
			<class name="anglo-frisian">
				<class name="old english">
					<class name="middle english">
						<class name="english"/>
					</class>
				</class>
				<class name="old frisian">
					<class name="frisian"/>
				</class>
			</class>
			<class name="german">
				<class name="low german">
					<class name="old saxon">
						<class name="plattdeutsch"/>
					</class>
					<class name="old low franconian">
						<class name="dutch"/>
						<class name="flemish"/>
						<class name="afrikaans"/>
						<class name="south african dutch"/>
					</class>
				</class>
				<class name="high german">
					<class name="alemannic"/>
					<class name="alsatian"/>
					<class name="bavarian"/>
					<class name="franconian"/>
					<class name="german"/>
					<class name="pensilvania german"/>
					<class name="swiss">
						<class name="bern german"/>
						<class name="eastern swiss german"/>
						<class name="grison german"/>
					</class>
					<class name="yiddish"/>
				</class>
			</class>
		</class>
		<class name="east germanic"/>
		<class name="north germanic">
			<class name="old west norse">
				<class name="islandic"/>
				<class name="faroese"/>
			</class>
			<class name="old east norse">
				<class name="norwegian"/>
				<class name="danish"/>
				<class name="swedish"/>
			</class>
		</class>
	</class>
	<class name="italic">
		<class name="oscan"/>
		<class name="umbrian"/>
		<class name="old latin">
			<class name="catalan"/>
			<class name="french"/>
			<class name="galician"/>
			<class name="portuguese"/>
			<class name="italian"/>
			<class name="provencal"/>
			<class name="romansch"/>
			<class name="romanian"/>
			<class name="spanish"/>
		</class>
	</class>
	<class name="slavonic">
		<class name="west slavic">
			<class name="chech"/>
			<class name="polish"/>
			<class name="slovak"/>
			<class name="sorbian"/>
		</class>
		<class name="east slavic">
			<class name="belarussian"/>
			<class name="russian"/>
			<class name="ukrainian"/>
		</class>
		<class name="south slavic">
			<class name="bosnian"/>
			<class name="bulgarian"/>
			<class name="macedonian"/>
			<class name="serbo-croatian"/>
			<class name="slovene"/>
		</class>
	</class>
	<class name="albanian"/>
	<class name="armenian"/>
	<class name="baltic">
		<class name="lettish"/>
		<class name="latvian"/>
		<class name="lithuanian"/>
	</class>
	<class name="tocharian"/>
	<class name="hittie"/>
</class></sparsetree></result>
tree:
'1', '0', 'indogermanic', '1', '156'
'14', '1', 'germanic', '3', '78'
'15', '14', 'west germanic', '4', '59'
'16', '15', 'anglo-frisian', '5', '16'
'17', '16', 'old english', '6', '11'
'18', '17', 'middle english', '7', '10'
'19', '18', 'english', '8', '9'
'20', '16', 'old frisian', '12', '15'
'21', '20', 'frisian', '13', '14'
'22', '15', 'german', '17', '58'
'23', '22', 'low german', '18', '33'
'24', '23', 'old saxon', '19', '22'
'25', '24', 'plattdeutsch', '20', '21'
'26', '23', 'old low franconian', '23', '32'
'27', '26', 'dutch', '24', '25'
'28', '26', 'flemish', '26', '27'
'29', '26', 'afrikaans', '28', '29'
'30', '26', 'south african dutch', '30', '31'
'31', '22', 'high german', '34', '57'
'32', '31', 'alemannic', '35', '36'
'33', '31', 'alsatian', '37', '38'
'34', '31', 'bavarian', '39', '40'
'35', '31', 'franconian', '41', '42'
'36', '31', 'german', '43', '44'
'37', '31', 'pensilvania german', '45', '46'
'38', '31', 'swiss', '47', '54'
'39', '31', 'yiddish', '55', '56'
'40', '14', 'east germanic', '60', '61'
'41', '14', 'north germanic', '62', '77'
'42', '41', 'old west norse', '63', '68'
'43', '42', 'islandic', '64', '65'
'44', '42', 'faroese', '66', '67'
'45', '41', 'old east norse', '69', '76'
'46', '45', 'norwegian', '70', '71'
'47', '45', 'danish', '72', '73'
'48', '45', 'swedish', '74', '75'
'49', '1', 'italic', '79', '104'
'50', '49', 'oscan', '80', '81'
'51', '49', 'umbrian', '82', '83'
'52', '49', 'old latin', '84', '103'
'53', '52', 'catalan', '85', '86'
'54', '52', 'french', '87', '88'
'55', '52', 'galician', '89', '90'
'56', '52', 'portuguese', '91', '92'
'57', '52', 'italian', '93', '94'
'58', '52', 'provencal', '95', '96'
'59', '52', 'romansch', '97', '98'
'60', '52', 'romanian', '99', '100'
'61', '52', 'spanish', '101', '102'
'62', '1', 'slavonic', '105', '136'
'63', '62', 'west slavic', '106', '115'
'64', '63', 'chech', '107', '108'
'65', '63', 'polish', '109', '110'
'66', '63', 'slovak', '111', '112'
'67', '63', 'sorbian', '113', '114'
'68', '62', 'east slavic', '116', '123'
'69', '68', 'belarussian', '117', '118'
'70', '68', 'russian', '119', '120'
'71', '68', 'ukrainian', '121', '122'
'72', '62', 'south slavic', '124', '135'
'73', '72', 'bosnian', '125', '126'
'74', '72', 'bulgarian', '127', '128'
'75', '72', 'macedonian', '129', '130'
'76', '72', 'serbo-croatian', '131', '132'
'77', '72', 'slovene', '133', '134'
'78', '1', 'albanian', '137', '138'
'79', '1', 'armenian', '139', '140'
'82', '1', 'baltic', '142', '149'
'83', '82', 'lettish', '143', '144'
'84', '82', 'latvian', '145', '146'
'85', '82', 'lithuanian', '147', '148'
'116', '1', 'tocharian', '152', '153'
'117', '38', 'bern german', '48', '49'
'118', '38', 'eastern swiss german', '50', '51'
'119', '38', 'grison german', '52', '53'
'120', '1', 'hittie', '154', '155'

sqlite_sequence:
'tree', '120'

**end

**
**requires:LUA
**requires:PGSQL
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
--input-filter textwolf --output-filter textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf -c wolframe.conf run
**requires:TEXTWOLF
**file:wolframe.conf
LoadModules
{
	module ../wolfilter/modules/database/postgresql/mod_db_postgresqltest
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../../src/modules/normalize/number/mod_normalize_number
	module ../../src/modules/normalize/string/mod_normalize_string
	module ../../src/modules/datatype/bcdnumber/mod_datatype_bcdnumber
}
Database
{
	PostgreSQLTest
	{
		identifier testdb
		host localhost
		port 5432
		database wolframe
		user wolfusr
		password wolfpwd
		dumpfile DBDUMP
		inputfile DBDATA
	}
}
Processor
{
	database testdb
	program normalize.wnmp
	program DBPRG.tdl
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
float=floatingpoint;
currency=bigfxp(2);
percent_1=bigfxp(2);
**file:script.lua
idcnt = 0

function insert_tree( parentid, itr)
	local id = idcnt + 1
	for v,t in itr do
		if (t == "name") then
			local name = v
			if idcnt == 0 then
				provider.formfunction( "treeAddRoot")( {name=name} )
			else
				provider.formfunction( "treeAddNode")( {name=name, parentid=parentid} )
			end
			idcnt = idcnt + 1
		end
		if (t == "class") then
			insert_tree( id, scope( itr))
		end
	end
end

function insert_node( parentname, name)
	local parentid = provider.formfunction( "treeSelectNodeByName")( { node={ name=parentname } } ):table().ID
	provider.formfunction( "treeAddNode")( {name=name, parentid=parentid} )
end

function copy_node( name, parentname)
	local parentid = provider.formfunction( "treeSelectNodeByName")( { node={ name=parentname } } ):table().ID
	local nodeid = provider.formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table().ID
	provider.formfunction( "treeCopyNode")( {nodeid=nodeid, newparentid=parentid} )
end


function delete_subtree( name)
	local id = provider.formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table().ID
	provider.formfunction( "treeDeleteSubtree")( {id=id} )
end

function select_subtree( name)
	local id = provider.formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table().ID
	local nodear = provider.formfunction( "treeSelectNodeAndChildren")( { node={ id=id } } ):table()["node"] or {}
	output:opentag( "subtree")
	output:print( name, "name")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
	end
	output:closetag()
end

function select_subtree2( name)
	local nodear = provider.formfunction( "treeSelectNodeAndChildrenByName")( { node={ name=name } } ):table()["node"] or {}
	output:opentag( "subtree")
	output:print( name, "name")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
	end
	output:closetag()
end

function select_children( name)
	local id = provider.formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table().ID
	local nodear = provider.formfunction( "treeSelectChildren")( { node={ id=id } } ):table()["node"] or {}
	output:opentag( "children")
	output:print( name, "name")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
	end
	output:closetag()
end

function select_children2( name)
	local nodear = provider.formfunction( "treeSelectChildrenByName")( { node={ name=name } } ):table()["node"] or {}
	output:opentag( "children")
	output:print( name, "name")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
	end
	output:closetag()
end

function select_cover( name)
	local id = provider.formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table().ID
	local nodear = provider.formfunction( "treeSelectNodeAndParents")( { node={ id=id } } ):table()["node"] or {}
	output:opentag( "cover")
	output:print( name, "name")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
	end
	output:closetag()
end

function select_cover2( name)
	local nodear = provider.formfunction( "treeSelectNodeAndParentsByName")( { node={ name=name } } ):table()["node"] or {}
	output:opentag( "cover")
	output:print( name, "name")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
	end
	output:closetag()
end

function select_parents( name)
	local id = provider.formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table().ID
	local nodear = provider.formfunction( "treeSelectParents")( { node={ id=id } } ):table()["node"] or {}
	output:opentag( "parents")
	output:print( name, "name")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
	end
	output:closetag()
end

function select_parents2( name)
	local nodear = provider.formfunction( "treeSelectParentsByName")( { node={ name=name } } ):table()["node"] or {}
	output:opentag( "parents")
	output:print( name, "name")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
	end
	output:closetag()
end

function get_tree( parentid)
	local t = provider.formfunction( "treeSelectNodeAndChildren")( { node = { id=parentid } } ):table()["node"] or {}
	local a = {}
	for i,v in pairs( t) do
		table.insert( a, tonumber( v.ID), { name=v.name, parent=tonumber(v.parent), children = {} } )
	end
	for i,v in pairs( a) do
		if v.parent and v.parent ~= 0 then
			table.insert( a[ v.parent ].children, i )
		end
	end
	return a
end

function print_tree( tree, nodeid)
	output:opentag( "class")
	output:print( tree[ nodeid].name, "name")
	local n = 0
	for i,v in pairs( tree[ nodeid].children) do
		print_tree( tree, v)
		n = n + 1
	end
	output:closetag()
end

function run()
	provider.filter().empty = false
	local itr = input:get()
	for v,t in itr do
		if t == "class" then
			insert_tree( idcnt, scope( itr))
		end
	end
	output:opentag( "result")
	print_tree( get_tree( 1), 1)
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
	print_tree( get_tree( 1), 1)
	output:closetag()
	output:closetag()
end
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
 ID SERIAL NOT NULL PRIMARY KEY,
 parent INT REFERENCES tree( ID ),
 name TEXT,
 lft INT NOT NULL CHECK ( lft > 0 ),
 rgt INT NOT NULL CHECK ( rgt > 1 ),
 CONSTRAINT order_check CHECK ( rgt > lft )
);
ALTER SEQUENCE tree_ID_seq RESTART WITH 1;
**file:DBPRG.tdl
--
-- treeAddRoot
--
TRANSACTION treeAddRoot -- (name)
BEGIN
	DO INSERT INTO tree (parent, name, lft, rgt) VALUES (NULL, $(name), 1, 2);
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
	INTO . DO NONEMPTY UNIQUE SELECT ID AS "ID" from tree WHERE lft = $1;
END

--
-- treeDeleteSubtree
--
TRANSACTION treeDeleteSubtree -- (id)
BEGIN
	DO NONEMPTY SELECT lft,rgt,rgt-lft+1 AS width FROM tree WHERE ID = $(id);
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
	FOREACH /node INTO . DO NONEMPTY UNIQUE SELECT ID AS "ID",parent,name FROM tree WHERE ID = $(id);
END
TRANSACTION treeSelectNodeByName -- (/node/name)
BEGIN
	FOREACH /node INTO . DO NONEMPTY UNIQUE SELECT ID AS "ID",parent,name FROM tree WHERE name = $(name);
END

--
-- treeSelectNodeAndParents       :Get the node and its parents
-- treeSelectNodeAndParentsByName :Get the node and its parents by name
--
TRANSACTION treeSelectNodeAndParents -- (/node/id)
BEGIN
	FOREACH /node INTO node DO SELECT P2.ID AS "ID",P2.name FROM tree AS P1, tree AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P1.ID = $(id) ORDER BY P2.ID;
END
TRANSACTION treeSelectNodeAndParentsByName -- (/node/name)
BEGIN
	FOREACH /node INTO node DO SELECT P2.ID AS "ID",P2.name FROM tree AS P1, tree AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P1.name = $(name) ORDER BY P2.ID;
END

--
-- treeSelectParents       :Get the parents of a node
-- treeSelectParentsByName :Get the parents of a node by name
--
TRANSACTION treeSelectParents -- (/node/id)
BEGIN
	FOREACH /node INTO node DO SELECT P2.ID AS "ID",P2.name FROM tree AS P1, tree AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P1.ID = $(id) ORDER BY P2.ID;
END
TRANSACTION treeSelectParentsByName -- (/node/name)
BEGIN
	FOREACH /node INTO node DO SELECT P2.ID AS "ID",P2.name FROM tree AS P1, tree AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P1.name = $(name) ORDER BY P2.ID;
END

--
-- treeSelectNodeAndChildren       :Get the node and its children
-- treeSelectNodeAndChildrenByName :Get the node and its children by name
--
TRANSACTION treeSelectNodeAndChildren -- (/node/id)
BEGIN
	FOREACH /node INTO node DO SELECT P1.ID AS "ID",P1.parent,P1.name FROM tree AS P1, tree AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P2.ID = $(id) ORDER BY P1.ID;
END
TRANSACTION treeSelectNodeAndChildrenByName -- (/node/name)
BEGIN
	FOREACH /node INTO node DO SELECT P1.ID AS "ID",P1.parent,P1.name FROM tree AS P1, tree AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P2.name = $(name) ORDER BY P1.ID;
END

--
-- treeSelectChildren       :Get the children of a node
-- treeSelectChildrenByName :Get the children of a node by name
--
TRANSACTION treeSelectChildren -- (/node/id)
BEGIN
	FOREACH /node INTO node DO SELECT P1.ID AS "ID",P1.name FROM tree AS P1, tree AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P2.ID = $(id) ORDER BY P1.ID;
END
TRANSACTION treeSelectChildrenByName -- (/node/name)
BEGIN
	FOREACH /node INTO node DO SELECT P1.ID AS "ID",P1.name FROM tree AS P1, tree AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P2.name = $(name) ORDER BY P1.ID;
END


--
-- treeMoveNode             :Move a node from one parent to another
--
TRANSACTION treeMoveNode -- (nodeid, newparentid)
BEGIN
	-- get parent boundaries
	DO NONEMPTY UNIQUE SELECT lft,rgt FROM tree WHERE ID = $(newparentid);

	-- mark parent left and child width
	-- verify constraint that new parent is not a child of the copied/moved node
	DO NONEMPTY UNIQUE SELECT $1,rgt-lft AS width FROM tree WHERE ID = $(nodeid) AND NOT ($1 >= lft AND $2 < rgt);

	-- get place for the move/copy in the destination node area
	DO UPDATE tree SET rgt = rgt + $2 WHERE rgt >= $1;
	DO UPDATE tree SET lft = lft + $2 WHERE lft > $1;

	-- Get the variables we need for the move/copy
	-- $1 = lft destination
	-- $2 = rgt destination
	-- $3 = width destination
	-- $4 = lft node
	-- $5 = rgt node
	-- $6 = width node
	DO NONEMPTY UNIQUE SELECT lft,rgt,rgt-lft AS width FROM tree WHERE ID = $(newparentid);
	DO NONEMPTY UNIQUE SELECT $1,$2,$3,lft,rgt,rgt-lft AS width FROM tree WHERE ID = $(nodeid);

	-- make a copy of the node to move/copy as child of the destination node
	DO INSERT INTO TREE
		SELECT P1.ID AS "ID",
			P1.parent AS parent,
			P1.lgt-$4+$1 AS lgt,
			P1.rgt-$5+$2 AS rgt,
			P1.name AS name
		FROM tree AS P1, tree AS P2
		WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P2.ID = $(nodeid);

	-- delete the original node
	DO DELETE FROM tree WHERE lft >= $4 AND lft <= $5;
	DO UPDATE tree SET lft = lft-$6 WHERE lft>=$5;
	DO UPDATE tree SET rgt = rgt-$6 WHERE rgt>$5;
END


--
-- treeCopyNode             :Copy a node in the tree
--
TRANSACTION treeCopyNode -- (nodeid, newparentid)
BEGIN
	-- get parent boundaries
	DO NONEMPTY UNIQUE SELECT lft,rgt FROM tree WHERE ID = $(newparentid);

	-- mark parent left and child width
	-- verify constraint that new parent is not a child of the copied/moved node
	DO NONEMPTY UNIQUE SELECT $1,rgt-lft AS width FROM tree WHERE ID = $(nodeid) AND NOT ($1 >= lft AND $2 < rgt);

	-- get place for the move/copy in the destination node area
	DO UPDATE tree SET rgt = rgt + $2 WHERE rgt >= $1;
	DO UPDATE tree SET lft = lft + $2 WHERE lft > $1;

	-- Get the variables we need for the move/copy
	-- $1 = lft destination
	-- $2 = rgt destination
	-- $3 = width destination
	-- $4 = lft node
	-- $5 = rgt node
	-- $6 = width node
	DO NONEMPTY UNIQUE SELECT lft,rgt,rgt-lft AS width FROM tree WHERE ID = $(newparentid);
	DO NONEMPTY UNIQUE SELECT $1,$2,$3,lft,rgt,rgt-lft AS width FROM tree WHERE ID = $(nodeid);

	-- make a copy of the node to move/copy as child of the destination node
	DO INSERT INTO TREE
		SELECT P1.ID AS "ID",
			P1.parent AS parent,
			P1.lgt-$4+$1 AS lgt,
			P1.rgt-$5+$2 AS rgt,
			P1.name AS name
		FROM tree AS P1, tree AS P2
		WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P2.ID = $(nodeid);
END
**outputfile:DBDUMP
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<result><class name="indogermanic"><class name="celtic"><class name="gaulisch"/><class name="goidelic"><class name="old irish"/><class name="middle irish"/><class name="manx"/><class name="irish"/><class name="scotts gaelic"/></class><class name="brythonic"><class name="comish"/><class name="welsh"/><class name="breton"/></class></class><class name="germanic"><class name="west germanic"><class name="anglo-frisian"><class name="old english"><class name="middle english"><class name="english"/></class></class><class name="old frisian"><class name="frisian"/></class></class><class name="german"><class name="low german"><class name="old saxon"><class name="plattdeutsch"/></class><class name="old low franconian"><class name="dutch"/><class name="flemish"/><class name="afrikaans"/><class name="south african dutch"/></class></class><class name="high german"><class name="alemannic"/><class name="alsatian"/><class name="bavarian"/><class name="franconian"/><class name="german"/><class name="pensilvania german"/><class name="swiss"/><class name="yiddish"/></class></class></class><class name="east germanic"/><class name="north germanic"><class name="old west norse"><class name="islandic"/><class name="faroese"/></class><class name="old east norse"><class name="norwegian"/><class name="danish"/><class name="swedish"/></class></class></class><class name="italic"><class name="oscan"/><class name="umbrian"/><class name="old latin"><class name="catalan"/><class name="french"/><class name="galician"/><class name="portuguese"/><class name="italian"/><class name="provencal"/><class name="romansch"/><class name="romanian"/><class name="spanish"/></class></class><class name="slavonic"><class name="west slavic"><class name="chech"/><class name="polish"/><class name="slovak"/><class name="sorbian"/></class><class name="east slavic"><class name="belarussian"/><class name="russian"/><class name="ukrainian"/></class><class name="south slavic"><class name="bosnian"/><class name="bulgarian"/><class name="macedonian"/><class name="serbo-croatian"/><class name="slovene"/></class></class><class name="albanian"/><class name="armenian"/><class name="hellenic"><class name="greek"/></class><class name="baltic"><class name="lettish"/><class name="latvian"/><class name="lithuanian"/></class><class name="hittie"/><class name="indo iranian"><class name="iranian"><class name="avestan"><class name="pashto"/></class><class name="old persian"><class name="balushti"/><class name="kurdish"/><class name="ossetic"/><class name="pashto"/><class name="persian"/></class><class name="scythian"/></class><class name="indic"><class name="sanskrit"/><class name="prakrit"/><class name="pali"/><class name="bengali"/><class name="bihari"/><class name="bhili"/><class name="gujarati"/><class name="hindi"/><class name="hindustani"/><class name="marati"/><class name="nepali"/><class name="bahari"/><class name="punjabi"/><class name="rajasthani"/><class name="sindhi"/><class name="singhalese"/><class name="urdu"/></class></class><class name="tocharian"/></class><subtree name="italic"><node id="49" name="italic"/><node id="50" name="oscan"/><node id="51" name="umbrian"/><node id="52" name="old latin"/><node id="53" name="catalan"/><node id="54" name="french"/><node id="55" name="galician"/><node id="56" name="portuguese"/><node id="57" name="italian"/><node id="58" name="provencal"/><node id="59" name="romansch"/><node id="60" name="romanian"/><node id="61" name="spanish"/></subtree><subtree name="brythonic"><node id="10" name="brythonic"/><node id="11" name="comish"/><node id="12" name="welsh"/><node id="13" name="breton"/></subtree><subtree name="germanic"><node id="14" name="germanic"/><node id="15" name="west germanic"/><node id="16" name="anglo-frisian"/><node id="17" name="old english"/><node id="18" name="middle english"/><node id="19" name="english"/><node id="20" name="old frisian"/><node id="21" name="frisian"/><node id="22" name="german"/><node id="23" name="low german"/><node id="24" name="old saxon"/><node id="25" name="plattdeutsch"/><node id="26" name="old low franconian"/><node id="27" name="dutch"/><node id="28" name="flemish"/><node id="29" name="afrikaans"/><node id="30" name="south african dutch"/><node id="31" name="high german"/><node id="32" name="alemannic"/><node id="33" name="alsatian"/><node id="34" name="bavarian"/><node id="35" name="franconian"/><node id="36" name="german"/><node id="37" name="pensilvania german"/><node id="38" name="swiss"/><node id="39" name="yiddish"/><node id="40" name="east germanic"/><node id="41" name="north germanic"/><node id="42" name="old west norse"/><node id="43" name="islandic"/><node id="44" name="faroese"/><node id="45" name="old east norse"/><node id="46" name="norwegian"/><node id="47" name="danish"/><node id="48" name="swedish"/></subtree><subtree name="anglo-frisian"><node id="16" name="anglo-frisian"/><node id="17" name="old english"/><node id="18" name="middle english"/><node id="19" name="english"/><node id="20" name="old frisian"/><node id="21" name="frisian"/></subtree><children name="gaulisch"/><children name="slavonic"><node id="63" name="west slavic"/><node id="64" name="chech"/><node id="65" name="polish"/><node id="66" name="slovak"/><node id="67" name="sorbian"/><node id="68" name="east slavic"/><node id="69" name="belarussian"/><node id="70" name="russian"/><node id="71" name="ukrainian"/><node id="72" name="south slavic"/><node id="73" name="bosnian"/><node id="74" name="bulgarian"/><node id="75" name="macedonian"/><node id="76" name="serbo-croatian"/><node id="77" name="slovene"/></children><children name="east germanic"/><children name="indic"><node id="99" name="sanskrit"/><node id="100" name="prakrit"/><node id="101" name="pali"/><node id="102" name="bengali"/><node id="103" name="bihari"/><node id="104" name="bhili"/><node id="105" name="gujarati"/><node id="106" name="hindi"/><node id="107" name="hindustani"/><node id="108" name="marati"/><node id="109" name="nepali"/><node id="110" name="bahari"/><node id="111" name="punjabi"/><node id="112" name="rajasthani"/><node id="113" name="sindhi"/><node id="114" name="singhalese"/><node id="115" name="urdu"/></children><cover name="italic"><node id="1" name="indogermanic"/><node id="49" name="italic"/></cover><cover name="brythonic"><node id="1" name="indogermanic"/><node id="2" name="celtic"/><node id="10" name="brythonic"/></cover><cover name="germanic"><node id="1" name="indogermanic"/><node id="14" name="germanic"/></cover><cover name="anglo-frisian"><node id="1" name="indogermanic"/><node id="14" name="germanic"/><node id="15" name="west germanic"/><node id="16" name="anglo-frisian"/></cover><parents name="gaulisch"><node id="1" name="indogermanic"/><node id="2" name="celtic"/></parents><parents name="slavonic"><node id="1" name="indogermanic"/></parents><parents name="east germanic"><node id="1" name="indogermanic"/><node id="14" name="germanic"/></parents><parents name="indic"><node id="1" name="indogermanic"/><node id="87" name="indo iranian"/></parents><sparsetree><class name="indogermanic"><class name="germanic"><class name="west germanic"><class name="anglo-frisian"><class name="old english"><class name="middle english"><class name="english"/></class></class><class name="old frisian"><class name="frisian"/></class></class><class name="german"><class name="low german"><class name="old saxon"><class name="plattdeutsch"/></class><class name="old low franconian"><class name="dutch"/><class name="flemish"/><class name="afrikaans"/><class name="south african dutch"/></class></class><class name="high german"><class name="alemannic"/><class name="alsatian"/><class name="bavarian"/><class name="franconian"/><class name="german"/><class name="pensilvania german"/><class name="swiss"><class name="bern german"/><class name="eastern swiss german"/><class name="grison german"/></class><class name="yiddish"/></class></class></class><class name="east germanic"/><class name="north germanic"><class name="old west norse"><class name="islandic"/><class name="faroese"/></class><class name="old east norse"><class name="norwegian"/><class name="danish"/><class name="swedish"/></class></class></class><class name="italic"><class name="oscan"/><class name="umbrian"/><class name="old latin"><class name="catalan"/><class name="french"/><class name="galician"/><class name="portuguese"/><class name="italian"/><class name="provencal"/><class name="romansch"/><class name="romanian"/><class name="spanish"/></class></class><class name="slavonic"><class name="west slavic"><class name="chech"/><class name="polish"/><class name="slovak"/><class name="sorbian"/></class><class name="east slavic"><class name="belarussian"/><class name="russian"/><class name="ukrainian"/></class><class name="south slavic"><class name="bosnian"/><class name="bulgarian"/><class name="macedonian"/><class name="serbo-croatian"/><class name="slovene"/></class></class><class name="albanian"/><class name="armenian"/><class name="baltic"><class name="lettish"/><class name="latvian"/><class name="lithuanian"/></class><class name="tocharian"/><class name="hittie"/></class></sparsetree></result>
tree:
id, parent, name, lft, rgt
'1', NULL, 'indogermanic', '1', '152'
'14', '1', 'germanic', '2', '77'
'15', '14', 'west germanic', '3', '58'
'16', '15', 'anglo-frisian', '4', '15'
'17', '16', 'old english', '5', '10'
'18', '17', 'middle english', '6', '9'
'19', '18', 'english', '7', '8'
'20', '16', 'old frisian', '11', '14'
'21', '20', 'frisian', '12', '13'
'22', '15', 'german', '16', '57'
'23', '22', 'low german', '17', '32'
'24', '23', 'old saxon', '18', '21'
'25', '24', 'plattdeutsch', '19', '20'
'26', '23', 'old low franconian', '22', '31'
'27', '26', 'dutch', '23', '24'
'28', '26', 'flemish', '25', '26'
'29', '26', 'afrikaans', '27', '28'
'30', '26', 'south african dutch', '29', '30'
'31', '22', 'high german', '33', '56'
'32', '31', 'alemannic', '34', '35'
'33', '31', 'alsatian', '36', '37'
'34', '31', 'bavarian', '38', '39'
'35', '31', 'franconian', '40', '41'
'36', '31', 'german', '42', '43'
'37', '31', 'pensilvania german', '44', '45'
'38', '31', 'swiss', '46', '53'
'39', '31', 'yiddish', '54', '55'
'40', '14', 'east germanic', '59', '60'
'41', '14', 'north germanic', '61', '76'
'42', '41', 'old west norse', '62', '67'
'43', '42', 'islandic', '63', '64'
'44', '42', 'faroese', '65', '66'
'45', '41', 'old east norse', '68', '75'
'46', '45', 'norwegian', '69', '70'
'47', '45', 'danish', '71', '72'
'48', '45', 'swedish', '73', '74'
'49', '1', 'italic', '78', '103'
'50', '49', 'oscan', '79', '80'
'51', '49', 'umbrian', '81', '82'
'52', '49', 'old latin', '83', '102'
'53', '52', 'catalan', '84', '85'
'54', '52', 'french', '86', '87'
'55', '52', 'galician', '88', '89'
'56', '52', 'portuguese', '90', '91'
'57', '52', 'italian', '92', '93'
'58', '52', 'provencal', '94', '95'
'59', '52', 'romansch', '96', '97'
'60', '52', 'romanian', '98', '99'
'61', '52', 'spanish', '100', '101'
'62', '1', 'slavonic', '104', '135'
'63', '62', 'west slavic', '105', '114'
'64', '63', 'chech', '106', '107'
'65', '63', 'polish', '108', '109'
'66', '63', 'slovak', '110', '111'
'67', '63', 'sorbian', '112', '113'
'68', '62', 'east slavic', '115', '122'
'69', '68', 'belarussian', '116', '117'
'70', '68', 'russian', '118', '119'
'71', '68', 'ukrainian', '120', '121'
'72', '62', 'south slavic', '123', '134'
'73', '72', 'bosnian', '124', '125'
'74', '72', 'bulgarian', '126', '127'
'75', '72', 'macedonian', '128', '129'
'76', '72', 'serbo-croatian', '130', '131'
'77', '72', 'slovene', '132', '133'
'78', '1', 'albanian', '136', '137'
'79', '1', 'armenian', '138', '139'
'82', '1', 'baltic', '140', '147'
'83', '82', 'lettish', '141', '142'
'84', '82', 'latvian', '143', '144'
'85', '82', 'lithuanian', '145', '146'
'116', '1', 'tocharian', '148', '149'
'117', '38', 'bern german', '47', '48'
'118', '38', 'eastern swiss german', '49', '50'
'119', '38', 'grison german', '51', '52'
'120', '1', 'hittie', '150', '151'
**end

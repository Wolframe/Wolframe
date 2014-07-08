**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**requires:PGSQL
**input
{
	"treeAddRoot": { "name": "Albert" },
	"treeAddNode": [
		{
			"parentid": "1",
			"name": "Bert"
		},
		{
			"parentid": "1",
			"name": "Chuck"
		},
		{
			"parentid": "3",
			"name": "Donna"
		},
		{
			"parentid": "3",
			"name": "Eddie"
		},
		{
			"parentid": "3",
			"name": "Fred"
		}
	]
}**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson --module ../../src/modules/doctype/json/mod_doctype_json -c wolframe.conf run

**file:wolframe.conf
LoadModules
{
	module ../wolfilter/modules/database/postgresql/mod_db_postgresqltest
	module ../../src/modules/cmdbind/tdl/mod_command_tdl
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
function run()
	provider.filter().empty = false
	output:opentag( "result")			-- top level result tag
	local itr = input:get()
	for v,t in itr do
		if t and t ~= "transactions" then	-- top level tag names are the transaction names
			f = provider.formfunction( t )	-- call the transaction
			output:print( f( itr))		-- print the result
		end
	end
	output:closetag()				-- close result
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
{
	"result":	["", "ID { '2' } ", "ID { '3' } ", "ID { '4' } ", "ID { '5' } ", "ID { '6' } "]
}
tree:
id, parent, name, lft, rgt
'1', NULL, 'Albert', '1', '12'
'2', '1', 'Bert', '2', '3'
'3', '1', 'Chuck', '4', '11'
'4', '3', 'Donna', '5', '6'
'5', '3', 'Eddie', '7', '8'
'6', '3', 'Fred', '9', '10'
**end

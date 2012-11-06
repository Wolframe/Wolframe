**
**requires:LUA
**requires:LIBXML2
**requires:SQLITE3
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<transactions>
<treeAddRoot><node><name>Albert</name></node></treeAddRoot>
<treeAddNode><node><parentid>1</parentid><name>Bert</name></node></treeAddNode>
<treeAddNode><node><parentid>1</parentid><name>Chuck</name></node></treeAddNode>
<treeAddNode><node><parentid>3</parentid><name>Donna</name></node></treeAddNode>
<treeAddNode><node><parentid>3</parentid><name>Eddie</name></node></treeAddNode>
<treeAddNode><node><parentid>3</parentid><name>Fred</name></node></treeAddNode>
</transactions>**config
--input-filter xml:libxml2 --output-filter xml:libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2  --module ../../src/modules/cmdbind/lua/mod_command_lua --program=transaction_sqlite_demo_tree.lua --program simpleform.normalize --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../wolfilter/modules/database/sqlite3/mod_db_sqlite3test --database 'identifier=testdb,file=test.db,dumpfile=DBDUMP,inputfile=DBDATA' --program=DBPRG.tdl run

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
 lft INT NOT NULL UNIQUE DEFERRABLE CHECK ( lft > 0 ),
 rgt INT NOT NULL UNIQUE DEFERRABLE CHECK ( rgt > 1 ),
 CONSTRAINT order_check CHECK ( rgt > lft )
);
**file:DBPRG.tdl
--
-- treeAddRoot
--
TRANSACTION treeAddRoot -- (/node/name)
BEGIN
	FOREACH /node DO INSERT INTO tree (parent, name, lft, rgt) VALUES (0, $(name), 1, 2);
END

--
-- treeAddNode
--
TRANSACTION treeAddNode -- (/node/parentid, /node/name)
BEGIN
	FOREACH /node DO NONEMPTY SELECT rgt FROM tree WHERE ID = $(parentid);
	DO UPDATE tree SET lft = lft + 2 WHERE lft > $1;
	DO UPDATE tree SET rgt = rgt + 2 WHERE rgt >= $1;
	FOREACH /node DO INSERT INTO tree (parent, name, lft, rgt) VALUES ($(parentid), $(name), $1, $1+1);
END

--
-- treeDeleteSubtree
--
TRANSACTION treeDeleteSubtree -- ( /node/id)
BEGIN
	FOREACH /node DO NONEMPTY SELECT lft,rgt,width=rgt-lft+1 FROM tree WHERE ID = $(id);
	DO DELETE FROM tree WHERE lft >= $1 AND lft <= $2;
	DO UPDATE tree SET lft = lft-$3 WHERE lft>$2;
	DO UPDATE tree SET rgt = rgt-$3 WHERE rgt>$2;
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
	FOREACH /node INTO /node DO SELECT P2.ID,P2.name FROM tree AS P1, tree AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P2.ID = $(id);
END
TRANSACTION treeSelectNodeAndChildrenByName -- (/node/name)
BEGIN
	FOREACH /node INTO /node DO SELECT P2.ID,P2.name FROM tree AS P1, tree AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P2.name = $(name);
END

--
-- treeSelectChildren       :Get the children of a node
-- treeSelectChildrenByName :Get the children of a node by name
--
TRANSACTION treeSelectChildren -- (/node/id)
BEGIN
	FOREACH /node INTO /node DO SELECT P2.ID,P2.name FROM tree AS P1, tree AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P2.ID = $(id);
END
TRANSACTION treeSelectChildrenByName -- (/node/name)
BEGIN
	FOREACH /node INTO /node DO SELECT P2.ID,P2.name FROM tree AS P1, tree AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P2.name = $(name);
END

--
-- treeSelectNodeWithLevels :Get all the nodes with their level
--
TRANSACTION treeSelectNodeWithLevels
BEGIN
	INTO /node DO SELECT count(P2.*) AS level, P1.ID, P1.name FROM tree AS P1, tree AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt GROUP BY P1.name ORDER BY level ASC;
END
**outputfile:DBDUMP
**file: transaction_sqlite_demo_tree.lua
function run()
	filter().empty = false
	output:opentag( "result")			-- top level result tag
	for v,t in input:get() do
		if t and t ~= "transactions" then	-- top level tag names are the transaction names
			q = input:table()		-- query is subtree
			f = formfunction( t )		-- call the transaction
			output:print( f( q ))		-- print the result
		end
	end
	output:closetag()				-- close result
end

**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<result></result>
tree:
'1', '0', 'Albert', '1', '12'
'2', '1', 'Bert', '2', '3'
'3', '1', 'Chuck', '4', '11'
'4', '3', 'Donna', '5', '6'
'5', '3', 'Eddie', '7', '8'
'6', '3', 'Fred', '9', '10'

sqlite_sequence:
'tree', '6'

**end

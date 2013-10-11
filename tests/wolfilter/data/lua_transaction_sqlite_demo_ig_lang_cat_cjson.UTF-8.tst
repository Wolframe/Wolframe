**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**requires:SQLITE3
**input
{
  "class": {
    "-name": "indogermanic",
    "class": [
      {
        "-name": "celtic",
        "-status": "living",
        "class": [
          {
            "-name": "gaulisch",
            "-status": "dead"
          },
          {
            "-name": "goidelic",
            "-status": "dead",
            "class": [
              {
                "-name": "old irish",
                "-status": "dead"
              },
              {
                "-name": "middle irish",
                "-status": "dead"
              },
              {
                "-name": "manx",
                "-status": "dead"
              },
              {
                "-name": "irish",
                "-status": "living"
              },
              {
                "-name": "scotts gaelic",
                "-status": "living"
              }
            ]
          },
          {
            "-name": "brythonic",
            "-status": "dead",
            "class": [
              {
                "-name": "comish",
                "-status": "dead"
              },
              {
                "-name": "welsh",
                "-status": "living"
              },
              {
                "-name": "breton",
                "-status": "living"
              }
            ]
          }
        ]
      },
      {
        "-name": "germanic",
        "-status": "living",
        "class": [
          {
            "-name": "west germanic",
            "-status": "living",
            "class": [
              {
                "-name": "anglo-frisian",
                "-status": "living",
                "class": [
                  {
                    "-name": "old english",
                    "-status": "dead",
                    "class": {
                      "-name": "middle english",
                      "-status": "dead",
                      "class": {
                        "-name": "english",
                        "-status": "living"
                      }
                    }
                  },
                  {
                    "-name": "old frisian",
                    "-status": "dead",
                    "class": {
                      "-name": "frisian",
                      "-status": "living"
                    }
                  }
                ]
              },
              {
                "-name": "german",
                "-status": "living",
                "class": [
                  {
                    "-name": "low german",
                    "-status": "living",
                    "class": [
                      {
                        "-name": "old saxon",
                        "-status": "dead",
                        "class": {
                          "-name": "plattdeutsch",
                          "-status": "dead"
                        }
                      },
                      {
                        "-name": "old low franconian",
                        "-status": "dead",
                        "class": [
                          {
                            "-name": "dutch",
                            "-status": "living"
                          },
                          {
                            "-name": "flemish",
                            "-status": "living"
                          },
                          {
                            "-name": "afrikaans",
                            "-status": "living"
                          },
                          {
                            "-name": "south african dutch",
                            "-status": "dead"
                          }
                        ]
                      }
                    ]
                  },
                  {
                    "-name": "high german",
                    "-status": "living",
                    "class": [
                      {
                        "-name": "alemannic",
                        "-status": "living"
                      },
                      {
                        "-name": "alsatian",
                        "-status": "living"
                      },
                      {
                        "-name": "bavarian",
                        "-status": "living"
                      },
                      {
                        "-name": "franconian",
                        "-status": "living"
                      },
                      {
                        "-name": "german",
                        "-status": "living"
                      },
                      {
                        "-name": "pensilvania german",
                        "-status": "living"
                      },
                      {
                        "-name": "swiss",
                        "-status": "living"
                      },
                      {
                        "-name": "yiddish",
                        "-status": "living"
                      }
                    ]
                  }
                ]
              }
            ]
          },
          {
            "-name": "east germanic",
            "-status": "living"
          },
          {
            "-name": "north germanic",
            "-status": "living",
            "class": [
              {
                "-name": "old west norse",
                "-status": "living",
                "class": [
                  {
                    "-name": "islandic",
                    "-status": "living"
                  },
                  {
                    "-name": "faroese",
                    "-status": "living"
                  }
                ]
              },
              {
                "-name": "old east norse",
                "-status": "living",
                "class": [
                  {
                    "-name": "norwegian",
                    "-status": "living"
                  },
                  {
                    "-name": "danish",
                    "-status": "living"
                  },
                  {
                    "-name": "swedish",
                    "-status": "living"
                  }
                ]
              }
            ]
          }
        ]
      },
      {
        "-name": "italic",
        "-status": "living",
        "class": [
          {
            "-name": "oscan",
            "-status": "dead"
          },
          {
            "-name": "umbrian",
            "-status": "dead"
          },
          {
            "-name": "old latin",
            "-status": "living",
            "class": [
              {
                "-name": "catalan",
                "-status": "living"
              },
              {
                "-name": "french",
                "-status": "living"
              },
              {
                "-name": "galician",
                "-status": "living"
              },
              {
                "-name": "portuguese",
                "-status": "living"
              },
              {
                "-name": "italian",
                "-status": "living"
              },
              {
                "-name": "provencal",
                "-status": "living"
              },
              {
                "-name": "romansch",
                "-status": "living"
              },
              {
                "-name": "romanian",
                "-status": "living"
              },
              {
                "-name": "spanish",
                "-status": "living"
              }
            ]
          }
        ]
      },
      {
        "-name": "slavonic",
        "-status": "living",
        "class": [
          {
            "-name": "west slavic",
            "-status": "living",
            "class": [
              {
                "-name": "chech",
                "-status": "living"
              },
              {
                "-name": "polish",
                "-status": "living"
              },
              {
                "-name": "slovak",
                "-status": "living"
              },
              {
                "-name": "sorbian",
                "-status": "living"
              }
            ]
          },
          {
            "-name": "east slavic",
            "-status": "living",
            "class": [
              {
                "-name": "belarussian",
                "-status": "living"
              },
              {
                "-name": "russian",
                "-status": "living"
              },
              {
                "-name": "ukrainian",
                "-status": "living"
              }
            ]
          },
          {
            "-name": "south slavic",
            "-status": "living",
            "class": [
              {
                "-name": "bosnian",
                "-status": "living"
              },
              {
                "-name": "bulgarian",
                "-status": "living"
              },
              {
                "-name": "macedonian",
                "-status": "living"
              },
              {
                "-name": "serbo-croatian",
                "-status": "living"
              },
              {
                "-name": "slovene",
                "-status": "living"
              }
            ]
          }
        ]
      },
      {
        "-name": "albanian",
        "-status": "living"
      },
      {
        "-name": "armenian",
        "-status": "living"
      },
      {
        "-name": "hellenic",
        "-status": "living",
        "class": {
          "-name": "greek",
          "-status": "living"
        }
      },
      {
        "-name": "baltic",
        "-status": "living",
        "class": [
          {
            "-name": "lettish",
            "-status": "living"
          },
          {
            "-name": "latvian",
            "-status": "living"
          },
          {
            "-name": "lithuanian",
            "-status": "living"
          }
        ]
      },
      {
        "-name": "hittie",
        "-status": "dead"
      },
      {
        "-name": "indo iranian",
        "-status": "living",
        "class": [
          {
            "-name": "iranian",
            "-status": "living",
            "class": [
              {
                "-name": "avestan",
                "-status": "dead",
                "class": {
                  "-name": "pashto",
                  "-status": "living"
                }
              },
              {
                "-name": "old persian",
                "-status": "dead",
                "class": [
                  {
                    "-name": "balushti",
                    "-status": "living"
                  },
                  {
                    "-name": "kurdish",
                    "-status": "living"
                  },
                  {
                    "-name": "ossetic",
                    "-status": "living"
                  },
                  {
                    "-name": "pashto",
                    "-status": "living"
                  },
                  {
                    "-name": "persian",
                    "-status": "living"
                  }
                ]
              },
              {
                "-name": "scythian",
                "-status": "dead"
              }
            ]
          },
          {
            "-name": "indic",
            "-status": "living",
            "class": [
              {
                "-name": "sanskrit",
                "-status": "dead"
              },
              {
                "-name": "prakrit",
                "-status": "dead"
              },
              {
                "-name": "pali",
                "-status": "dead"
              },
              {
                "-name": "bengali",
                "-status": "living"
              },
              {
                "-name": "bihari",
                "-status": "living"
              },
              {
                "-name": "bhili",
                "-status": "living"
              },
              {
                "-name": "gujarati",
                "-status": "living"
              },
              {
                "-name": "hindi",
                "-status": "living"
              },
              {
                "-name": "hindustani",
                "-status": "living"
              },
              {
                "-name": "marati",
                "-status": "living"
              },
              {
                "-name": "nepali",
                "-status": "living"
              },
              {
                "-name": "bahari",
                "-status": "living"
              },
              {
                "-name": "punjabi",
                "-status": "living"
              },
              {
                "-name": "rajasthani",
                "-status": "living"
              },
              {
                "-name": "sindhi",
                "-status": "living"
              },
              {
                "-name": "singhalese",
                "-status": "living"
              },
              {
                "-name": "urdu",
                "-status": "living"
              }
            ]
          }
        ]
      },
      {
        "-name": "tocharian",
        "-status": "dead"
      }
    ]
  }
}**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson  --module ../../src/modules/cmdbind/lua/mod_command_lua --cmdprogram=transaction_demo_ig_lang_cat.lua --program simpleform.wnmp --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/normalize//string/mod_normalize_string --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../wolfilter/modules/database/sqlite3/mod_db_sqlite3test --database 'identifier=testdb,file=test.db,dumpfile=DBDUMP,inputfile=DBDATA' --program=DBPRG.tdl run

**file:simpleform.wnmp
int=number:integer;
uint=number:unsigned;
float=number:float;
currency=number:fixedpoint(13,2);
percent_1=number:fixedpoint(5,1);
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
**file: transaction_demo_ig_lang_cat.lua
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

function copy_node( name, parentname)
	local parentid = formfunction( "treeSelectNodeByName")( { node={ name=parentname } } ):table().ID
	local nodeid = formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table().ID
	formfunction( "treeCopyNode")( {nodeid=nodeid, newparentid=parentid} )
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
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
	end
	output:closetag()
end

function select_subtree2( name)
	local nodear = formfunction( "treeSelectNodeAndChildrenByName")( { node={ name=name } } ):table()["node"] or {}
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
	local id = formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table().ID
	local nodear = formfunction( "treeSelectChildren")( { node={ id=id } } ):table()["node"] or {}
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
	local nodear = formfunction( "treeSelectChildrenByName")( { node={ name=name } } ):table()["node"] or {}
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
	local id = formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table().ID
	local nodear = formfunction( "treeSelectNodeAndParents")( { node={ id=id } } ):table()["node"] or {}
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
	local nodear = formfunction( "treeSelectNodeAndParentsByName")( { node={ name=name } } ):table()["node"] or {}
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
	local id = formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table().ID
	local nodear = formfunction( "treeSelectParents")( { node={ id=id } } ):table()["node"] or {}
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
	local nodear = formfunction( "treeSelectParentsByName")( { node={ name=name } } ):table()["node"] or {}
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
	local t = formfunction( "treeSelectNodeAndChildren")( { node = { id=parentid } } ):table()["node"] or {}
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
	filter().empty = false
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

**output
{
	"result":	{
		"class":	{
			"name":	"indogermanic",
			"class":	[{
					"name":	"celtic",
					"class":	[{
							"name":	"gaulisch"
						}, {
							"name":	"goidelic",
							"class":	[{
									"name":	"old irish"
								}, {
									"name":	"middle irish"
								}, {
									"name":	"manx"
								}, {
									"name":	"irish"
								}, {
									"name":	"scotts gaelic"
								}]
						}, {
							"name":	"brythonic",
							"class":	[{
									"name":	"comish"
								}, {
									"name":	"welsh"
								}, {
									"name":	"breton"
								}]
						}]
				}, {
					"name":	"germanic",
					"class":	[{
							"name":	"west germanic",
							"class":	[{
									"name":	"anglo-frisian",
									"class":	[{
											"name":	"old english",
											"class":	{
												"name":	"middle english",
												"class":	{
													"name":	"english"
												}
											}
										}, {
											"name":	"old frisian",
											"class":	{
												"name":	"frisian"
											}
										}]
								}, {
									"name":	"german",
									"class":	[{
											"name":	"low german",
											"class":	[{
													"name":	"old saxon",
													"class":	{
														"name":	"plattdeutsch"
													}
												}, {
													"name":	"old low franconian",
													"class":	[{
															"name":	"dutch"
														}, {
															"name":	"flemish"
														}, {
															"name":	"afrikaans"
														}, {
															"name":	"south african dutch"
														}]
												}]
										}, {
											"name":	"high german",
											"class":	[{
													"name":	"alemannic"
												}, {
													"name":	"alsatian"
												}, {
													"name":	"bavarian"
												}, {
													"name":	"franconian"
												}, {
													"name":	"german"
												}, {
													"name":	"pensilvania german"
												}, {
													"name":	"swiss"
												}, {
													"name":	"yiddish"
												}]
										}]
								}]
						}, {
							"name":	"east germanic"
						}, {
							"name":	"north germanic",
							"class":	[{
									"name":	"old west norse",
									"class":	[{
											"name":	"islandic"
										}, {
											"name":	"faroese"
										}]
								}, {
									"name":	"old east norse",
									"class":	[{
											"name":	"norwegian"
										}, {
											"name":	"danish"
										}, {
											"name":	"swedish"
										}]
								}]
						}]
				}, {
					"name":	"italic",
					"class":	[{
							"name":	"oscan"
						}, {
							"name":	"umbrian"
						}, {
							"name":	"old latin",
							"class":	[{
									"name":	"catalan"
								}, {
									"name":	"french"
								}, {
									"name":	"galician"
								}, {
									"name":	"portuguese"
								}, {
									"name":	"italian"
								}, {
									"name":	"provencal"
								}, {
									"name":	"romansch"
								}, {
									"name":	"romanian"
								}, {
									"name":	"spanish"
								}]
						}]
				}, {
					"name":	"slavonic",
					"class":	[{
							"name":	"west slavic",
							"class":	[{
									"name":	"chech"
								}, {
									"name":	"polish"
								}, {
									"name":	"slovak"
								}, {
									"name":	"sorbian"
								}]
						}, {
							"name":	"east slavic",
							"class":	[{
									"name":	"belarussian"
								}, {
									"name":	"russian"
								}, {
									"name":	"ukrainian"
								}]
						}, {
							"name":	"south slavic",
							"class":	[{
									"name":	"bosnian"
								}, {
									"name":	"bulgarian"
								}, {
									"name":	"macedonian"
								}, {
									"name":	"serbo-croatian"
								}, {
									"name":	"slovene"
								}]
						}]
				}, {
					"name":	"albanian"
				}, {
					"name":	"armenian"
				}, {
					"name":	"hellenic",
					"class":	{
						"name":	"greek"
					}
				}, {
					"name":	"baltic",
					"class":	[{
							"name":	"lettish"
						}, {
							"name":	"latvian"
						}, {
							"name":	"lithuanian"
						}]
				}, {
					"name":	"hittie"
				}, {
					"name":	"indo iranian",
					"class":	[{
							"name":	"iranian",
							"class":	[{
									"name":	"avestan",
									"class":	{
										"name":	"pashto"
									}
								}, {
									"name":	"old persian",
									"class":	[{
											"name":	"balushti"
										}, {
											"name":	"kurdish"
										}, {
											"name":	"ossetic"
										}, {
											"name":	"pashto"
										}, {
											"name":	"persian"
										}]
								}, {
									"name":	"scythian"
								}]
						}, {
							"name":	"indic",
							"class":	[{
									"name":	"sanskrit"
								}, {
									"name":	"prakrit"
								}, {
									"name":	"pali"
								}, {
									"name":	"bengali"
								}, {
									"name":	"bihari"
								}, {
									"name":	"bhili"
								}, {
									"name":	"gujarati"
								}, {
									"name":	"hindi"
								}, {
									"name":	"hindustani"
								}, {
									"name":	"marati"
								}, {
									"name":	"nepali"
								}, {
									"name":	"bahari"
								}, {
									"name":	"punjabi"
								}, {
									"name":	"rajasthani"
								}, {
									"name":	"sindhi"
								}, {
									"name":	"singhalese"
								}, {
									"name":	"urdu"
								}]
						}]
				}, {
					"name":	"tocharian"
				}]
		},
		"subtree":	[{
				"name":	"italic",
				"node":	[{
						"id":	"49",
						"name":	"italic"
					}, {
						"id":	"50",
						"name":	"oscan"
					}, {
						"id":	"51",
						"name":	"umbrian"
					}, {
						"id":	"52",
						"name":	"old latin"
					}, {
						"id":	"53",
						"name":	"catalan"
					}, {
						"id":	"54",
						"name":	"french"
					}, {
						"id":	"55",
						"name":	"galician"
					}, {
						"id":	"56",
						"name":	"portuguese"
					}, {
						"id":	"57",
						"name":	"italian"
					}, {
						"id":	"58",
						"name":	"provencal"
					}, {
						"id":	"59",
						"name":	"romansch"
					}, {
						"id":	"60",
						"name":	"romanian"
					}, {
						"id":	"61",
						"name":	"spanish"
					}]
			}, {
				"name":	"brythonic",
				"node":	[{
						"id":	"10",
						"name":	"brythonic"
					}, {
						"id":	"11",
						"name":	"comish"
					}, {
						"id":	"12",
						"name":	"welsh"
					}, {
						"id":	"13",
						"name":	"breton"
					}]
			}, {
				"name":	"germanic",
				"node":	[{
						"id":	"14",
						"name":	"germanic"
					}, {
						"id":	"15",
						"name":	"west germanic"
					}, {
						"id":	"16",
						"name":	"anglo-frisian"
					}, {
						"id":	"17",
						"name":	"old english"
					}, {
						"id":	"18",
						"name":	"middle english"
					}, {
						"id":	"19",
						"name":	"english"
					}, {
						"id":	"20",
						"name":	"old frisian"
					}, {
						"id":	"21",
						"name":	"frisian"
					}, {
						"id":	"22",
						"name":	"german"
					}, {
						"id":	"23",
						"name":	"low german"
					}, {
						"id":	"24",
						"name":	"old saxon"
					}, {
						"id":	"25",
						"name":	"plattdeutsch"
					}, {
						"id":	"26",
						"name":	"old low franconian"
					}, {
						"id":	"27",
						"name":	"dutch"
					}, {
						"id":	"28",
						"name":	"flemish"
					}, {
						"id":	"29",
						"name":	"afrikaans"
					}, {
						"id":	"30",
						"name":	"south african dutch"
					}, {
						"id":	"31",
						"name":	"high german"
					}, {
						"id":	"32",
						"name":	"alemannic"
					}, {
						"id":	"33",
						"name":	"alsatian"
					}, {
						"id":	"34",
						"name":	"bavarian"
					}, {
						"id":	"35",
						"name":	"franconian"
					}, {
						"id":	"36",
						"name":	"german"
					}, {
						"id":	"37",
						"name":	"pensilvania german"
					}, {
						"id":	"38",
						"name":	"swiss"
					}, {
						"id":	"39",
						"name":	"yiddish"
					}, {
						"id":	"40",
						"name":	"east germanic"
					}, {
						"id":	"41",
						"name":	"north germanic"
					}, {
						"id":	"42",
						"name":	"old west norse"
					}, {
						"id":	"43",
						"name":	"islandic"
					}, {
						"id":	"44",
						"name":	"faroese"
					}, {
						"id":	"45",
						"name":	"old east norse"
					}, {
						"id":	"46",
						"name":	"norwegian"
					}, {
						"id":	"47",
						"name":	"danish"
					}, {
						"id":	"48",
						"name":	"swedish"
					}]
			}, {
				"name":	"anglo-frisian",
				"node":	[{
						"id":	"16",
						"name":	"anglo-frisian"
					}, {
						"id":	"17",
						"name":	"old english"
					}, {
						"id":	"18",
						"name":	"middle english"
					}, {
						"id":	"19",
						"name":	"english"
					}, {
						"id":	"20",
						"name":	"old frisian"
					}, {
						"id":	"21",
						"name":	"frisian"
					}]
			}],
		"children":	[{
				"name":	"gaulisch"
			}, {
				"name":	"slavonic",
				"node":	[{
						"id":	"63",
						"name":	"west slavic"
					}, {
						"id":	"64",
						"name":	"chech"
					}, {
						"id":	"65",
						"name":	"polish"
					}, {
						"id":	"66",
						"name":	"slovak"
					}, {
						"id":	"67",
						"name":	"sorbian"
					}, {
						"id":	"68",
						"name":	"east slavic"
					}, {
						"id":	"69",
						"name":	"belarussian"
					}, {
						"id":	"70",
						"name":	"russian"
					}, {
						"id":	"71",
						"name":	"ukrainian"
					}, {
						"id":	"72",
						"name":	"south slavic"
					}, {
						"id":	"73",
						"name":	"bosnian"
					}, {
						"id":	"74",
						"name":	"bulgarian"
					}, {
						"id":	"75",
						"name":	"macedonian"
					}, {
						"id":	"76",
						"name":	"serbo-croatian"
					}, {
						"id":	"77",
						"name":	"slovene"
					}]
			}, {
				"name":	"east germanic"
			}, {
				"name":	"indic",
				"node":	[{
						"id":	"99",
						"name":	"sanskrit"
					}, {
						"id":	"100",
						"name":	"prakrit"
					}, {
						"id":	"101",
						"name":	"pali"
					}, {
						"id":	"102",
						"name":	"bengali"
					}, {
						"id":	"103",
						"name":	"bihari"
					}, {
						"id":	"104",
						"name":	"bhili"
					}, {
						"id":	"105",
						"name":	"gujarati"
					}, {
						"id":	"106",
						"name":	"hindi"
					}, {
						"id":	"107",
						"name":	"hindustani"
					}, {
						"id":	"108",
						"name":	"marati"
					}, {
						"id":	"109",
						"name":	"nepali"
					}, {
						"id":	"110",
						"name":	"bahari"
					}, {
						"id":	"111",
						"name":	"punjabi"
					}, {
						"id":	"112",
						"name":	"rajasthani"
					}, {
						"id":	"113",
						"name":	"sindhi"
					}, {
						"id":	"114",
						"name":	"singhalese"
					}, {
						"id":	"115",
						"name":	"urdu"
					}]
			}],
		"cover":	[{
				"name":	"italic",
				"node":	[{
						"id":	"1",
						"name":	"indogermanic"
					}, {
						"id":	"49",
						"name":	"italic"
					}]
			}, {
				"name":	"brythonic",
				"node":	[{
						"id":	"1",
						"name":	"indogermanic"
					}, {
						"id":	"2",
						"name":	"celtic"
					}, {
						"id":	"10",
						"name":	"brythonic"
					}]
			}, {
				"name":	"germanic",
				"node":	[{
						"id":	"1",
						"name":	"indogermanic"
					}, {
						"id":	"14",
						"name":	"germanic"
					}]
			}, {
				"name":	"anglo-frisian",
				"node":	[{
						"id":	"1",
						"name":	"indogermanic"
					}, {
						"id":	"14",
						"name":	"germanic"
					}, {
						"id":	"15",
						"name":	"west germanic"
					}, {
						"id":	"16",
						"name":	"anglo-frisian"
					}]
			}],
		"parents":	[{
				"name":	"gaulisch",
				"node":	[{
						"id":	"1",
						"name":	"indogermanic"
					}, {
						"id":	"2",
						"name":	"celtic"
					}]
			}, {
				"name":	"slavonic",
				"node":	{
					"id":	"1",
					"name":	"indogermanic"
				}
			}, {
				"name":	"east germanic",
				"node":	[{
						"id":	"1",
						"name":	"indogermanic"
					}, {
						"id":	"14",
						"name":	"germanic"
					}]
			}, {
				"name":	"indic",
				"node":	[{
						"id":	"1",
						"name":	"indogermanic"
					}, {
						"id":	"87",
						"name":	"indo iranian"
					}]
			}],
		"sparsetree":	{
			"class":	{
				"name":	"indogermanic",
				"class":	[{
						"name":	"germanic",
						"class":	[{
								"name":	"west germanic",
								"class":	[{
										"name":	"anglo-frisian",
										"class":	[{
												"name":	"old english",
												"class":	{
													"name":	"middle english",
													"class":	{
														"name":	"english"
													}
												}
											}, {
												"name":	"old frisian",
												"class":	{
													"name":	"frisian"
												}
											}]
									}, {
										"name":	"german",
										"class":	[{
												"name":	"low german",
												"class":	[{
														"name":	"old saxon",
														"class":	{
															"name":	"plattdeutsch"
														}
													}, {
														"name":	"old low franconian",
														"class":	[{
																"name":	"dutch"
															}, {
																"name":	"flemish"
															}, {
																"name":	"afrikaans"
															}, {
																"name":	"south african dutch"
															}]
													}]
											}, {
												"name":	"high german",
												"class":	[{
														"name":	"alemannic"
													}, {
														"name":	"alsatian"
													}, {
														"name":	"bavarian"
													}, {
														"name":	"franconian"
													}, {
														"name":	"german"
													}, {
														"name":	"pensilvania german"
													}, {
														"name":	"swiss",
														"class":	[{
																"name":	"bern german"
															}, {
																"name":	"eastern swiss german"
															}, {
																"name":	"grison german"
															}]
													}, {
														"name":	"yiddish"
													}]
											}]
									}]
							}, {
								"name":	"east germanic"
							}, {
								"name":	"north germanic",
								"class":	[{
										"name":	"old west norse",
										"class":	[{
												"name":	"islandic"
											}, {
												"name":	"faroese"
											}]
									}, {
										"name":	"old east norse",
										"class":	[{
												"name":	"norwegian"
											}, {
												"name":	"danish"
											}, {
												"name":	"swedish"
											}]
									}]
							}]
					}, {
						"name":	"italic",
						"class":	[{
								"name":	"oscan"
							}, {
								"name":	"umbrian"
							}, {
								"name":	"old latin",
								"class":	[{
										"name":	"catalan"
									}, {
										"name":	"french"
									}, {
										"name":	"galician"
									}, {
										"name":	"portuguese"
									}, {
										"name":	"italian"
									}, {
										"name":	"provencal"
									}, {
										"name":	"romansch"
									}, {
										"name":	"romanian"
									}, {
										"name":	"spanish"
									}]
							}]
					}, {
						"name":	"slavonic",
						"class":	[{
								"name":	"west slavic",
								"class":	[{
										"name":	"chech"
									}, {
										"name":	"polish"
									}, {
										"name":	"slovak"
									}, {
										"name":	"sorbian"
									}]
							}, {
								"name":	"east slavic",
								"class":	[{
										"name":	"belarussian"
									}, {
										"name":	"russian"
									}, {
										"name":	"ukrainian"
									}]
							}, {
								"name":	"south slavic",
								"class":	[{
										"name":	"bosnian"
									}, {
										"name":	"bulgarian"
									}, {
										"name":	"macedonian"
									}, {
										"name":	"serbo-croatian"
									}, {
										"name":	"slovene"
									}]
							}]
					}, {
						"name":	"albanian"
					}, {
						"name":	"armenian"
					}, {
						"name":	"baltic",
						"class":	[{
								"name":	"lettish"
							}, {
								"name":	"latvian"
							}, {
								"name":	"lithuanian"
							}]
					}, {
						"name":	"tocharian"
					}, {
						"name":	"hittie"
					}]
			}
		}
	}
}
tree:
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

sqlite_sequence:
'tree', '120'

**end

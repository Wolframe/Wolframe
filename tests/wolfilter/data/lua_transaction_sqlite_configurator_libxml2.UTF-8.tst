**
**requires:LUA
**requires:LIBXML2
**requires:SQLITE3
**input
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE test SYSTEM 'test.simpleform'>
<test>
<addCategoryHierarchy>
<node name="category">
	<node name="Computer">
		<node name="Laptop">
			<node name="Laptop 1"></node>
			<node name="Laptop 2"></node>
			<node name="Laptop 3"></node>
		</node>
		<node name="Desktop">
		</node>
		<node name="Server">
		</node>
		<node name="Pad">
		</node>
	</node>
</node>
</addCategoryHierarchy>
</test>**config
--input-filter xml:libxml2 --output-filter xml:libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2  --module ../../src/modules/cmdbind/lua/mod_command_lua --program=transaction_sqlite_configurator.lua --program simpleform.normalize --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../wolfilter/modules/database/sqlite3/mod_db_sqlite3test --database 'identifier=testdb,file=test.db,dumpfile=DBDUMP,inputfile=DBDATA' --program=DBPRG.tdl run

**file:simpleform.normalize
int=number:integer;
uint=number:unsigned;
float=number:float;
**file: DBDATA

-- The categories tree
--
CREATE TABLE Category	(
	ID INTEGER PRIMARY KEY AUTOINCREMENT,
	parent		INT	REFERENCES Category( ID ),
	name		TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL UNIQUE,
	lft		INT	NOT NULL UNIQUE DEFERRABLE CHECK ( lft > 0 ),
	rgt		INT	NOT NULL UNIQUE DEFERRABLE CHECK ( rgt > 1 ),
	CONSTRAINT order_check CHECK ( rgt > lft )
);

-- The features tree
--
CREATE TABLE Feature	(
	ID INTEGER PRIMARY KEY AUTOINCREMENT,
	parent		INT	REFERENCES Feature( ID ),
	name		TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL UNIQUE,
	lft		INT	NOT NULL UNIQUE DEFERRABLE CHECK ( lft > 0 ),
	rgt		INT	NOT NULL UNIQUE DEFERRABLE CHECK ( rgt > 1 ),
	CONSTRAINT order_check CHECK ( rgt > lft )
);

-- The list of manufacturers
--
CREATE TABLE Manufacturer	(
	ID		SERIAL	PRIMARY KEY,
	name		TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL UNIQUE
);

-- The list of components
--
CREATE TABLE Components	(
	ID		SERIAL	PRIMARY KEY,
	code		TEXT	NOT NULL UNIQUE,
	name		TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL UNIQUE,
	category	INT	REFERENCES Category( ID ),
	manufacturerID	INT	REFERENCES Manufacturer( ID ),
	mfgCode		TEXT
);

-- The list of features required by members of a category
--
CREATE TABLE CategRequires	(
	Category	INT	REFERENCES Category( ID ),
	Feature		INT	REFERENCES Feature( ID ),
	MinQuantity	INT,
	MaxQuantity	INT
);

-- The list of features provided by members of a category
--
CREATE TABLE CategProvides	(
	Category	INT	REFERENCES Category( ID ),
	Feature		INT	REFERENCES Feature( ID ),
	MinQuantity	INT,
	MaxQuantity	INT
);

-- The list of checks for members of a category
--
CREATE TABLE CategoryCheck	(
	Category	INT	REFERENCES Category( ID ),
	RuleName	TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL UNIQUE
);

-- The list of features required by a component
--
CREATE TABLE ComponentRequires	(
	Category	INT	REFERENCES Category( ID ),
	Feature		INT	REFERENCES Feature( ID ),
	MinQuantity	INT,
	MaxQuantity	INT
);

-- The list of features provided by a component
--
CREATE TABLE ComponentProvides	(
	Category	INT	REFERENCES Category( ID ),
	Feature		INT	REFERENCES Feature( ID ),
	MinQuantity	INT,
	MaxQuantity	INT
);

-- The list of checks for a component
--
CREATE TABLE ComponentCheck	(
	Category	INT	REFERENCES Category( ID ),
	RuleName	TEXT
);

-- Receipes
--
CREATE TABLE Receipe	(
	ID		SERIAL	PRIMARY KEY,
	name		TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL UNIQUE,
	description	TEXT,
	categoryID	INT	REFERENCES Category( ID ),
	CreationDate	TIMESTAMP,
	Comment		TEXT
);

CREATE TABLE ReceipeContent	(
	receipeID	INT	REFERENCES Receipe( ID ),
	categoryID	INT	REFERENCES Category( ID ),
	MinQuantity	INT,
	MaxQuantity	INT,
	Comment		TEXT
);


-- Configuration
--
CREATE TABLE Configuration	(
	ID		SERIAL	PRIMARY KEY,
	categoryID	INT	REFERENCES Category( ID ),
	name		TEXT,
	description	TEXT,
	comment		TEXT
);

CREATE TABLE ConfigComponent	(
	configID	INT	REFERENCES Configuration( ID ),
	componentID	INT	REFERENCES Components( ID ),
	quantity	INT
);

CREATE TABLE ComposedConfig	(
	configID	INT	REFERENCES Configuration( ID ),
	subConfigID	INT	REFERENCES Configuration( ID ),
	quantity	INT
);
**file:DBPRG.tdl
--
-- addCategoryRoot
--
TRANSACTION addCategoryRoot -- (name, normalizedName)
BEGIN
	DO INSERT INTO Category (ID, parent, name, normalizedName, lft, rgt) VALUES (1, NULL, $(name), $(normalizedName), 1, 2);
END

--
-- addCategory
--
TRANSACTION addCategory -- (parentid, name, normalizedName)
BEGIN
	DO NONEMPTY UNIQUE SELECT rgt FROM Category WHERE ID = $(parentid);
	DO UPDATE Category SET rgt = rgt + 2 WHERE rgt >= $1;
	DO UPDATE Category SET lft = lft + 2 WHERE lft > $1;
	DO INSERT INTO Category (parent, name, normalizedName, lft, rgt) VALUES ($(parentid), $(name), $(normalizedName), $1, $1+1);
	INTO . DO NONEMPTY UNIQUE SELECT ID from Category WHERE lft = $1;
END

--
-- deleteCategory
--
TRANSACTION deleteCategory -- (id)
BEGIN
	DO NONEMPTY SELECT lft,rgt,rgt-lft+1 AS width FROM Category WHERE ID = $(id);
	DO DELETE FROM Category WHERE lft >= $1 AND lft <= $2;
	DO UPDATE Category SET lft = lft-$3 WHERE lft>$2;
	DO UPDATE Category SET rgt = rgt-$3 WHERE rgt>$2;
END

--
-- selectCategory       :Get the category
-- selectCategoryByName :Get the category by name
--
TRANSACTION selectCategory -- (/category/id)
BEGIN
	FOREACH /category INTO . DO NONEMPTY UNIQUE SELECT ID,parent,name,normalizedName FROM Category WHERE ID = $(id);
END
TRANSACTION selectCategoryByName -- (/category/name)
BEGIN
	FOREACH /category INTO . DO NONEMPTY UNIQUE SELECT ID,parent,name,normalizedName FROM Category WHERE name = $(name);
END
TRANSACTION selectCategoryByNormalizedName -- (/category/normalizedName)
BEGIN
	FOREACH /category INTO . DO NONEMPTY UNIQUE SELECT ID,parent,name,normalizedName FROM Category WHERE normalizedName = $(normalizedName);
END

--
-- selectTopCategory       :Get the parents of a category
--
TRANSACTION selectTopCategory -- (/category/id)
BEGIN
	FOREACH /category INTO /category DO SELECT P2.ID,P2.parent,P2.name,P2.normalizedName FROM category AS P1, category AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P1.ID = $(id);
END

--
-- selectSubCategory       :Get the categories
--
TRANSACTION selectSubCategories -- (/category/id)
BEGIN
	FOREACH /category INTO /category DO SELECT P1.ID,P1.parent,P1.name,P1.normalizedName FROM category AS P1, category AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P2.ID = $(id);
END
--
-- addFeatureRoot
--
TRANSACTION addFeatureRoot -- (name, normalizedName)
BEGIN
	DO INSERT INTO Feature (ID, parent, name, normalizedName, lft, rgt) VALUES (1, NULL, $(name), $(normalizedName), 1, 2);
END

--
-- addFeature
--
TRANSACTION addFeature -- (parentid, name, normalizedName)
BEGIN
	DO NONEMPTY UNIQUE SELECT rgt FROM Feature WHERE ID = $(parentid);
	DO UPDATE Feature SET rgt = rgt + 2 WHERE rgt >= $1;
	DO UPDATE Feature SET lft = lft + 2 WHERE lft > $1;
	DO INSERT INTO Feature (parent, name, normalizedName, lft, rgt) VALUES ($(parentid), $(name), $(normalizedName), $1, $1+1);
	INTO . DO NONEMPTY UNIQUE SELECT ID from Feature WHERE lft = $1;
END

--
-- deleteFeature
--
TRANSACTION deleteFeature -- (id)
BEGIN
	DO NONEMPTY SELECT lft,rgt,rgt-lft+1 AS width FROM Feature WHERE ID = $(id);
	DO DELETE FROM Feature WHERE lft >= $1 AND lft <= $2;
	DO UPDATE Feature SET lft = lft-$3 WHERE lft>$2;
	DO UPDATE Feature SET rgt = rgt-$3 WHERE rgt>$2;
END

--
-- selectFeature       :Get the feature
-- selectFeatureByName :Get the feature by name
--
TRANSACTION selectFeature -- (/feature/id)
BEGIN
	FOREACH /feature INTO . DO NONEMPTY UNIQUE SELECT ID,parent,name,normalizedName FROM Feature WHERE ID = $(id);
END
TRANSACTION selectFeatureByName -- (/feature/name)
BEGIN
	FOREACH /feature INTO . DO NONEMPTY UNIQUE SELECT ID,parent,name,normalizedName FROM Feature WHERE name = $(name);
END
TRANSACTION selectFeatureByNormalizedName -- (/feature/normalizedName)
BEGIN
	FOREACH /feature INTO . DO NONEMPTY UNIQUE SELECT ID,parent,name,normalizedName FROM Feature WHERE normalizedName = $(normalizedName);
END

--
-- selectTopFeatures       :Get the parents of a feature
--
TRANSACTION selectTopFeatures -- (/feature/id)
BEGIN
	FOREACH /feature INTO /feature DO SELECT P2.ID,P2.parent,P2.name,P2.normalizedName FROM feature AS P1, feature AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P1.ID = $(id);
END

--
-- selectSubFeatures       :Get the sub features
--
TRANSACTION selectSubFeatures -- (/feature/id)
BEGIN
	FOREACH /feature INTO /feature DO SELECT P1.ID,P1.parent,P1.name,P1.normalizedName FROM feature AS P1, feature AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P2.ID = $(id);
END
**outputfile:DBDUMP
**file: transaction_sqlite_configurator.lua
local function normalizeName( name)
	return name:gsub("[^%s]+", string.lower):gsub("^%s+", ""):gsub("%s+$", ""):gsub("%s+", " ")
end

local function insert_itr( tablename, parentid, itr)
	local id = 1
	local name = ""
	for v,t in itr do
		if (t == "name") then
			local nname = normalizeName( v)
			id = formfunction( "add" .. tablename)( {name=v, normalizedName=nname, parentid=parentid} ):table().ID
		elseif (t == "node") then
			insert_itr( tablename, id, scope( itr))
		end
	end
end

local function insert_topnode( tablename, name, parentid)
	local nname = normalizeName( name)
	if not parentid then
		formfunction( "add" .. tablename .. "Root")( {normalizedName=nname, name=name} )
		return 1
	else
		return formfunction( "add" .. tablename)( {normalizedName=nname, name=name, parentid=parentid} ):table().ID
	end
end

local function insert_tree_topnode( tablename, itr)
	local parentid = nil
	local id = 1
	local name = nil
	for v,t in itr do
		if (t == "parent") then
			parentid = tonumber( v)
		elseif (t == "name") then
			name = v
		elseif (t == "node") then
			if name then
				id = insert_topnode( tablename, name, parentid)
				name = nil
			end
			insert_itr( tablename, id, scope( itr))
		end
	end
	if name then
		insert_topnode( tablename, name, parentid)
	end
end

local function get_tree( tablename, parentid)
	local t = formfunction( "selectSub" .. tablename)( { node = { id=parentid } } ):table()["node"] or {}
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

local function print_tree( tree, nodeid, indent)
	if (indent ~= "") then
		output:print( "\n" .. indent)
	end
	output:opentag( "node")
	output:print( tree[ nodeid].name, "name")
	output:print( nodeid, "id")
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

local function select_tree( tablename, itr)
	filter().empty = false
	output:opentag( "tree")
	for v,t in itr do
		if t == "id" then
			local id = tonumber( v)
			print_tree( get_tree( tablename, id), id, "")
		end
	end
	output:closetag()
end

function selectCategoryHierarchy()
	output:as( "tree 'hierarchyCategory")
	select_tree( "Category")
end

function selectFeatureHierarchy()
	output:as( "tree 'hierarchyFeature")
	select_tree( "Feature")
end

local function add_tree( tablename, itr)
	filter().empty = false
	for v,t in itr do
		if t == "node" then
			insert_tree_topnode( tablename, scope( itr))
		end
	end
end

function addCategoryHierarchy()
	add_tree( "Category", input:get())
end

function addFeatureHierarchy()
	add_tree( "Feature", input:get())
end

function selectCategoryHierarchy()
	select_tree( "Category", input:get())
end

function selectFeatureHierarchy()
	select_tree( "Feature", input:get())
end


function run()
	filter().empty = false
	local itr = input:get()
	for v,t in itr do
		if (t == "addCategoryHierarchy") then
			add_tree( "Category", scope(itr))
		elseif (t == "addFeatureHierarchy") then
			add_tree( "Feature", scope(itr))
		elseif (t == "selectCategoryHierarchy") then
			select_tree( "Category", scope(itr))
		elseif (t == "selectFeatureHierarchy") then
			select_tree( "Feature", scope(itr))
		end
	end
end
**output
Category:
'1', NULL, 'category', 'category', '1', '18'
'2', '1', 'Computer', 'computer', '2', '17'
'3', '2', 'Laptop', 'laptop', '3', '10'
'4', '3', 'Laptop 1', 'laptop 1', '4', '5'
'5', '3', 'Laptop 2', 'laptop 2', '6', '7'
'6', '3', 'Laptop 3', 'laptop 3', '8', '9'
'7', '2', 'Desktop', 'desktop', '11', '12'
'8', '2', 'Server', 'server', '13', '14'
'9', '2', 'Pad', 'pad', '15', '16'

sqlite_sequence:
'Category', '9'

Feature:

Manufacturer:

Components:

CategRequires:

CategProvides:

CategoryCheck:

ComponentRequires:

ComponentProvides:

ComponentCheck:

Receipe:

ReceipeContent:

Configuration:

ConfigComponent:

ComposedConfig:

**end

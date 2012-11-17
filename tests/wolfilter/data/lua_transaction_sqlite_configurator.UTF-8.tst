**
**requires:LUA
**requires:SQLITE3
**input
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE test SYSTEM 'test.simpleform'>
<test>
<pushCategoryHierarchy>
<node name="computer">
	<node name="Minicomputer">
		<node name="Superminicomputer"/>
		<node name="Minicluster"/>
		<node name="Server (Minicomputer)"/>
		<node name="Workstation (Minicomputer)"/>
	</node>
	<node name="Microcomputer">
		<node name="Tower PC"></node>
		<node name="Mid-Tower PC"></node>
		<node name="Mini-Tower PC"></node>
		<node name="Server (Microcomputer)"></node>
		<node name="Workstation (Microcomputer)"></node>
		<node name="Personal computer"></node>
		<node name="Desktop computer"></node>
		<node name="Home computer"></node>
	</node>
	<node name="Mobile">
		<node name="Desknote"></node>
		<node name="Laptop">
			<node name="Notebook">
				<node name="Subnotebook"/>
			</node>
			<node name="Tablet personal computer"></node>
			<node name="slabtop computer">
				<node name="Word-processing keyboard"/>
				<node name="TRS-80 Model 100"/>
			</node>
			<node name="Handheld computer">
				<node name="Ultra-mobile personal computer"/>
				<node name="Personal digital assistant">
					<node name="HandheldPC"/>
					<node name="Palmtop computer"/>
					<node name="Pocket personal computer"/>
				</node>
				<node name="Electronic organizer"/>
				<node name="Pocket computer"/>
				<node name="Calculator">
					<node name="Graphing calculator"/>
					<node name="Scientific calculator"/>
					<node name="Programmable calculator"/>
					<node name="Financial Calculator"/>
				</node>
				<node name="Handheld game console"/>
				<node name="Portable media player"/>
				<node name="Portable data terminal"/>
				<node name="Information appliance">
					<node name="QWERTY Smartphone"/>
					<node name="Smartphone"/>
				</node>
			</node>
			<node name="Wearable computer"/>
		</node>
		<node name="Single board computer"/>
		<node name="Wireless sensor network component"/>
		<node name="Plug computer"/>
		<node name="Microcontroller"/>
		<node name="Smartdust"/>
		<node name="Nanocomputer"/>
	</node>
</node>
</pushCategoryHierarchy>
<deleteCategory><category id="42"/></deleteCategory>
<CategoryHierarchyRequest><category id="1"/></CategoryHierarchyRequest>
<CategoryHierarchyRequest><category id="43"/></CategoryHierarchyRequest>
<CategoryHierarchyRequest><category id="33"/></CategoryHierarchyRequest>
<CategoryHierarchyRequest><category id="16"/></CategoryHierarchyRequest>
<editCategory><category id="46" name="Wireless network component"/></editCategory>
<deleteCategory><category id="22"/></deleteCategory>
<deleteCategory><category id="25"/></deleteCategory>
<createCategory><category id="7" name="Device from outer space" parent="1"/></createCategory>
<CategoryRequest><category id="46"/></CategoryRequest>
<CategoryHierarchyRequest><category id="1"/></CategoryHierarchyRequest>
</test>**config
--input-filter xml:textwolf --output-filter xml:textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf  --module ../../src/modules/cmdbind/lua/mod_command_lua --program=transaction_sqlite_configurator.lua --program simpleform.normalize --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../wolfilter/modules/database/sqlite3/mod_db_sqlite3test --database 'identifier=testdb,file=test.db,dumpfile=DBDUMP,inputfile=DBDATA' --program=DBPRG.tdl run

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
	lft		INT	NOT NULL,
	rgt		INT	NOT NULL,
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
	INTO . DO NONEMPTY UNIQUE SELECT ID from Category WHERE normalizedName = $(normalizedName);
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
-- updateCategory
--
TRANSACTION updateCategory -- (id, name, normalizedName)
BEGIN
	DO UPDATE Category SET name = $(name),normalizedName = $(normalizedName) WHERE ID = $(id);
END

--
-- selectCategory                 :Get the category
-- selectCategoryByName           :Get the category by name
-- selectCategoryByNormalizedName :Get the category by name
-- selectCategoryList             :Get a list of categories
--
TRANSACTION selectCategory -- (id)
BEGIN
	INTO . DO NONEMPTY UNIQUE SELECT name,normalizedName FROM Category WHERE ID = $(id);
END
TRANSACTION selectCategoryByName -- (name)
BEGIN
	INTO . DO NONEMPTY UNIQUE SELECT name,normalizedName FROM Category WHERE name = $(name);
END
TRANSACTION selectCategoryByNormalizedName -- (normalizedName)
BEGIN
	INTO . DO NONEMPTY UNIQUE SELECT name,normalizedName FROM Category WHERE normalizedName = $(normalizedName);
END
TRANSACTION selectCategorySet -- (/category/id)
BEGIN
	FOREACH /category INTO category DO NONEMPTY UNIQUE SELECT ID AS id,name,normalizedName FROM Category WHERE ID = $(id);
END

--
-- selectTopCategory       :Get the parents of a category
--
TRANSACTION selectTopCategory -- (id)
BEGIN
	INTO /node DO SELECT P2.ID,P2.parent,P2.name,P2.normalizedName FROM category AS P1, category AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P1.ID = $(id);
END

--
-- selectSubCategory       :Get the Category
--
TRANSACTION selectSubCategory -- (id)
BEGIN
	INTO /node DO SELECT P1.ID,P1.parent,P1.name,P1.normalizedName FROM category AS P1, category AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P2.ID = $(id);
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
	INTO . DO NONEMPTY UNIQUE SELECT ID from Feature WHERE normalizedName = $(normalizedName);
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
-- updateFeature
--
TRANSACTION updateFeature -- (id, name, normalizedName)
BEGIN
	DO UPDATE Feature SET name = $(name),normalizedName = $(normalizedName) WHERE ID = $(id);
END

--
-- selectFeature                 :Get the feature
-- selectFeatureByName           :Get the feature by name
-- selectFeatureByNormalizedName :Get the feature by name
-- selectFeatureList             :Get a list of features
--
TRANSACTION selectFeature -- (id)
BEGIN
	INTO . DO NONEMPTY UNIQUE SELECT name,normalizedName FROM Feature WHERE ID = $(id);
END
TRANSACTION selectFeatureByName -- (name)
BEGIN
	INTO . DO NONEMPTY UNIQUE SELECT name,normalizedName FROM Feature WHERE name = $(name);
END
TRANSACTION selectFeatureByNormalizedName -- (normalizedName)
BEGIN
	INTO . DO NONEMPTY UNIQUE SELECT name,normalizedName FROM Feature WHERE normalizedName = $(normalizedName);
END
TRANSACTION selectFeatureSet -- (/feature/id)
BEGIN
	FOREACH /feature INTO feature DO NONEMPTY UNIQUE SELECT ID AS id,name,normalizedName FROM Feature WHERE ID = $(id);
END

--
-- selectTopFeatures       :Get the parents of a feature
--
TRANSACTION selectTopFeatures -- (id)
BEGIN
	INTO /node DO SELECT P2.ID,P2.parent,P2.name,P2.normalizedName FROM feature AS P1, feature AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P1.ID = $(id);
END

--
-- selectSubFeatures       :Get the sub features
--
TRANSACTION selectSubFeatures -- (id)
BEGIN
	INTO /node DO SELECT P1.ID,P1.parent,P1.name,P1.normalizedName FROM feature AS P1, feature AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P2.ID = $(id);
END
**outputfile:DBDUMP
**file: transaction_sqlite_configurator.lua
local function normalizeName( name)
	return name:gsub("[^%s]+", string.lower):gsub("[%-()]+", " "):gsub("^%s+", ""):gsub("%s+$", ""):gsub("%s+", " ")
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
	return id
end

local function insert_topnode( tablename, name, parentid)
	local nname = normalizeName( name)
	if not parentid then
		formfunction( "add" .. tablename .. "Root")( {normalizedName=nname, name=name} )
		return 1
	else
		local id = formfunction( "add" .. tablename)( {normalizedName=nname, name=name, parentid=parentid} ):table().ID
		return id
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
	local t = formfunction( "selectSub" .. tablename)( {id=parentid} ):table()["node"] or {}
	local a = {}
	for i,v in pairs( t) do
		table.insert( a, tonumber( v.ID), { name=v.name, parent=tonumber(v.parent), children = {} } )
	end
	for i,v in pairs( a) do
		if i ~= parentid and v.parent then
			table.insert( a[ v.parent ].children, i )
		end
	end
	return a
end

local function print_tree( tree, nodeid, indent)
	if (indent ~= "") then
		output:print( "\n" .. indent)
	end
	output:opentag( "tree" )
	output:opentag( "item" )
	output:print( nodeid, "id")
	output:print( "\n" .. indent )
	output:opentag( "category" )
	output:print( tree[ nodeid ].name )
	-- more attributes of category follow here, like description
	output:closetag( )
	local n = 0
	for i,v in pairs( tree[ nodeid].children) do
		print_tree( tree, v, indent .. "\t")
		n = n + 1
	end
	if n > 0 then
		output:print( "\n" .. indent)
	end
	output:closetag( )
	output:closetag()
end

local function select_tree( tablename, itr)
	filter().empty = false
	for v,t in itr do
		if t == "id" then
			local id = tonumber( v)
			print_tree( get_tree( tablename, id), id, "")
		end
	end
end

local function select_node( tablename, elementname, itr)
	filter().empty = false
	for v,t in itr do
		if t == "id" then
			output:opentag( elementname)
			output:print( v, "id")
			local r = formfunction( "select" .. tablename)( {id=v} )
			output:print( r:get())
			output:closetag()
		end
	end
end

local function edit_node( tablename, itr)
	local name = nil;
	local nname = nil;
	local id = nil;
	for v,t in itr do
		if t == "id" then
			id = v
		elseif t ==  "name" then
			name = v
			nname = normalizeName( name)
		end
	end
	formfunction( "update" .. tablename)( {normalizedName=nname, name=name, id=id} )
end

local function delete_node( tablename, itr)
	local id = nil;
	for v,t in itr do
		if t == "id" then
			id = v
		end
	end
	formfunction( "delete" .. tablename)( {id=id} )
end

local function create_node( tablename, itr)
	local name = nil;
	local parentid = nil;
	for v,t in itr do
		if t == "id" then
			parentid = v
		elseif t == "parent" then
			parentid = v
		elseif t ==  "name" then
			name = v
		end
	end
	insert_topnode( tablename, name, parentid)
end

local function add_tree( tablename, itr)
	filter().empty = false
	for v,t in itr do
		if t == "node" then
			insert_tree_topnode( tablename, scope( itr))
		end
	end
end

function CategoryHierarchyRequest()
	output:as( "node SYSTEM 'CategoryHierarchy.simpleform'")
	select_tree( "Category")
end

function FeatureHierarchyRequest()
	output:as( "node SYSTEM 'FeatureHierarchy.simpleform'")
	select_tree( "Feature")
end

function pushCategoryHierarchy()
	add_tree( "Category", input:get())
end

function pushFeatureHierarchy()
	add_tree( "Feature", input:get())
end

function CategoryRequest()
	output:as( "node SYSTEM 'Category.simpleform'")
	select_node( "Category", "category", input:get())
end

function FeatureRequest()
	output:as( "node SYSTEM 'Feature.simpleform'")
	select_node( "Feature", "feature", input:get())
end

function readCategory()
	print_tree( get_tree( "Category", 1), 1, "")
end

function editCategory()
	edit_node( "Category", input:get())
	output:as( "node SYSTEM 'CategoryHierarchy.simpleform'")
	print_tree( get_tree( "Category", 1), 1, "")
end

function editFeature()
	edit_node( "Feature", input:get())
	output:as( "node SYSTEM 'FeatureHierarchy.simpleform'")
	print_tree( get_tree( "Feature", 1), 1, "")
end

function deleteCategory()
	delete_node( "Category", input:get())
	output:as( "node SYSTEM 'CategoryHierarchy.simpleform'")
	print_tree( get_tree( "Category", 1), 1, "")
end

function deleteFeature()
	delete_node( "Feature", input:get())
	output:as( "node SYSTEM 'FeatureHierarchy.simpleform'")
	print_tree( get_tree( "Feature", 1), 1, "")
end

function createCategory()
	create_node( "Category", input:get())
	output:as( "node SYSTEM 'CategoryHierarchy.simpleform'")
	print_tree( get_tree( "Category", 1), 1, "")
end

function createFeature()
	create_node( "Feature", input:get())
	output:as( "node SYSTEM 'FeatureHierarchy.simpleform'")
	print_tree( get_tree( "Feature", 1), 1, "")
end



function run()
	filter().empty = false
	output:as( "result SYSTEM 'test.simpleform'")
	output:opentag("result")
	local itr = input:get()
	for v,t in itr do
		if (t == "pushCategoryHierarchy") then
			add_tree( "Category", scope(itr))
		elseif (t == "pushFeatureHierarchy") then
			add_tree( "Feature", scope(itr))
		elseif (t == "CategoryHierarchyRequest") then
			select_tree( "Category", scope(itr))
		elseif (t == "FeatureHierarchyRequest") then
			select_tree( "Feature", scope(itr))
		elseif (t == "editCategory") then
			edit_node( "Category", scope(itr))
		elseif (t == "editFeature") then
			edit_node( "Feature", scope(itr))
		elseif (t == "deleteCategory") then
			delete_node( "Category", scope(itr))
		elseif (t == "deleteFeature") then
			delete_node( "Feature", scope(itr))
		elseif (t == "createCategory") then
			create_node( "Category", scope(itr))
		elseif (t == "createFeature") then
			create_node( "Feature", scope(itr))
		elseif (t == "CategoryRequest") then
			select_node( "Category", "category", scope(itr))
		elseif (t == "FeatureRequest") then
			select_node( "Feature", "feature", scope(itr))
		end
	end
	output:closetag()
end
**output
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE result SYSTEM "test.simpleform"><result><tree><item id="1">
<category>computer</category>
	<tree><item id="2">
	<category>Minicomputer</category>
		<tree><item id="3">
		<category>Superminicomputer</category></item></tree>
		<tree><item id="4">
		<category>Minicluster</category></item></tree>
		<tree><item id="5">
		<category>Server (Minicomputer)</category></item></tree>
		<tree><item id="6">
		<category>Workstation (Minicomputer)</category></item></tree>
	</item></tree>
	<tree><item id="7">
	<category>Microcomputer</category>
		<tree><item id="8">
		<category>Tower PC</category></item></tree>
		<tree><item id="9">
		<category>Mid-Tower PC</category></item></tree>
		<tree><item id="10">
		<category>Mini-Tower PC</category></item></tree>
		<tree><item id="11">
		<category>Server (Microcomputer)</category></item></tree>
		<tree><item id="12">
		<category>Workstation (Microcomputer)</category></item></tree>
		<tree><item id="13">
		<category>Personal computer</category></item></tree>
		<tree><item id="14">
		<category>Desktop computer</category></item></tree>
		<tree><item id="15">
		<category>Home computer</category></item></tree>
	</item></tree>
	<tree><item id="16">
	<category>Mobile</category>
		<tree><item id="17">
		<category>Desknote</category></item></tree>
		<tree><item id="18">
		<category>Laptop</category>
			<tree><item id="19">
			<category>Notebook</category>
				<tree><item id="20">
				<category>Subnotebook</category></item></tree>
			</item></tree>
			<tree><item id="21">
			<category>Tablet personal computer</category></item></tree>
			<tree><item id="22">
			<category>slabtop computer</category>
				<tree><item id="23">
				<category>Word-processing keyboard</category></item></tree>
				<tree><item id="24">
				<category>TRS-80 Model 100</category></item></tree>
			</item></tree>
			<tree><item id="25">
			<category>Handheld computer</category>
				<tree><item id="26">
				<category>Ultra-mobile personal computer</category></item></tree>
				<tree><item id="27">
				<category>Personal digital assistant</category>
					<tree><item id="28">
					<category>HandheldPC</category></item></tree>
					<tree><item id="29">
					<category>Palmtop computer</category></item></tree>
					<tree><item id="30">
					<category>Pocket personal computer</category></item></tree>
				</item></tree>
				<tree><item id="31">
				<category>Electronic organizer</category></item></tree>
				<tree><item id="32">
				<category>Pocket computer</category></item></tree>
				<tree><item id="33">
				<category>Calculator</category>
					<tree><item id="34">
					<category>Graphing calculator</category></item></tree>
					<tree><item id="35">
					<category>Scientific calculator</category></item></tree>
					<tree><item id="36">
					<category>Programmable calculator</category></item></tree>
					<tree><item id="37">
					<category>Financial Calculator</category></item></tree>
				</item></tree>
				<tree><item id="38">
				<category>Handheld game console</category></item></tree>
				<tree><item id="39">
				<category>Portable media player</category></item></tree>
				<tree><item id="40">
				<category>Portable data terminal</category></item></tree>
				<tree><item id="41">
				<category>Information appliance</category>
					<tree><item id="43">
					<category>Smartphone</category></item></tree>
				</item></tree>
			</item></tree>
			<tree><item id="44">
			<category>Wearable computer</category></item></tree>
		</item></tree>
		<tree><item id="45">
		<category>Single board computer</category></item></tree>
		<tree><item id="46">
		<category>Wireless sensor network component</category></item></tree>
		<tree><item id="47">
		<category>Plug computer</category></item></tree>
		<tree><item id="48">
		<category>Microcontroller</category></item></tree>
		<tree><item id="49">
		<category>Smartdust</category></item></tree>
		<tree><item id="50">
		<category>Nanocomputer</category></item></tree>
	</item></tree>
</item></tree><tree><item id="43">
<category>Smartphone</category></item></tree><tree><item id="33">
<category>Calculator</category>
	<tree><item id="35">
	<category>Scientific calculator</category></item></tree>
	<tree><item id="36">
	<category>Programmable calculator</category></item></tree>
	<tree><item id="37">
	<category>Financial Calculator</category></item></tree>
	<tree><item id="34">
	<category>Graphing calculator</category></item></tree>
</item></tree><tree><item id="16">
<category>Mobile</category>
	<tree><item id="17">
	<category>Desknote</category></item></tree>
	<tree><item id="18">
	<category>Laptop</category>
		<tree><item id="19">
		<category>Notebook</category>
			<tree><item id="20">
			<category>Subnotebook</category></item></tree>
		</item></tree>
		<tree><item id="21">
		<category>Tablet personal computer</category></item></tree>
		<tree><item id="22">
		<category>slabtop computer</category>
			<tree><item id="23">
			<category>Word-processing keyboard</category></item></tree>
			<tree><item id="24">
			<category>TRS-80 Model 100</category></item></tree>
		</item></tree>
		<tree><item id="25">
		<category>Handheld computer</category>
			<tree><item id="26">
			<category>Ultra-mobile personal computer</category></item></tree>
			<tree><item id="27">
			<category>Personal digital assistant</category>
				<tree><item id="28">
				<category>HandheldPC</category></item></tree>
				<tree><item id="29">
				<category>Palmtop computer</category></item></tree>
				<tree><item id="30">
				<category>Pocket personal computer</category></item></tree>
			</item></tree>
			<tree><item id="31">
			<category>Electronic organizer</category></item></tree>
			<tree><item id="32">
			<category>Pocket computer</category></item></tree>
			<tree><item id="33">
			<category>Calculator</category>
				<tree><item id="34">
				<category>Graphing calculator</category></item></tree>
				<tree><item id="35">
				<category>Scientific calculator</category></item></tree>
				<tree><item id="36">
				<category>Programmable calculator</category></item></tree>
				<tree><item id="37">
				<category>Financial Calculator</category></item></tree>
			</item></tree>
			<tree><item id="38">
			<category>Handheld game console</category></item></tree>
			<tree><item id="39">
			<category>Portable media player</category></item></tree>
			<tree><item id="40">
			<category>Portable data terminal</category></item></tree>
			<tree><item id="41">
			<category>Information appliance</category>
				<tree><item id="43">
				<category>Smartphone</category></item></tree>
			</item></tree>
		</item></tree>
		<tree><item id="44">
		<category>Wearable computer</category></item></tree>
	</item></tree>
	<tree><item id="45">
	<category>Single board computer</category></item></tree>
	<tree><item id="46">
	<category>Wireless sensor network component</category></item></tree>
	<tree><item id="47">
	<category>Plug computer</category></item></tree>
	<tree><item id="48">
	<category>Microcontroller</category></item></tree>
	<tree><item id="49">
	<category>Smartdust</category></item></tree>
	<tree><item id="50">
	<category>Nanocomputer</category></item></tree>
</item></tree><category id="46"><name>Wireless network component</name><normalizedName>wireless network component</normalizedName></category><tree><item id="1">
<category>computer</category>
	<tree><item id="2">
	<category>Minicomputer</category>
		<tree><item id="3">
		<category>Superminicomputer</category></item></tree>
		<tree><item id="4">
		<category>Minicluster</category></item></tree>
		<tree><item id="5">
		<category>Server (Minicomputer)</category></item></tree>
		<tree><item id="6">
		<category>Workstation (Minicomputer)</category></item></tree>
	</item></tree>
	<tree><item id="7">
	<category>Microcomputer</category>
		<tree><item id="8">
		<category>Tower PC</category></item></tree>
		<tree><item id="9">
		<category>Mid-Tower PC</category></item></tree>
		<tree><item id="10">
		<category>Mini-Tower PC</category></item></tree>
		<tree><item id="11">
		<category>Server (Microcomputer)</category></item></tree>
		<tree><item id="12">
		<category>Workstation (Microcomputer)</category></item></tree>
		<tree><item id="13">
		<category>Personal computer</category></item></tree>
		<tree><item id="14">
		<category>Desktop computer</category></item></tree>
		<tree><item id="15">
		<category>Home computer</category></item></tree>
	</item></tree>
	<tree><item id="16">
	<category>Mobile</category>
		<tree><item id="17">
		<category>Desknote</category></item></tree>
		<tree><item id="18">
		<category>Laptop</category>
			<tree><item id="19">
			<category>Notebook</category>
				<tree><item id="20">
				<category>Subnotebook</category></item></tree>
			</item></tree>
			<tree><item id="21">
			<category>Tablet personal computer</category></item></tree>
			<tree><item id="44">
			<category>Wearable computer</category></item></tree>
		</item></tree>
		<tree><item id="49">
		<category>Smartdust</category></item></tree>
		<tree><item id="50">
		<category>Nanocomputer</category></item></tree>
		<tree><item id="45">
		<category>Single board computer</category></item></tree>
		<tree><item id="46">
		<category>Wireless network component</category></item></tree>
		<tree><item id="47">
		<category>Plug computer</category></item></tree>
		<tree><item id="48">
		<category>Microcontroller</category></item></tree>
	</item></tree>
	<tree><item id="51">
	<category>Device from outer space</category></item></tree>
</item></tree></result>
Category:
'1', NULL, 'computer', 'computer', '1', '58'
'2', '1', 'Minicomputer', 'minicomputer', '2', '11'
'3', '2', 'Superminicomputer', 'superminicomputer', '3', '4'
'4', '2', 'Minicluster', 'minicluster', '5', '6'
'5', '2', 'Server (Minicomputer)', 'server minicomputer', '7', '8'
'6', '2', 'Workstation (Minicomputer)', 'workstation minicomputer', '9', '10'
'7', '1', 'Microcomputer', 'microcomputer', '12', '29'
'8', '7', 'Tower PC', 'tower pc', '13', '14'
'9', '7', 'Mid-Tower PC', 'mid tower pc', '15', '16'
'10', '7', 'Mini-Tower PC', 'mini tower pc', '17', '18'
'11', '7', 'Server (Microcomputer)', 'server microcomputer', '19', '20'
'12', '7', 'Workstation (Microcomputer)', 'workstation microcomputer', '21', '22'
'13', '7', 'Personal computer', 'personal computer', '23', '24'
'14', '7', 'Desktop computer', 'desktop computer', '25', '26'
'15', '7', 'Home computer', 'home computer', '27', '28'
'16', '1', 'Mobile', 'mobile', '30', '55'
'17', '16', 'Desknote', 'desknote', '31', '32'
'18', '16', 'Laptop', 'laptop', '33', '42'
'19', '18', 'Notebook', 'notebook', '34', '37'
'20', '19', 'Subnotebook', 'subnotebook', '35', '36'
'21', '18', 'Tablet personal computer', 'tablet personal computer', '38', '39'
'44', '18', 'Wearable computer', 'wearable computer', '40', '41'
'45', '16', 'Single board computer', 'single board computer', '43', '44'
'46', '16', 'Wireless network component', 'wireless network component', '45', '46'
'47', '16', 'Plug computer', 'plug computer', '47', '48'
'48', '16', 'Microcontroller', 'microcontroller', '49', '50'
'49', '16', 'Smartdust', 'smartdust', '51', '52'
'50', '16', 'Nanocomputer', 'nanocomputer', '53', '54'
'51', '1', 'Device from outer space', 'device from outer space', '56', '57'

sqlite_sequence:
'Category', '51'

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

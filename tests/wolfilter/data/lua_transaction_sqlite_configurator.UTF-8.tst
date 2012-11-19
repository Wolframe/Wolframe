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
<createCategory><category name="Device from outer space" parent="7"></category></createCategory>
<createCategory>
<category parent="46"><name>WNC child</name>
<picture><caption>WNC caption</caption><info>WNC info</info><image>WNC image</image></picture>
</category>
</createCategory>
<editCategory>
<category name="Device from outer space" id="51"></category>
</editCategory>
<editCategory>
<category id="52">
	<name>WNC child Y</name>
	<picture id='1'>
		<caption>WNC caption X</caption>
		<info>WNC info X</info>
		<image>WNC image X</image>
	</picture>
</category>
</editCategory>
<CategoryRequest><category id="52"/></CategoryRequest>
<CategoryRequest><category id="46"/></CategoryRequest>
<CategoryHierarchyRequest><category id="1"/></CategoryHierarchyRequest>
<pushFeatureHierarchy>
<node name="feature">
	<node name="Color">
		<node name="green"/>
		<node name="blue"/>
		<node name="gray"/>
		<node name="red"/>
	</node>
	<node name="Size">
		<node name="big"/>
		<node name="small"/>
		<node name="tiny"/>
		<node name="pocket size"/>
	</node>
	<node name="Noise">
		<node name="loud"/>
		<node name="silent"/>
		<node name="grumling"/>
	</node>
</node>
</pushFeatureHierarchy>
<deleteFeature><feature id="15"/></deleteFeature>
<FeatureHierarchyRequest><feature id="1"/></FeatureHierarchyRequest>
<FeatureHierarchyRequest><feature id="7"/></FeatureHierarchyRequest>
<editFeature><feature id="6" name="pink"/></editFeature>
<createFeature><feature name="mumling" parent="12"/></createFeature>
<createFeature><feature name="Space" parent="1"/></createFeature>
<createFeature><feature name="yellow" parent="2"/></createFeature>
<FeatureHierarchyRequest><feature id="1"/></FeatureHierarchyRequest>
</test>**config
--input-filter xml:textwolf --output-filter xml:textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf  --module ../../src/modules/cmdbind/lua/mod_command_lua --program=transaction_sqlite_configurator.lua --program simpleform.normalize --program category.simpleform --program feature.simpleform --module ../../src/modules/ddlcompiler//simpleform/mod_ddlcompiler_simpleform --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../wolfilter/modules/database/sqlite3/mod_db_sqlite3test --database 'identifier=testdb,file=test.db,dumpfile=DBDUMP,inputfile=DBDATA' --program=DBPRG.tdl run

**file:simpleform.normalize
int=number:integer;
uint=number:unsigned;
float=number:float;
**file: DBDATA

-- The list of images used
--
CREATE TABLE Picture	(
	ID		INTEGER	PRIMARY KEY AUTOINCREMENT,
	caption		TEXT,
	info		TEXT,
	image		BYTEA
);

-- The categories tree
--
CREATE TABLE Category	(
	ID		INTEGER	PRIMARY KEY AUTOINCREMENT,
	parent		INT	REFERENCES Category( ID ),
	name		TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL UNIQUE,
	description	TEXT,
	lft		INT	NOT NULL,
	rgt		INT	NOT NULL,
	CONSTRAINT order_check CHECK ( rgt > lft )
);

CREATE TABLE CategoryPicture	(
	categoryID	INT	REFERENCES Category( ID ),
	pictureID	INT	REFERENCES Picture( ID )
);


-- The features tree
--
CREATE TABLE Feature	(
	ID 		INTEGER	PRIMARY KEY AUTOINCREMENT,
	parent		INT	REFERENCES Feature( ID ),
	name		TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL UNIQUE,
	lft		INT	NOT NULL,
	rgt		INT	NOT NULL,
	CONSTRAINT order_check CHECK ( rgt > lft )
);

CREATE TABLE FeaturePicture	(
	featureID	INT	REFERENCES Feature( ID ),
	pictureID	INT	REFERENCES Picture( ID )
);


-- The list of manufacturers
--
CREATE TABLE Manufacturer	(
	ID		INTEGER	PRIMARY KEY AUTOINCREMENT,
	name		TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL UNIQUE,
	webPage		TEXT,
	logo		INT	REFERENCES Picture( ID )
);

-- The list of components
--
CREATE TABLE Component	(
	ID		INTEGER	PRIMARY KEY AUTOINCREMENT,
	code		TEXT	NOT NULL UNIQUE,
	name		TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL UNIQUE,
	category	INT	REFERENCES Category( ID ),
	manufacturerID	INT	REFERENCES Manufacturer( ID ),
	mfgCode		TEXT,
	webPage		TEXT,
	price		NUMERIC( 10, 2 )
);

CREATE TABLE ComponentHistory	(
	componentID	INT	REFERENCES Component( ID ),
	price		NUMERIC( 10, 2 ),
	priceDate	TIMESTAMP,
	username	TEXT
);

CREATE TABLE ComponentPicture	(
	componentID	INT	REFERENCES Component( ID ),
	pictureID	INT	REFERENCES Picture( ID )
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
	ID		INTEGER	PRIMARY KEY AUTOINCREMENT,
	name		TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL UNIQUE,
	description	TEXT,
	categoryID	INT	REFERENCES Category( ID ),
	CreationDate	TIMESTAMP,
	Comment		TEXT
);

CREATE TABLE RecipePicture	(
	receipeID	INT	REFERENCES Receipe( ID ),
	pictureID	INT	REFERENCES Picture( ID )
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
	ID		INTEGER	PRIMARY KEY AUTOINCREMENT,
	categoryID	INT	REFERENCES Category( ID ),
	name		TEXT,
	description	TEXT,
	comment		TEXT
);

CREATE TABLE ConfigComponent	(
	configID	INT	REFERENCES Configuration( ID ),
	componentID	INT	REFERENCES Component( ID ),
	quantity	INT
);

CREATE TABLE ComposedConfig	(
	configID	INT	REFERENCES Configuration( ID ),
	subConfigID	INT	REFERENCES Configuration( ID ),
	quantity	INT
);
**file:category.simpleform
DOCTYPE "picture Picture"
{
	id @int
	caption string
	info string
	image string
}
DOCTYPE "category Category"
{
	id int
	parent int
	name string
	normalizedName string
	description string
	picture Picture[]
}
**file:feature.simpleform
DOCTYPE "feature Feature"
{
	feature
	{
		parentid int
		name string
	}
}
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
OPERATION addCategoryPicture -- (categoryID,caption,info,image)
BEGIN
	DO INSERT INTO CategoryPicture (caption,info,image) VALUES ($(caption), $(info), $(image));
	DO SELECT $1,last_insert_rowid() FROM Picture;
	DO INSERT INTO CategoryPicture (categoryID,pictureID) VALUES ($1,$2);
END

TRANSACTION addCategory -- (parentid, name, normalizedName)
BEGIN
	DO NONEMPTY UNIQUE SELECT rgt FROM Category WHERE ID = $(parentid);
	DO UPDATE Category SET rgt = rgt + 2 WHERE rgt >= $1;
	DO UPDATE Category SET lft = lft + 2 WHERE lft > $1;
	DO INSERT INTO Category (parent, name, normalizedName, description, lft, rgt) VALUES ($(parentid), $(name), $(normalizedName), $(description), $1, $1+1);
	INTO . DO NONEMPTY UNIQUE SELECT ID from Category WHERE normalizedName = $(normalizedName);
	FOREACH picture DO INSERT INTO Picture (caption,info,image) VALUES ($(caption), $(info), $(image));
	FOREACH picture DO SELECT $1,last_insert_rowid() FROM Picture;
	FOREACH picture DO INSERT INTO CategoryPicture (categoryID,pictureID) VALUES ($1,$2);
END

--
-- deleteCategory
--
TRANSACTION deleteCategory -- (id)
BEGIN
	DO NONEMPTY SELECT lft,rgt,rgt-lft+1 AS width FROM Category WHERE ID = $(id);
	DO DELETE FROM Picture WHERE ID IN (SELECT pictureID FROM CategoryPicture WHERE categoryID = $(id));
	DO DELETE FROM CategoryPicture WHERE categoryID = $(id);
	DO DELETE FROM Category WHERE lft >= $1 AND lft <= $2;
	DO UPDATE Category SET lft = lft-$3 WHERE lft>$2;
	DO UPDATE Category SET rgt = rgt-$3 WHERE rgt>$2;
END

--
-- updateCategory
--
TRANSACTION updateCategory -- (id, name, normalizedName, picture/id, picture/caption, picture/info, picture/image)
BEGIN
	DO UPDATE Category SET name = $(name), normalizedName = $(normalizedName), description = $(description) WHERE ID = $(id);
	FOREACH picture DO UPDATE Picture SET caption = $(caption), info = $(info), image = $(image) WHERE ID = $(id);
END

--
-- selectCategory                 :Get the category
-- selectCategoryByName           :Get the category by name
-- selectCategoryByNormalizedName :Get the category by name
-- selectCategoryList             :Get a list of categories
--
TRANSACTION selectCategory -- (id)
BEGIN
	INTO . DO NONEMPTY UNIQUE SELECT ID AS id,parent,name,normalizedName,description FROM Category WHERE ID = $(id);
	INTO /picture DO SELECT CategoryPicture.pictureID AS id,Picture.caption,Picture.info,Picture.image FROM CategoryPicture,Picture WHERE CategoryPicture.pictureID = Picture.ID AND CategoryPicture.categoryID = $(id);
END
TRANSACTION selectCategoryByName -- (name)
BEGIN
	INTO . DO NONEMPTY UNIQUE SELECT ID AS id,parent,name,normalizedName,description FROM Category WHERE name = $(name);
	INTO picture DO SELECT pictureID AS id,caption,info,image FROM CategoryPicture,Category WHERE CategoryPicture.categoryID = Category.ID AND Category.ID = $1;
END
TRANSACTION selectCategoryByNormalizedName -- (normalizedName)
BEGIN
	INTO . DO NONEMPTY UNIQUE SELECT ID AS id,parent,name,normalizedName,description FROM Category WHERE normalizedName = $(normalizedName);
	INTO picture DO SELECT pictureID AS id,caption,info,image FROM CategoryPicture,Category WHERE CategoryPicture.categoryID = Category.ID AND Category.ID = $1;
END
TRANSACTION selectCategorySet -- (/category/id)
BEGIN
	FOREACH /category INTO category DO NONEMPTY UNIQUE SELECT ID AS id,name,normalizedName,description FROM Category WHERE ID = $(id);
END

--
-- selectTopCategory       :Get the parents of a category
--
TRANSACTION selectTopCategory -- (id)
BEGIN
	INTO /node DO SELECT P2.ID,P2.parent,P2.name,P2.normalizedName,P2.description FROM category AS P1, category AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P1.ID = $(id);
END

--
-- selectSubCategory       :Get the Category
--
TRANSACTION selectSubCategory -- (id)
BEGIN
	INTO /node DO SELECT P1.ID,P1.parent,P1.name,P1.normalizedName,P1.description FROM category AS P1, category AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P2.ID = $(id);
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
-- selectTopFeature       :Get the parents of a feature
--
TRANSACTION selectTopFeature -- (id)
BEGIN
	INTO /node DO SELECT P2.ID,P2.parent,P2.name,P2.normalizedName FROM feature AS P1, feature AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P1.ID = $(id);
END

--
-- selectSubFeature       :Get the sub features
--
TRANSACTION selectSubFeature -- (id)
BEGIN
	INTO /node DO SELECT P1.ID,P1.parent,P1.name,P1.normalizedName FROM feature AS P1, feature AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P2.ID = $(id);
END
**outputfile:DBDUMP
**file: transaction_sqlite_configurator.lua
local function normalizeName( name)
	return name:gsub("[^%s]+", string.lower):gsub("[%-()]+", " "):gsub("^%s+", ""):gsub("%s+$", ""):gsub("%s+", " ")
end

local function content_value( v, itr)
	if v then
		return v
	end
	for v,t in itr do
		if t then
			return nil
		end
		if v then
			return v
		end
		return nil
	end
end

local function picture_value( itr)
	local picture = {}
	for v,t in itr do
		if (t == "id" or t == "caption" or t == "info" or t == "image") then
			picture[ t] = content_value( v, itr)
		end
	end
	return picture
end

local function insert_itr( tablename, parentid, itr)
	local id = 1
	local name = nil
	local nname = nil
	local description = nil
	local picture = nil
	for v,t in itr do
		if (t == "name") then
			name = content_value( v, itr)
			nname = normalizeName( name)
		elseif (t == "description") then
			description = content_value( v, itr)
		elseif (t == "picture") then
			picture = picture_value( scope( itr))
		elseif (t == "node") then
			if name then
				id = formfunction( "add" .. tablename)( {name=name, normalizedName=nname, description=description, parentid=parentid, picture=picture} ):table().ID
				name = nil
				description = nil
			end
			insert_itr( tablename, id, scope( itr))
		end
	end
	if name then
		id = formfunction( "add" .. tablename)( {name=name, normalizedName=nname, description=description, parentid=parentid} ):table().ID
	end
	return id
end

local function insert_topnode( tablename, name, description, picture, parentid)
	local nname = normalizeName( name)
	if not parentid then
		formfunction( "add" .. tablename .. "Root")( {normalizedName=nname, name=name, description=description, picture=picture} )
		return 1
	else
		local id = formfunction( "add" .. tablename)( {normalizedName=nname, name=name, description=description, parentid=parentid, picture=picture} ):table().ID
		return id
	end
end

local function insert_tree_topnode( tablename, itr)
	local parentid = nil
	local id = 1
	local name = nil
	local description = nil
	local picture = nil
	for v,t in itr do
		if (t == "parent") then
			parentid = tonumber( v)
		elseif (t == "name") then
			name = content_value( v, itr)
		elseif (t == "description") then
			description = content_value( v, itr)
		elseif (t == "picture") then
			picture = picture_value( scope( itr))
		elseif (t == "node") then
			if name then
				id = insert_topnode( tablename, name, description, picture, parentid)
				name = nil
				description = nil
			end
			insert_itr( tablename, id, scope( itr))
		end
	end
	if name then
		insert_topnode( tablename, name, description, picture, parentid)
	end
end

local function get_tree( tablename, parentid)
	local t = formfunction( "selectSub" .. tablename)( {id=parentid} ):table()["node"] or {}
	local a = {}
	for i,v in pairs( t) do
		table.insert( a, tonumber( v.ID), { name=v.name, description=v.description, picture=v.picture, parent=tonumber(v.parent), children = {} } )
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
	output:print( "\n" .. indent .. "\t")
	output:opentag( "category" )
	output:print( tree[ nodeid ].name )
	output:closetag( )
	if tree[ nodeid ].description then
		output:print( "\n" .. indent .. "\t")
		output:opentag( "description" )
		output:print( tree[ nodeid ].description )
		output:closetag( )
	end
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
			local f = form( "Category");
			f:fill( r:get())
			output:print( f:get())
			output:closetag( )
		end
	end
end

local function edit_node( tablename, itr)
	local name = nil;
	local nname = nil;
	local description = nil;
	local picture = nil;
	local id = nil;
	for v,t in itr do
		if t == "id" then
			id = v
		elseif t ==  "name" then
			name = content_value( v, itr)
			nname = normalizeName( name)
		elseif t == "description" then
			description = content_value( v, itr)
		elseif t == "picture" then
			picture = picture_value( scope(itr))
			logger.printc( "PICTURE ", picture)
		end
	end
	logger.printc( "CALL update" .. tablename, {normalizedName=nname, name=name, description=description, id=id, picture=picture} )
	formfunction( "update" .. tablename)( {normalizedName=nname, name=name, description=description, id=id, picture=picture} )
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
	local description = nil;
	local picture = nil;
	for v,t in itr do
		if t == "parent" then
			parentid = v
		elseif t ==  "name" then
			name = content_value( v, itr)
			nname = normalizeName( name)
		elseif t ==  "description" then
			description = content_value( v, itr)
		elseif t ==  "picture" then
			picture = picture_value( scope(itr))
		end
	end
	insert_topnode( tablename, name, description, picture, parentid)
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
	select_tree( "Category", input:get())
end

function FeatureHierarchyRequest()
	output:as( "node SYSTEM 'FeatureHierarchy.simpleform'")
	select_tree( "Feature", input:get())
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
</item></tree><category id="52"><id>52</id><parent>46</parent><name>WNC child</name><normalizedName>wnc child</normalizedName><description></description><picture id="1"><caption>WNC caption</caption><info>WNC info</info><image>WNC image</image></picture></category><category id="46"><id>46</id><parent>16</parent><name>Wireless network component</name><normalizedName>wireless network component</normalizedName><description></description></category><tree><item id="1">
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
		<tree><item id="51">
			<category>Device from outer space</category></item></tree>
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
		<tree><item id="45">
			<category>Single board computer</category></item></tree>
		<tree><item id="47">
			<category>Plug computer</category></item></tree>
		<tree><item id="49">
			<category>Smartdust</category></item></tree>
		<tree><item id="46">
			<category>Wireless network component</category>
			<tree><item id="52">
				<category>WNC child</category></item></tree>
		</item></tree>
		<tree><item id="48">
			<category>Microcontroller</category></item></tree>
		<tree><item id="50">
			<category>Nanocomputer</category></item></tree>
	</item></tree>
</item></tree><tree><item id="1">
	<category>feature</category>
	<tree><item id="2">
		<category>Color</category>
		<tree><item id="3">
			<category>green</category></item></tree>
		<tree><item id="4">
			<category>blue</category></item></tree>
		<tree><item id="5">
			<category>gray</category></item></tree>
		<tree><item id="6">
			<category>red</category></item></tree>
	</item></tree>
	<tree><item id="7">
		<category>Size</category>
		<tree><item id="8">
			<category>big</category></item></tree>
		<tree><item id="9">
			<category>small</category></item></tree>
		<tree><item id="10">
			<category>tiny</category></item></tree>
		<tree><item id="11">
			<category>pocket size</category></item></tree>
	</item></tree>
	<tree><item id="12">
		<category>Noise</category>
		<tree><item id="13">
			<category>loud</category></item></tree>
		<tree><item id="14">
			<category>silent</category></item></tree>
	</item></tree>
</item></tree><tree><item id="7">
	<category>Size</category>
	<tree><item id="10">
		<category>tiny</category></item></tree>
	<tree><item id="9">
		<category>small</category></item></tree>
	<tree><item id="11">
		<category>pocket size</category></item></tree>
	<tree><item id="8">
		<category>big</category></item></tree>
</item></tree><tree><item id="1">
	<category>feature</category>
	<tree><item id="2">
		<category>Color</category>
		<tree><item id="3">
			<category>green</category></item></tree>
		<tree><item id="4">
			<category>blue</category></item></tree>
		<tree><item id="5">
			<category>gray</category></item></tree>
		<tree><item id="6">
			<category>pink</category></item></tree>
		<tree><item id="18">
			<category>yellow</category></item></tree>
	</item></tree>
	<tree><item id="7">
		<category>Size</category>
		<tree><item id="8">
			<category>big</category></item></tree>
		<tree><item id="9">
			<category>small</category></item></tree>
		<tree><item id="10">
			<category>tiny</category></item></tree>
		<tree><item id="11">
			<category>pocket size</category></item></tree>
	</item></tree>
	<tree><item id="12">
		<category>Noise</category>
		<tree><item id="13">
			<category>loud</category></item></tree>
		<tree><item id="14">
			<category>silent</category></item></tree>
		<tree><item id="16">
			<category>mumling</category></item></tree>
	</item></tree>
	<tree><item id="17">
		<category>Space</category></item></tree>
</item></tree></result>
Picture:
'1', 'WNC caption', 'WNC info', 'WNC image'

sqlite_sequence:
'Category', '52'
'Picture', '1'
'Feature', '18'

Category:
'1', NULL, 'computer', 'computer', NULL, '1', '60'
'2', '1', 'Minicomputer', 'minicomputer', NULL, '2', '11'
'3', '2', 'Superminicomputer', 'superminicomputer', NULL, '3', '4'
'4', '2', 'Minicluster', 'minicluster', NULL, '5', '6'
'5', '2', 'Server (Minicomputer)', 'server minicomputer', NULL, '7', '8'
'6', '2', 'Workstation (Minicomputer)', 'workstation minicomputer', NULL, '9', '10'
'7', '1', 'Microcomputer', 'microcomputer', NULL, '12', '31'
'8', '7', 'Tower PC', 'tower pc', NULL, '13', '14'
'9', '7', 'Mid-Tower PC', 'mid tower pc', NULL, '15', '16'
'10', '7', 'Mini-Tower PC', 'mini tower pc', NULL, '17', '18'
'11', '7', 'Server (Microcomputer)', 'server microcomputer', NULL, '19', '20'
'12', '7', 'Workstation (Microcomputer)', 'workstation microcomputer', NULL, '21', '22'
'13', '7', 'Personal computer', 'personal computer', NULL, '23', '24'
'14', '7', 'Desktop computer', 'desktop computer', NULL, '25', '26'
'15', '7', 'Home computer', 'home computer', NULL, '27', '28'
'16', '1', 'Mobile', 'mobile', NULL, '32', '59'
'17', '16', 'Desknote', 'desknote', NULL, '33', '34'
'18', '16', 'Laptop', 'laptop', NULL, '35', '44'
'19', '18', 'Notebook', 'notebook', NULL, '36', '39'
'20', '19', 'Subnotebook', 'subnotebook', NULL, '37', '38'
'21', '18', 'Tablet personal computer', 'tablet personal computer', NULL, '40', '41'
'44', '18', 'Wearable computer', 'wearable computer', NULL, '42', '43'
'45', '16', 'Single board computer', 'single board computer', NULL, '45', '46'
'46', '16', 'Wireless network component', 'wireless network component', NULL, '47', '50'
'47', '16', 'Plug computer', 'plug computer', NULL, '51', '52'
'48', '16', 'Microcontroller', 'microcontroller', NULL, '53', '54'
'49', '16', 'Smartdust', 'smartdust', NULL, '55', '56'
'50', '16', 'Nanocomputer', 'nanocomputer', NULL, '57', '58'
'51', '7', 'Device from outer space', 'device from outer space', NULL, '29', '30'
'52', '46', 'WNC child', 'wnc child', NULL, '48', '49'

CategoryPicture:
'52', '1'

Feature:
'1', NULL, 'feature', 'feature', '1', '34'
'2', '1', 'Color', 'color', '2', '13'
'3', '2', 'green', 'green', '3', '4'
'4', '2', 'blue', 'blue', '5', '6'
'5', '2', 'gray', 'gray', '7', '8'
'6', '2', 'pink', 'pink', '9', '10'
'7', '1', 'Size', 'size', '14', '23'
'8', '7', 'big', 'big', '15', '16'
'9', '7', 'small', 'small', '17', '18'
'10', '7', 'tiny', 'tiny', '19', '20'
'11', '7', 'pocket size', 'pocket size', '21', '22'
'12', '1', 'Noise', 'noise', '24', '31'
'13', '12', 'loud', 'loud', '25', '26'
'14', '12', 'silent', 'silent', '27', '28'
'16', '12', 'mumling', 'mumling', '29', '30'
'17', '1', 'Space', 'space', '32', '33'
'18', '2', 'yellow', 'yellow', '11', '12'

FeaturePicture:

Manufacturer:

Component:

ComponentHistory:

ComponentPicture:

CategRequires:

CategProvides:

CategoryCheck:

ComponentRequires:

ComponentProvides:

ComponentCheck:

Receipe:

RecipePicture:

ReceipeContent:

Configuration:

ConfigComponent:

ComposedConfig:

**end

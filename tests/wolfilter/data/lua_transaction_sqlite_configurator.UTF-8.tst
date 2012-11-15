**
**requires:LUA
**requires:SQLITE3
**input
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE test SYSTEM 'test.simpleform'>
<test>
<addCategoryHierarchy>
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
</addCategoryHierarchy>
<selectCategoryHierarchy><category id="1"/></selectCategoryHierarchy>
<selectCategoryHierarchy><category id="43"/></selectCategoryHierarchy>
<selectCategoryHierarchy><category id="33"/></selectCategoryHierarchy>
<selectCategoryHierarchy><category id="16"/></selectCategoryHierarchy>
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
	for v,t in itr do
		if t == "id" then
			local id = tonumber( v)
			print_tree( get_tree( tablename, id), id, "")
		end
	end
end

function selectCategoryHierarchy()
	output:as( "node SYSTEM 'hierarchyCategory.simpleform'")
	select_tree( "Category")
end

function selectFeatureHierarchy()
	output:as( "node SYSTEM 'hierarchyFeature.simpleform'")
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
	output:as( "result SYSTEM 'test.simpleform'")
	output:opentag("result")
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
	output:closetag()
end
**output
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE result SYSTEM "test.simpleform"><result><node name="computer" id="1">
	<node name="Minicomputer" id="2">
		<node name="Superminicomputer" id="3"/>
		<node name="Minicluster" id="4"/>
		<node name="Server (Minicomputer)" id="5"/>
		<node name="Workstation (Minicomputer)" id="6"/>
	</node>
	<node name="Microcomputer" id="7">
		<node name="Tower PC" id="8"/>
		<node name="Mid-Tower PC" id="9"/>
		<node name="Mini-Tower PC" id="10"/>
		<node name="Server (Microcomputer)" id="11"/>
		<node name="Workstation (Microcomputer)" id="12"/>
		<node name="Personal computer" id="13"/>
		<node name="Desktop computer" id="14"/>
		<node name="Home computer" id="15"/>
	</node>
	<node name="Mobile" id="16">
		<node name="Desknote" id="17"/>
		<node name="Laptop" id="18">
			<node name="Notebook" id="19">
				<node name="Subnotebook" id="20"/>
			</node>
			<node name="Tablet personal computer" id="21"/>
			<node name="slabtop computer" id="22">
				<node name="Word-processing keyboard" id="23"/>
				<node name="TRS-80 Model 100" id="24"/>
			</node>
			<node name="Handheld computer" id="25">
				<node name="Ultra-mobile personal computer" id="26"/>
				<node name="Personal digital assistant" id="27">
					<node name="HandheldPC" id="28"/>
					<node name="Palmtop computer" id="29"/>
					<node name="Pocket personal computer" id="30"/>
				</node>
				<node name="Electronic organizer" id="31"/>
				<node name="Pocket computer" id="32"/>
				<node name="Calculator" id="33">
					<node name="Graphing calculator" id="34"/>
					<node name="Scientific calculator" id="35"/>
					<node name="Programmable calculator" id="36"/>
					<node name="Financial Calculator" id="37"/>
				</node>
				<node name="Handheld game console" id="38"/>
				<node name="Portable media player" id="39"/>
				<node name="Portable data terminal" id="40"/>
				<node name="Information appliance" id="41">
					<node name="QWERTY Smartphone" id="42"/>
					<node name="Smartphone" id="43"/>
				</node>
			</node>
			<node name="Wearable computer" id="44"/>
		</node>
		<node name="Single board computer" id="45"/>
		<node name="Wireless sensor network component" id="46"/>
		<node name="Plug computer" id="47"/>
		<node name="Microcontroller" id="48"/>
		<node name="Smartdust" id="49"/>
		<node name="Nanocomputer" id="50"/>
	</node>
</node><node name="Smartphone" id="43"/><node name="Calculator" id="33">
	<node name="Scientific calculator" id="35"/>
	<node name="Programmable calculator" id="36"/>
	<node name="Financial Calculator" id="37"/>
	<node name="Graphing calculator" id="34"/>
</node><node name="Mobile" id="16">
	<node name="Desknote" id="17"/>
	<node name="Laptop" id="18">
		<node name="Notebook" id="19">
			<node name="Subnotebook" id="20"/>
		</node>
		<node name="Tablet personal computer" id="21"/>
		<node name="slabtop computer" id="22">
			<node name="Word-processing keyboard" id="23"/>
			<node name="TRS-80 Model 100" id="24"/>
		</node>
		<node name="Handheld computer" id="25">
			<node name="Ultra-mobile personal computer" id="26"/>
			<node name="Personal digital assistant" id="27">
				<node name="HandheldPC" id="28"/>
				<node name="Palmtop computer" id="29"/>
				<node name="Pocket personal computer" id="30"/>
			</node>
			<node name="Electronic organizer" id="31"/>
			<node name="Pocket computer" id="32"/>
			<node name="Calculator" id="33">
				<node name="Graphing calculator" id="34"/>
				<node name="Scientific calculator" id="35"/>
				<node name="Programmable calculator" id="36"/>
				<node name="Financial Calculator" id="37"/>
			</node>
			<node name="Handheld game console" id="38"/>
			<node name="Portable media player" id="39"/>
			<node name="Portable data terminal" id="40"/>
			<node name="Information appliance" id="41">
				<node name="QWERTY Smartphone" id="42"/>
				<node name="Smartphone" id="43"/>
			</node>
		</node>
		<node name="Wearable computer" id="44"/>
	</node>
	<node name="Single board computer" id="45"/>
	<node name="Wireless sensor network component" id="46"/>
	<node name="Plug computer" id="47"/>
	<node name="Microcontroller" id="48"/>
	<node name="Smartdust" id="49"/>
	<node name="Nanocomputer" id="50"/>
</node></result>
Category:
'1', NULL, 'computer', 'computer', '1', '100'
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
'16', '1', 'Mobile', 'mobile', '30', '99'
'17', '16', 'Desknote', 'desknote', '31', '32'
'18', '16', 'Laptop', 'laptop', '33', '86'
'19', '18', 'Notebook', 'notebook', '34', '37'
'20', '19', 'Subnotebook', 'subnotebook', '35', '36'
'21', '18', 'Tablet personal computer', 'tablet personal computer', '38', '39'
'22', '18', 'slabtop computer', 'slabtop computer', '40', '45'
'23', '22', 'Word-processing keyboard', 'word processing keyboard', '41', '42'
'24', '22', 'TRS-80 Model 100', 'trs 80 model 100', '43', '44'
'25', '18', 'Handheld computer', 'handheld computer', '46', '83'
'26', '25', 'Ultra-mobile personal computer', 'ultra mobile personal computer', '47', '48'
'27', '25', 'Personal digital assistant', 'personal digital assistant', '49', '56'
'28', '27', 'HandheldPC', 'handheldpc', '50', '51'
'29', '27', 'Palmtop computer', 'palmtop computer', '52', '53'
'30', '27', 'Pocket personal computer', 'pocket personal computer', '54', '55'
'31', '25', 'Electronic organizer', 'electronic organizer', '57', '58'
'32', '25', 'Pocket computer', 'pocket computer', '59', '60'
'33', '25', 'Calculator', 'calculator', '61', '70'
'34', '33', 'Graphing calculator', 'graphing calculator', '62', '63'
'35', '33', 'Scientific calculator', 'scientific calculator', '64', '65'
'36', '33', 'Programmable calculator', 'programmable calculator', '66', '67'
'37', '33', 'Financial Calculator', 'financial calculator', '68', '69'
'38', '25', 'Handheld game console', 'handheld game console', '71', '72'
'39', '25', 'Portable media player', 'portable media player', '73', '74'
'40', '25', 'Portable data terminal', 'portable data terminal', '75', '76'
'41', '25', 'Information appliance', 'information appliance', '77', '82'
'42', '41', 'QWERTY Smartphone', 'qwerty smartphone', '78', '79'
'43', '41', 'Smartphone', 'smartphone', '80', '81'
'44', '18', 'Wearable computer', 'wearable computer', '84', '85'
'45', '16', 'Single board computer', 'single board computer', '87', '88'
'46', '16', 'Wireless sensor network component', 'wireless sensor network component', '89', '90'
'47', '16', 'Plug computer', 'plug computer', '91', '92'
'48', '16', 'Microcontroller', 'microcontroller', '93', '94'
'49', '16', 'Smartdust', 'smartdust', '95', '96'
'50', '16', 'Nanocomputer', 'nanocomputer', '97', '98'

sqlite_sequence:
'Category', '50'

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

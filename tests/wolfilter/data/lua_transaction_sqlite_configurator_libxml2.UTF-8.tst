**
**requires:LUA
**requires:LIBXML2
**requires:SQLITE3
**input
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE test SYSTEM 'test.simpleform'>
<test>
<pushCategoryHierarchy>
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
</pushCategoryHierarchy>
<deleteCategory><category id="42"/></deleteCategory>
<CategoryHierarchyRequest><category id="1"/></CategoryHierarchyRequest>
<CategoryHierarchyRequest><category id="43"/></CategoryHierarchyRequest>
<CategoryHierarchyRequest><category id="33"/></CategoryHierarchyRequest>
<CategoryHierarchyRequest><category id="16"/></CategoryHierarchyRequest>
<editCategory><category id="46" name="Wireless network component"/></editCategory>
<deleteCategory><category id="22"/></deleteCategory>
<deleteCategory><category id="25"/></deleteCategory>
<createCategory><category name="Device from outer space" parentID="7"></category></createCategory>
<createPicture>
<picture>
	<caption>WNC caption</caption><info>WNC info</info><image>WNC image</image>
</picture>
</createPicture>
<createCategory><category parentID="46"><name>WNC child</name><pictures><picture id="1"/></pictures></category></createCategory>
<createCategory><category parentID="46"><name>WNC child 2</name><pictures><picture id="1"/></pictures></category></createCategory>
<createCategory><category parentID="46"><name>WNC child 3</name><pictures><picture id="1"/></pictures></category></createCategory>
<editCategory><category name="Device from outer space" id="51"></category></editCategory>
<CategoryRequest><category id="52"/></CategoryRequest>
<createPicture>
<picture>
	<caption>WNC caption X</caption>
	<info>WNC info X</info>
	<image>WNC image X</image>
</picture>
</createPicture>
<createPicture>
<picture>
	<caption>WNC caption Z</caption>
	<info>WNC info Z</info>
	<image>WNC image Z</image>
</picture>
</createPicture>
<editCategory>
<category id="52">
	<name>WNC child Y</name>
	<pictures><picture id='2'/></pictures>
</category>
</editCategory>
<editPicture>
<picture id="1">
	<caption>WNC caption X</caption>
	<info>WNC info X</info>
	<image>WNC image X</image>
</picture>
</editPicture>
<editCategory>
<category id="52">
	<name>WNC child Y</name>
	<pictures><picture id='3'/></pictures>
</category>
</editCategory>
<CategoryRequest><category id="52"/></CategoryRequest>
<CategoryRequest><category id="54"/></CategoryRequest>
<CategoryRequest><category id="46"/></CategoryRequest>
<CategoryHierarchyRequest><category id="1"/></CategoryHierarchyRequest>
<pushFeatureHierarchy>
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
</pushFeatureHierarchy>
<deleteFeature><feature id="15"/></deleteFeature>
<FeatureHierarchyRequest><feature id="1"/></FeatureHierarchyRequest>
<FeatureHierarchyRequest><feature id="7"/></FeatureHierarchyRequest>
<editFeature><feature id="6" name="pink"/></editFeature>
<createFeature><feature name="mumling" parentID="12"/></createFeature>
<createFeature><feature name="Space" parentID="1"/></createFeature>
<createFeature><feature name="yellow" parentID="2"/></createFeature>
<FeatureHierarchyRequest><feature id="1"/></FeatureHierarchyRequest>
<pushTagHierarchy>
	<node name="Tag1">
		<node name="Tag1.1"/>
		<node name="Tag1.2"/>
		<node name="Tag1.3"/>
		<node name="Tag1.4"/>
	</node>
	<node name="Tag2">
		<node name="Tag2.1"/>
		<node name="Tag2.2"/>
		<node name="Tag2.3"/>
		<node name="Tag2.4"/>
	</node>
	<node name="Tag3">
		<node name="Tag3.1"/>
		<node name="Tag3.2"/>
		<node name="Tag3.3"/>
		<node name="Tag3.4"/>
	</node>
</pushTagHierarchy>
<deleteTag><tag id="10"/></deleteTag>
<TagHierarchyRequest><tag id="1"/></TagHierarchyRequest>
<TagHierarchyRequest><tag id="7"/></TagHierarchyRequest>
<editTag><tag id="6" name="Tag1.4x"/></editTag>
<createTag><tag name="Tag3.5x" parentID="12"/></createTag>
<createTag><tag name="Tag5" parentID="1"/></createTag>
<createTag><tag name="Tag1.5x" parentID="2"/></createTag>
<TagHierarchyRequest><Tag id="1"/></TagHierarchyRequest>
</test>**config
--input-filter xml:libxml2 --output-filter xml:libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2  --module ../../src/modules/cmdbind/lua/mod_command_lua --program=transaction_configurator.lua --program configurator.normalize --program category.simpleform --program feature.simpleform --program manufacturer.simpleform --program picture.simpleform --program tag.simpleform --module ../../src/modules/ddlcompiler//simpleform/mod_ddlcompiler_simpleform --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/normalize//string/mod_normalize_string --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../wolfilter/modules/functions/fakegraphix/mod_graphix --module ../wolfilter/modules/database/sqlite3/mod_db_sqlite3test --database 'identifier=testdb,file=test.db,dumpfile=DBDUMP,inputfile=DBDATA,program=program.sql' --program=DBPRG.tdl run

**file: DBDATA
-- The tags tree
--
CREATE TABLE Tag	(
	ID		INTEGER	PRIMARY KEY AUTOINCREMENT,
	parentID	INT	REFERENCES Tag( ID ),
	name		TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL,
	description	TEXT,
	lft		INT	NOT NULL,
	rgt		INT	NOT NULL,
	CONSTRAINT order_check CHECK ( rgt > lft ),
	CONSTRAINT tag_normalizedName_check UNIQUE( normalizedName, parentID )
);

INSERT INTO Tag( parentID, name, normalizedName, description, lft, rgt )
	VALUES ( NULL, '_ROOT_', '_ROOT_', 'Tags tree root', 1, 2 );


-- The list of images used
--
CREATE TABLE Picture	(
	ID		INTEGER	PRIMARY KEY AUTOINCREMENT,
	caption		TEXT,
	info		TEXT,
	width		INT,
	height		INT,
	image		TEXT,
	thumbnail	TEXT
);

CREATE TABLE PictureTag	(
	pictureID	INT	REFERENCES Picture( ID ),
	tagID		INT	REFERENCES Tag( ID ),
	UNIQUE ( pictureID, tagID )
);


-- The categories tree
--
CREATE TABLE Category	(
	ID		INTEGER	PRIMARY KEY AUTOINCREMENT,
	parentID	INT	REFERENCES Category( ID ),
	name		TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL,
	description	TEXT,
	lft		INT	NOT NULL,
	rgt		INT	NOT NULL,
	CONSTRAINT order_check CHECK ( rgt > lft ),
	CONSTRAINT category_normalizedName_check UNIQUE( normalizedName, parentID )
);

INSERT INTO Category( parentID, name, normalizedName, description, lft, rgt )
	VALUES ( NULL, '_ROOT_', '_ROOT_', 'Categories tree root', 1, 2 );

CREATE TABLE CategoryPicture	(
	categoryID	INT	REFERENCES Category( ID ),
	pictureID	INT	REFERENCES Picture( ID ),
	UNIQUE ( categoryID, pictureID )
);


-- The features tree
--
CREATE TABLE Feature	(
	ID 		INTEGER	PRIMARY KEY AUTOINCREMENT,
	parentID	INT	REFERENCES Feature( ID ),
	name		TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL,
	description	TEXT,
	lft		INT	NOT NULL,
	rgt		INT	NOT NULL,
	CONSTRAINT order_check CHECK ( rgt > lft ),
	CONSTRAINT feature_normalizedName_check UNIQUE( normalizedName, parentID )
);

INSERT INTO Feature( parentID, name, normalizedName, description, lft, rgt )
	VALUES ( NULL, '_ROOT_', '_ROOT_', 'Features tree root', 1, 2 );

CREATE TABLE FeaturePicture	(
	featureID	INT	REFERENCES Feature( ID ),
	pictureID	INT	REFERENCES Picture( ID ),
	UNIQUE ( featureID, pictureID )
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
	categoryID	INT	REFERENCES Category( ID ),
	manufacturerID	INT	REFERENCES Manufacturer( ID ),
	mfgCode		TEXT,
	webPage		TEXT,
	description	TEXT,
	price		NUMERIC( 10, 2 )
);

CREATE TABLE ComponentPriceHistory	(
	componentID	INT	REFERENCES Component( ID ),
	price		NUMERIC( 10, 2 ),
	priceDate	TIMESTAMP,
	username	TEXT
);

CREATE TABLE ComponentPicture	(
	componentID	INT	REFERENCES Component( ID ),
	pictureID	INT	REFERENCES Picture( ID ),
	UNIQUE ( componentID, pictureID )
);

-- The list of features required by members of a category
--
CREATE TABLE CategRequires	(
	categoryID	INT	REFERENCES Category( ID ),
	featureID	INT	REFERENCES Feature( ID ),
	minQuantity	INT,
	maxQuantity	INT,
	UNIQUE ( categoryID, featureID )
);

-- The list of features provided by members of a category
--
CREATE TABLE CategProvides	(
	categoryID	INT	REFERENCES Category( ID ),
	featureID	INT	REFERENCES Feature( ID ),
	minQuantity	INT,
	maxQuantity	INT,
	UNIQUE ( categoryID, featureID )
);

-- The list of checks for members of a category
--
CREATE TABLE CategoryCheck	(
	categoryID	INT	REFERENCES Category( ID ),
	ruleName	TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL UNIQUE
);

-- The list of features required by a component
--
CREATE TABLE ComponentRequires	(
	componentID	INT	REFERENCES Component( ID ),
	featureID	INT	REFERENCES Feature( ID ),
	minQuantity	INT,
	maxQuantity	INT,
	UNIQUE ( componentID, featureID )
);

-- The list of features provided by a component
--
CREATE TABLE ComponentProvides	(
	componentID	INT	REFERENCES Component( ID ),
	featureID	INT	REFERENCES Feature( ID ),
	minQuantity	INT,
	maxQuantity	INT,
	UNIQUE ( componentID, featureID )
);

-- The list of checks for a component
--
CREATE TABLE ComponentCheck	(
	categoryID	INT	REFERENCES Category( ID ),
	ruleName	TEXT
);

-- Recipes
--
CREATE TABLE Recipe	(
	ID		INTEGER	PRIMARY KEY AUTOINCREMENT,
	name		TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL UNIQUE,
	description	TEXT,
	categoryID	INT	REFERENCES Category( ID ),
	creationDate	TIMESTAMP,
	comment		TEXT
);

CREATE TABLE RecipePicture	(
	recipeID	INT	REFERENCES Recipe( ID ),
	pictureID	INT	REFERENCES Picture( ID ),
	UNIQUE ( recipeID, pictureID )
);

CREATE TABLE RecipeContent	(
	recipeID	INT	REFERENCES Recipe( ID ),
	categoryID	INT	REFERENCES Category( ID ),
	minQuantity	INT,
	maxQuantity	INT,
	comment		TEXT,
	UNIQUE ( recipeID, categoryID )
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
	quantity	INT,
	UNIQUE ( configID, componentID )
);

CREATE TABLE ComposedConfig	(
	configID	INT	REFERENCES Configuration( ID ),
	subConfigID	INT	REFERENCES Configuration( ID ),
	quantity	INT,
	UNIQUE ( configID, subConfigID )
);
**file:category.simpleform
FORM Category
{
	id @int
	parentID @int
	name string
	normalizedName string
	description string
	picture []
	{
		id @int
	}
}
**file:feature.simpleform
FORM Feature
{
	id @int
	parentID @int
	name string
	normalizedName string
	description string
	picture []
	{
		id @int
	}
}
**file:tag.simpleform
FORM Tag
{
	id @int
	parentID @int
	name string
	normalizedName string
	description string
}
**file:manufacturer.simpleform
FORM Manufacturer
{
	manufacturer
	{
		id @int
		form @string
		name string
		webPage string
		picture
		{
			id @int
			image string
			_ string
		}
		search string
	}
}
**file:picture.simpleform
FORM Picture
{
	picture
	{
		id @int
		caption string
		info string
		thumbnail string
		image string
		width int
		height int
		size int
		tags string
		used_categories string
		used_features string
		tagwrap
		{
			id @int
			tag []
			{
				id @int
			}
		}
	}
	
	tagwrap []
	{
		id @int
		tag string
	}

	list
	{
		picture []
		{
			id @int
			caption string
			info string
			thumbnail string
			image string
			width int
			height int
			size int
			tags string
		}
	}
}
**file:configurator.normalize
int=number:integer;
uint=number:unsigned;
float=number:float;
name=string:lcname;
**file:program.sql
PREPARE getLastCategoryID AS SELECT DISTINCT last_insert_rowid() FROM Category;
PREPARE getLastFeatureID AS SELECT DISTINCT last_insert_rowid() FROM Feature;
PREPARE getLastTagID AS SELECT DISTINCT last_insert_rowid() FROM Tag;
PREPARE getLastPictureID AS SELECT DISTINCT last_insert_rowid( ) FROM Picture;
**file:DBPRG.tdl
--
-- addCategory
--
TRANSACTION addCategory -- (parentID, name, normalizedName, description, pictures/picture/id)
BEGIN
	DO NONEMPTY UNIQUE SELECT rgt FROM Category WHERE ID = $(parentID);
	DO UPDATE Category SET rgt = rgt + 2 WHERE rgt >= $1;
	DO UPDATE Category SET lft = lft + 2 WHERE lft > $1;
	DO INSERT INTO Category (parentID, name, normalizedName, description, lft, rgt) VALUES ($(parentID), $(name), $(normalizedName), $(description), $1, $1+1);
	INTO . DO NONEMPTY UNIQUE SELECT ID AS "ID" from Category WHERE normalizedName = $(normalizedName);
	FOREACH pictures/picture DO INSERT INTO CategoryPicture( categoryID, pictureID ) VALUES( $1, $(id) );
END

--
-- deleteCategory
--
TRANSACTION deleteCategory -- (id)
BEGIN
	DO NONEMPTY UNIQUE SELECT lft,rgt,rgt-lft+1 AS width FROM Category WHERE ID = $(id) AND ID != '1';
	DO DELETE FROM CategoryPicture WHERE categoryID = $(id);
	DO DELETE FROM Category WHERE lft >= $1 AND lft <= $2;
	DO UPDATE Category SET lft = lft-$3 WHERE lft>$2;
	DO UPDATE Category SET rgt = rgt-$3 WHERE rgt>$2;
END

--
-- updateCategory
--
TRANSACTION updateCategory -- (id, name, normalizedName, description, pictures/picture/id)
BEGIN
	DO UPDATE Category SET name = $(name), normalizedName = $(normalizedName), description = $(description) WHERE ID = $(id);
	DO DELETE FROM CategoryPicture WHERE categoryID = $(id);
	FOREACH pictures/picture DO INSERT INTO CategoryPicture( categoryID, pictureID ) VALUES( $(/id), $(id) );
END

--
-- selectCategory                 :Get the category
-- selectCategoryByNormalizedName :Get the category by name
-- selectCategoryList             :Get a list of categories
--
TRANSACTION selectCategory -- (id)
BEGIN
	INTO . DO NONEMPTY UNIQUE SELECT ID AS id,parentID AS "parentID", name,
		normalizedName AS "normalizedName",description
		FROM Category WHERE ID = $(id);
	INTO picture DO 
		SELECT CategoryPicture.pictureID AS "id"
			FROM CategoryPicture
			WHERE CategoryPicture.categoryID = $(id);
END

TRANSACTION selectCategoryByNormalizedName -- (normalizedName)
BEGIN
	INTO . DO NONEMPTY UNIQUE
		SELECT ID AS id, parentID AS "parentID", name,
			normalizedName AS "normalizedName",description FROM Category
			WHERE normalizedName = $(normalizedName);
	INTO picture DO
		SELECT pictureID AS "id"
			FROM CategoryPicture
			WHERE CategoryPicture.categoryID = $1;
END

TRANSACTION selectCategorySet -- (/category/id)
BEGIN
	FOREACH /category INTO category DO NONEMPTY UNIQUE
		SELECT ID AS "ID", name, normalizedName AS "normalizedName", description
			FROM Category WHERE ID = $(id) ORDER BY ID;
END

--
-- selectTopCategory       :Get the parents of a category
--
TRANSACTION selectTopCategory -- (id)
BEGIN
	INTO node DO SELECT P2.ID AS "ID",P2.parentID AS "parentID",P2.name,P2.normalizedName AS "normalizedName",P2.description FROM category AS P1, category AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P1.ID = $(id) ORDER BY P2.ID;
END

--
-- selectSubCategory       :Get the Category
--
TRANSACTION selectSubCategory -- (id)
BEGIN
	INTO node DO SELECT P1.ID AS "ID",P1.parentID AS "parentID",P1.name,P1.normalizedName AS "normalizedName",P1.description FROM category AS P1, category AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P2.ID = $(id) ORDER BY P1.ID;
END
--
-- addFeature
--
TRANSACTION addFeature -- (parentID, name, normalizedName, description, pictures/picture/id)
BEGIN
	DO NONEMPTY UNIQUE SELECT rgt FROM Feature WHERE ID = $(parentID);
	DO UPDATE Feature SET rgt = rgt + 2 WHERE rgt >= $1;
	DO UPDATE Feature SET lft = lft + 2 WHERE lft > $1;
	DO INSERT INTO Feature (parentID, name, normalizedName, description, lft, rgt) VALUES ($(parentID), $(name), $(normalizedName), $(description), $1, $1+1);
	INTO . DO NONEMPTY UNIQUE SELECT ID AS "ID" from Feature WHERE normalizedName = $(normalizedName);
	FOREACH pictures/picture DO INSERT INTO FeaturePicture( featureID, pictureID ) VALUES( $1, $(id) );
END

--
-- deleteFeature
--
TRANSACTION deleteFeature -- (id)
BEGIN
	DO NONEMPTY UNIQUE SELECT lft,rgt,rgt-lft+1 AS width FROM Feature WHERE ID = $(id) AND ID != '1';
	DO DELETE FROM FeaturePicture WHERE featureID = $(id);
	DO DELETE FROM Feature WHERE lft >= $1 AND lft <= $2;
	DO UPDATE Feature SET lft = lft-$3 WHERE lft>$2;
	DO UPDATE Feature SET rgt = rgt-$3 WHERE rgt>$2;
END

--
-- updateFeature
--
TRANSACTION updateFeature -- (id, name, normalizedName, description, pictures/picture/id)
BEGIN
	DO UPDATE Feature SET name = $(name), normalizedName = $(normalizedName), description = $(description) WHERE ID = $(id);
	DO DELETE FROM FeaturePicture WHERE featureID = $(id);
	FOREACH pictures/picture DO INSERT INTO FeaturePicture( featureID, pictureID ) VALUES( $(/id), $(id) );
END

--
-- selectFeature                 :Get the feature
-- selectFeatureByNormalizedName :Get the feature by name
-- selectFeatureList             :Get a list of categories
--
TRANSACTION selectFeature -- (id)
BEGIN
	INTO . DO NONEMPTY UNIQUE
		SELECT ID AS id, parentID AS "parentID", name,
			normalizedName AS "normalizedName", description
			FROM Feature WHERE ID = $(id);
	INTO picture DO
		SELECT pictureID AS "id"
			FROM FeaturePicture
			WHERE FeaturePicture.featureID = $(id);
END

TRANSACTION selectFeatureByNormalizedName -- (normalizedName)
BEGIN
	INTO . DO NONEMPTY UNIQUE
		SELECT ID AS id, parentID AS "parentID", name,
			normalizedName AS "normalizedName", description
			FROM Feature WHERE normalizedName = $(normalizedName);
	INTO picture DO SELECT pictureID AS id
		FROM FeaturePicture
		WHERE FeaturePicture.featureID = $1;
END

TRANSACTION selectFeatureSet -- (/feature/id)
BEGIN
	FOREACH /feature INTO feature DO NONEMPTY UNIQUE
		SELECT ID AS id, name, normalizedName AS "normalizedName", description
			FROM Feature WHERE ID = $(id) ORDER BY ID;
END

--
-- selectTopFeature       :Get the parents of a feature
--
TRANSACTION selectTopFeature -- (id)
BEGIN
	INTO node DO SELECT P2.ID AS "ID",P2.parentID AS "parentID",P2.name,P2.normalizedName AS "normalizedName",P2.description FROM Feature AS P1, Feature AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P1.ID = $(id) ORDER BY P2.ID;
END

--
-- selectSubFeature       :Get the feature
--
TRANSACTION selectSubFeature -- (id)
BEGIN
	INTO node DO SELECT P1.ID AS "ID",P1.parentID AS "parentID",P1.name,P1.normalizedName AS "normalizedName",P1.description FROM Feature AS P1, Feature AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P2.ID = $(id) ORDER BY P1.ID;
END
--
-- addTag
--
TRANSACTION addTag -- (parentID, name, normalizedName, description)
BEGIN
	DO NONEMPTY UNIQUE SELECT rgt FROM Tag WHERE ID = $(parentID);
	DO UPDATE Tag SET rgt = rgt + 2 WHERE rgt >= $1;
	DO UPDATE Tag SET lft = lft + 2 WHERE lft > $1;
	DO INSERT INTO Tag (parentID, name, normalizedName, description, lft, rgt) VALUES ($(parentID), $(name), $(normalizedName), $(description), $1, $1+1);
	INTO . DO NONEMPTY UNIQUE SELECT ID AS "ID" from Tag WHERE normalizedName = $(normalizedName);
END

--
-- deleteTag
--
TRANSACTION deleteTag -- (id)
BEGIN
	DO NONEMPTY UNIQUE SELECT lft,rgt,rgt-lft+1 AS width FROM Tag WHERE ID = $(id) AND ID != '1';
	DO DELETE FROM Tag WHERE lft >= $1 AND lft <= $2;
	DO UPDATE Tag SET lft = lft-$3 WHERE lft>$2;
	DO UPDATE Tag SET rgt = rgt-$3 WHERE rgt>$2;
END

--
-- updateTag
--
TRANSACTION updateTag -- (id, name, normalizedName, description)
BEGIN
	DO UPDATE Tag SET name = $(name), normalizedName = $(normalizedName), description = $(description) WHERE ID = $(id);
END

--
-- selectTag                 :Get the tag
-- selectTagByNormalizedName :Get the tag by name
-- selectTagList             :Get a list of categories
--
TRANSACTION selectTag -- (id)
BEGIN
	INTO . DO NONEMPTY UNIQUE SELECT ID AS id,parentID AS "parentID",name,normalizedName AS "normalizedName",description FROM Tag WHERE ID = $(id);
END

TRANSACTION selectTagByNormalizedName -- (normalizedName)
BEGIN
	INTO . DO NONEMPTY UNIQUE SELECT ID AS id,parentID AS "parentID",name,normalizedName AS "normalizedName",description FROM Tag WHERE normalizedName = $(normalizedName);
END

TRANSACTION selectTagSet -- (/tag/id)
BEGIN
	FOREACH /tag INTO tag DO NONEMPTY UNIQUE SELECT ID AS id,name,normalizedName AS "normalizedName",description FROM Tag WHERE ID = $(id) ORDER BY ID;
END

--
-- selectTopTag       :Get the parents of a tag
--
TRANSACTION selectTopTag -- (id)
BEGIN
	INTO node DO SELECT P2.ID AS "ID",P2.parentID AS "parentID",P2.name,P2.normalizedName AS "normalizedName",P2.description FROM Tag AS P1, Tag AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P1.ID = $(id) ORDER BY P2.ID;
END

--
-- selectSubTag       :Get the tag
--
TRANSACTION selectSubTag -- (id)
BEGIN
	INTO node DO SELECT P1.ID AS "ID",P1.parentID AS "parentID",P1.name,P1.normalizedName AS "normalizedName",P1.description FROM Tag AS P1, Tag AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P2.ID = $(id) ORDER BY P1.ID;
END
TRANSACTION selectManufacturerList
RESULT INTO list
BEGIN
	INTO manufacturer DO
		SELECT Manufacturer.ID AS "id", name, webPage AS "webPage", thumbnail
		FROM Manufacturer
		LEFT JOIN Picture
		ON Manufacturer.logo = Picture.ID;
END

TRANSACTION addManufacturer
BEGIN
	DO INSERT INTO Manufacturer( name, normalizedName, webPage, logo )
		VALUES ( $(name), $(normalizedName), $(webPage), $(logo) );
END

TRANSACTION updateManufacturer
BEGIN
	DO UPDATE Manufacturer SET name=$(name), normalizedName=$(normalizedName), webPage=$(webPage), logo=$(logo)
		WHERE ID=$(id);
END

TRANSACTION deleteManufacturer
BEGIN
	DO DELETE FROM Manufacturer WHERE ID = $(id);
END

TRANSACTION selectManufacturer
RESULT INTO manufacturer
BEGIN
	INTO . DO
		SELECT Manufacturer.ID AS "id", name, webPage AS "webPage"
		FROM Manufacturer
		WHERE Manufacturer.ID = $(id);
	INTO picture DO
		SELECT Picture.ID AS "id", image
		FROM Manufacturer
		LEFT JOIN Picture
		ON Manufacturer.logo = Picture.ID
		WHERE Manufacturer.ID = $(id);
END
TRANSACTION selectPictureList
RESULT INTO list
BEGIN
	INTO picture DO
		SELECT Picture.ID AS "id", thumbnail, caption, info, width, height,
			coalesce( group_concat( Tag.name ), '' ) as tags,
			3*length(image)/4+2 as size
		FROM Picture
		LEFT JOIN PictureTag
		ON PictureTag.pictureID = Picture.ID
		LEFT JOIN Tag
		ON PictureTag.tagID = Tag.ID
		WHERE coalesce( Tag.normalizedName, '' ) like $(search)
		GROUP BY Picture.ID, Picture.thumbnail, Picture.caption, Picture.info,
			Picture.width, Picture.height, Picture.image;
END

TRANSACTION selectPicture -- (/picture/id)
BEGIN
	INTO picture DO UNIQUE
		SELECT Picture.ID AS "id", image, caption, info, width, height,
			coalesce( group_concat( distinct Tag.name ), '' ) as tags,
			3*length(image)/4+2 as size,
			coalesce( group_concat( distinct Category.name ), '' ) as used_categories,
			coalesce( group_concat( distinct Feature.name ), '' ) as used_features
		FROM Picture
		LEFT JOIN PictureTag
		ON PictureTag.pictureID = Picture.ID
		LEFT JOIN Tag
		ON PictureTag.tagID = Tag.ID
		LEFT JOIN CategoryPicture
		ON CategoryPicture.pictureID = Picture.ID
		LEFT JOIN Category
		ON CategoryPicture.categoryID = Category.ID
		LEFT JOIN FeaturePicture
		ON FeaturePicture.pictureID = Picture.ID
		LEFT JOIN Feature
		ON FeaturePicture.featureID = Feature.ID
		WHERE Picture.ID = $(id)
		GROUP BY Picture.ID, Picture.image, Picture.caption, Picture.info,
			Picture.width, Picture.height;

	-- HACK from here, see wrapper element in LUA code, no other solution found..
	INTO tagwrap DO
		select Tag.ID AS "id", name as tag
		FROM Tag
		LEFT JOIN PictureTag
		ON PictureTag.tagID = Tag.ID
		LEFT JOIN Picture
		ON PictureTag.pictureID = Picture.ID
		WHERE Picture.ID = $(id);
END

TRANSACTION updatePicture -- (picture/id, picture/caption, picture/info, picture/image, picture/width, picture/height, picture/thumbnail)
BEGIN
	FOREACH picture DO UPDATE Picture SET
		caption = $(caption), info = $(info), image = $(image),
		width = $(width), height = $(height), thumbnail = $(thumbnail)
		WHERE ID = $(id);
	FOREACH picture DO DELETE FROM PictureTag WHERE pictureID = $(id);
	FOREACH picture/tags DO INSERT INTO PictureTag( pictureID, tagID ) VALUES( $(/picture/id), $(id) );
END

TRANSACTION addPicture -- (picture/caption, picture/info, picture/image, picture/width, picture/height, picture/thumbnail)
BEGIN
	FOREACH picture DO INSERT INTO Picture( caption, info, image, width, height, thumbnail )
		VALUES ($(caption), $(info), $(image), $(width), $(height), $(thumbnail) );
	DO NONEMPTY UNIQUE getLastPictureID( );
	FOREACH picture/tags DO INSERT INTO PictureTag( pictureID, tagID ) VALUES( $1, $(id) );
END

TRANSACTION deletePicture -- (id)
BEGIN
	DO DELETE FROM PictureTag WHERE PictureID = $(id);
	DO DELETE FROM Picture WHERE ID = $(id);
END
**outputfile:DBDUMP
**file: transaction_configurator.lua
local function content_value( itr)
	local rt = nil
	for v,t in itr do
		if v and not t then
			rt = v
		end
	end
	return rt
end

local function pictures_value( pictures, itr )
	if pictures == nil then
		pictures = { ["picture"] = { } }
	end
	for v,t in itr do
		if( t == "id") then
			table.insert( pictures[ "picture" ], { ["id"] = v } )
		end
	end
	return pictures
end

local function insert_itr( tablename, parentID, itr)
	local id = 1
	local name = nil
	local nname = nil
	local description = nil
	local pictures = nil
	for v,t in itr do
		if (t == "name") then
			if v then
				name = v
			else
				name = content_value( scope(itr))
			end
			nname = normalizer("name")( name)
		elseif (t == "description") then
			if v then
				description = v
			else
				description = content_value( scope(itr))
			end
		elseif (t == "picture") then
			pictures = pictures_value( pictures, scope( itr))
		elseif (t == "node") then
			if name then
				id = formfunction( "add" .. tablename)( {name=name, normalizedName=nname, description=description, parentID=parentID, pictures=pictures} ):table().ID
				name = nil
				description = nil
			end
			insert_itr( tablename, id, scope( itr))
		end
	end
	if name then
		id = formfunction( "add" .. tablename)( {name=name, normalizedName=nname, description=description, parentID=parentID, pictures=pictures} ):table().ID
	end
	return id
end

local function insert_topnode( tablename, name, description, pictures, parentID)
	local nname = normalizer("name")( name)
	if not parentID then
		parentID = 1
	end
	local id = formfunction( "add" .. tablename)( {normalizedName=nname, name=name, description=description, parentID=parentID, pictures=pictures} ):table().ID
	return id
end

local function insert_tree_topnode( tablename, itr)
	local parentID = nil
	local id = 1
	local name = nil
	local description = nil
	local pictures = nil
	for v,t in itr do
		if (t == "parentID") then
			parentID = tonumber( v)
		elseif (t == "name") then
			if v then
				name = v
			else
				name = content_value( scope(itr))
			end
		elseif (t == "description") then
			if v then
				description = v
			else
				description = content_value( scope(itr))
			end
		elseif (t == "picture") then
			pictures = pictures_value( pictures, scope( itr))
		elseif (t == "node") then
			if name then
				id = insert_topnode( tablename, name, description, pictures, parentID)
				name = nil
				description = nil
			end
			insert_itr( tablename, id, scope( itr))
		end
	end
	if name then
		insert_topnode( tablename, name, description, pictures, parentID)
	end
end

local function get_tree( tablename, parentID)
	local t = formfunction( "selectSub" .. tablename)( {id=parentID} ):table()["node"] or {}
	local a = {}
	for i,v in pairs( t) do
		table.insert( a, tonumber( v.ID), { name=v.name, description=v.description, pictures=v.pictures, parentID=tonumber(v.parentID), children = {} } )
	end
	for i,v in pairs( a) do
		if i ~= parentID and v.parentID then
			table.insert( a[ v.parentID ].children, i )
		end
	end
	return a
end

local function print_tree( tree, tagname, nodeid, indent)
	if (indent ~= "") then
		output:print( "\n" .. indent)
	end
	output:opentag( "tree" )
	if tree[ nodeid ] then
		output:opentag( "item" )
		output:print( nodeid, "id")
		output:print( "\n" .. indent .. "\t")
		output:opentag( tagname)
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
			print_tree( tree, tagname, v, indent .. "\t")
			n = n + 1
		end
		if n > 0 then
			output:print( "\n" .. indent)
		end
		output:closetag( )
	end
	output:closetag()
end

local function select_tree( tablename, tagname, itr)
	filter().empty = false
	for v,t in itr do
		if t == "id" then
			local id = tonumber( v)
			print_tree( get_tree( tablename, id), tagname, id, "")
		end
	end
end

local function select_node( tablename, elementname, itr)
	filter().empty = false
	for v,t in itr do
		if t == "id" then
			output:opentag( elementname)
			local r = formfunction( "select" .. tablename)( {id=v} )
			local f = form( tablename)
			f:fill( r:get())
			output:print( f:get())
			output:closetag( )
		end
	end
end

local function edit_node( tablename, itr)
	local name = nil
	local nname = nil
	local description = nil
	local pictures = nil
	local id = nil
	for v,t in itr do
		if( t == "id" ) then
			id = v
		elseif t ==  "name" then
			if v then
				name = v
			else
				name = content_value( scope(itr))
			end
			nname = normalizer("name")( name)
		elseif t == "description" then
			if v then
				description = v
			else
				description = content_value( scope(itr))
			end
		elseif( t == "picture" ) then
			pictures = pictures_value( pictures, scope( itr))
		end
	end
	formfunction( "update" .. tablename)( {normalizedName=nname, name=name, description=description, id=id, pictures=pictures} )
end

local function delete_node( tablename, itr)
	local id = nil
	for v,t in itr do
		if t == "id" then
			id = v
		end
	end
	formfunction( "delete" .. tablename)( {id=id} )
end

local function create_node( tablename, itr)
	local name = nil
	local parentID = nil
	local description = nil
	local pictures = nil
	for v,t in itr do
		if t == "parentID" then
			parentID = v
		elseif t ==  "name" then
			if v then
				name = v
			else
				name = content_value( scope(itr))
			end
			nname = normalizer("name")( name)
		elseif t ==  "description" then
			if v then
				description = v
			else
				description = content_value( scope(itr))
			end
		elseif t ==  "picture" then
			pictures = pictures_value( pictures, scope( itr))
		end
	end
	insert_topnode( tablename, name, description, pictures, parentID)
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
	output:as( {root='tree', system='CategoryHierarchy.simpleform'})
	select_tree( "Category", "category", input:get())
end

function FeatureHierarchyRequest()
	output:as( {root='tree', system='FeatureHierarchy.simpleform'})
	select_tree( "Feature", "feature", input:get())
end

function TagHierarchyRequest()
	output:as( {root='tree', system='TagHierarchy.simpleform'})
	select_tree( "Tag", "tag", input:get())
end

function pushCategoryHierarchy()
	add_tree( "Category", input:get())
end

function pushFeatureHierarchy()
	add_tree( "Feature", input:get())
end

function pushTagHierarchy()
	add_tree( "Tag", input:get())
end

function CategoryRequest()
	output:as( {root='category', system='Category.simpleform'})
	select_node( "Category", "category", input:get())
end

function FeatureRequest()
	output:as( {root='feature', system='Feature.simpleform'})
	select_node( "Feature", "feature", input:get())
end

function TagRequest()
	output:as( {root='tag', system='Tag.simpleform'})
	select_node( "Tag", "tag", input:get())
end

function editCategory()
	edit_node( "Category", input:get())
end

function editFeature()
	edit_node( "Feature", input:get())
end

function editTag()
	edit_node( "Tag", input:get())
end

function deleteCategory()
	delete_node( "Category", input:get())
end

function deleteFeature()
	delete_node( "Feature", input:get())
end

function deleteTag()
	delete_node( "Tag", input:get())
end

function createCategory()
	create_node( "Category", input:get())
end

function createFeature()
	create_node( "Feature", input:get())
end

function createTag()
	create_node( "Tag", input:get())
end

-- manufacturers

function ManufacturerListRequest( )
	output:as( {root='list', system='manufacturerList.simpleform'})
	local t = formfunction( "selectManufacturerList" )( {} )
	local f = form( "ManufacturerList" )
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

function createManufacturer( )
	local manufacturer = input:table( )["manufacturer"]
	if manufacturer["picture"] then
		manufacturer["logo"] = manufacturer["picture"]["id"]
	end
	if manufacturer["name"] then
		manufacturer["normalizedName"] = normalizer( "name" )( manufacturer["name"] )
	end
	formfunction( "addManufacturer" )( manufacturer )
end

function editManufacturer( )
	local manufacturer = input:table( )["manufacturer"]
	if manufacturer["picture"] then
		manufacturer["logo"] = manufacturer["picture"]["id"]
	end
	if manufacturer["name"] then
		manufacturer["normalizedName"] = normalizer( "name" )( manufacturer["name"] )
	end
	formfunction( "updateManufacturer" )( manufacturer )
end

function deleteManufacturer( )
	formfunction( "deleteManufacturer" )( { id = input:table( )["manufacturer"]["id"] } )
end

function ManufacturerRequest( )
	local t = formfunction( "selectManufacturer" )( { id = input:table( )["manufacturer"]["id"] } )
	local f = form( "Manufacturer" )
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

-- components

function ComponentListRequest( )
	output:as( {root='list', system='componentList.simpleform'})
	local t = formfunction( "selectComponentList" )( {} )
	local f = form( "ComponentList" )
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

function createComponent( )
	local component = input:table( )["component"]
	if component["category"] then
		component["categoryID"] = component["category"]["id"]
	end
	if component["manufacturer"] then
		component["manufacturerID"] = component["manufacturer"]["id"]
	end
	if component["name"] then
		component["normalizedName"] = normalizer( "name" )( component["name"] )
	end
	formfunction( "addComponent" )( component )
end

function editComponent( )
	local component = input:table( )["component"]
	if component["category"] then
		component["categoryID"] = component["category"]["id"]
	end
	if component["manufacturer"] then
		component["manufacturerID"] = component["manufacturer"]["id"]
	end
	if component["name"] then
		component["normalizedName"] = normalizer( "name" )( component["name"] )
	end
	formfunction( "updateComponent" )( component )
end

function deleteComponent( )
	formfunction( "deleteComponent" )( { id = input:table( )["component"]["id"] } )
end

function ComponentRequest( )
	local t = formfunction( "selectComponent" )( { id = input:table( )["component"]["id"] } )
	local f = form( "Component" )
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

-- category/features associations

function createCategoryFeature( )
	local categoryFeature = input:table( )["CategoryFeature"]
	formfunction( "addCategoryFeature" )( categoryFeature )
end

function deleteCategoryFeature( )
	local categoryFeature = input:table( )["CategoryFeature"]
	formfunction( "deleteCategoryFeature" )( categoryFeature )
end

function CategoryFeatureRequest( )
	local categoryFeature = input:table( )["CategoryFeature"]
	local t = formfunction( "selectCategoryFeature" )( {
		category_id = categoryFeature["category_id"],
		feature_id = categoryFeature["feature_id"]
	} )
	local f = form( "CategoryFeature" )
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

function CategoryFeatureListRequest( )
	local categoryFeature = input:table( )["CategoryFeature"]
	local t = formfunction( "selectCategoryFeatureList" )( {
		category_id = categoryFeature["category_id"]
	} )
	local f = form( "CategoryFeatureList" )
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

-- pictures

function PictureListRequest( )
	output:as( {root='list', system='PictureList.simpleform'})
	filter().empty = false
	local search = nil
	for v,t in input:get( ) do
		if t == "search" then
			search = "%" .. normalizer( "name" )( v ) .. "%"
		end
	end
	if search == nil then
		search = "%%"
	end
	local t = formfunction( "selectPictureList" )( { search = search } )
	local f = form( "Picture" )
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

function PictureRequest( )
	output:as( {root='dummy', system='Picture.simpleform'})
	filter().empty = false
	local id = nil
	for v,t in input:get( ) do
		if t == "id" then
			id = v
		end
	end
	local t = formfunction( "selectPicture" )( { id = id } )
	local f = form( "Picture" )
	f:fill( t:get( ) )
	-- hack again, can't get the right subtags in TDL..
	output:opentag( "dummy" )
	output:print( f:get( ) )
	output:closetag( )
end

local function transform_picture( itr )
	-- should be a form transformation, not lua code :-)
	local picture = {}
	picture["tags"] = {}
	local intag = false
	local intagwrap = false
	local inid = false
	for v,t in itr do
		if( not v and t ) then
			-- begin tag
			if( t == "tagwrap" ) then
				intagwrap = true
			elseif( t == "tag" ) then
				intag = true
			elseif( t == "caption" or t == "info" or t == "image" ) then
				picture[ t] = content_value( scope( itr))
			end
		elseif( v and t ) then
			-- attribute
			inid = true
			if ( ( t == "id" ) and not intagwrap and not intag ) then
				picture[ t] = v
			elseif( t == "id" and intag and intagwrap ) then
				table.insert( picture["tags"], { ["id"] = v } )
			end
		elseif( not v and not t ) then
			-- end tag
			if( inid ) then
				inid = false
			elseif( intag and intagwrap ) then
				intag = false
			elseif( intagwrap ) then
				intagwrap = false
			end
		else
			-- dummy content
		end
	end
	info = formfunction( "imageInfo" )( { [ "data"] = picture["image"] } ):table( )
	picture["width"] = info.width
	picture["height"] = info.height
	thumb = formfunction( "imageThumb" )( { [ "image" ] = { [ "data" ] = picture["image"] }, [ "size" ] = 50 } ):table( )
	picture["thumbnail"] = thumb.data
	return picture
end

local function delete_picture( itr)
	local id = nil
	for v,t in itr do
		if t == "id" then
			id = v
		end
	end
	formfunction( "deletePicture" )( { id = id } )
end

local function update_picture( itr)
	local picture = transform_picture( itr)
	formfunction( "updatePicture" )( { picture = picture } )
end

local function create_picture( itr)
	local picture = transform_picture( itr)
	formfunction( "addPicture" )( { picture = picture } )
end

function editPicture( )
	update_picture( input:get())
end

function createPicture( )
	create_picture( input:get())
end

function deletePicture( )
	delete_picture( input:get())
end


local function create_manufacturer(itr)
	local manufacturerform = form("Manufacturer");
	manufacturerform:fill(itr)
	local manufacturer = manufacturerform:table()["manufacturer"]
	manufacturer["normalizedName"] = normalizer( "name" )( manufacturer["name"] )
	manufacturer["logo"] = manufacturer["picture"]["id"]
	formfunction( "addManufacturer" )( manufacturer )
end

function run()
	filter().empty = false
	output:as( {root='result', system='test.simpleform'})
	output:opentag("result")
	local itr = input:get()
	for v,t in itr do
		if (t == "pushCategoryHierarchy") then
			add_tree( "Category", scope(itr))
		elseif (t == "pushFeatureHierarchy") then
			add_tree( "Feature", scope(itr))
		elseif (t == "pushTagHierarchy") then
			add_tree( "Tag", scope(itr))
		elseif (t == "CategoryHierarchyRequest") then
			select_tree( "Category", "category", scope(itr))
		elseif (t == "FeatureHierarchyRequest") then
			select_tree( "Feature", "feature", scope(itr))
		elseif (t == "TagHierarchyRequest") then
			select_tree( "Tag", "tag", scope(itr))
		elseif (t == "editCategory") then
			edit_node( "Category", scope(itr))
		elseif (t == "editFeature") then
			edit_node( "Feature", scope(itr))
		elseif (t == "editTag") then
			edit_node( "Tag", scope(itr))
		elseif (t == "deleteCategory") then
			delete_node( "Category", scope(itr))
		elseif (t == "deleteFeature") then
			delete_node( "Feature", scope(itr))
		elseif (t == "deleteTag") then
			delete_node( "Tag", scope(itr))
		elseif (t == "createCategory") then
			create_node( "Category", scope(itr))
		elseif (t == "createFeature") then
			create_node( "Feature", scope(itr))
		elseif (t == "createTag") then
			create_node( "Tag", scope(itr))
		elseif (t == "CategoryRequest") then
			select_node( "Category", "category", scope(itr))
		elseif (t == "FeatureRequest") then
			select_node( "Feature", "feature", scope(itr))
		elseif (t == "TagRequest") then
			select_node( "Tag", "tag", scope(itr))
		elseif (t == "createPicture") then
			create_picture(scope(itr))
		elseif (t == "editPicture") then
			update_picture(scope(itr))
		elseif (t == "deletePicture") then
			delete_picture( scope(itr))
		elseif (t == "createManufacturer") then
			create_manufacturer( scope(itr))
		end
	end
	output:closetag()
end
**output
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE result SYSTEM "test.simpleform"><result><tree><item id="1">
	<category>_ROOT_</category>
	<description>Categories tree root</description>
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
</item></tree><category id="52" parentID="46"><name>WNC child</name><normalizedName>wnc child</normalizedName><picture id="1"/></category><category id="52" parentID="46"><name>WNC child Y</name><normalizedName>wnc child y</normalizedName><picture id="3"/></category><category id="54" parentID="46"><name>WNC child 3</name><normalizedName>wnc child 3</normalizedName><picture id="1"/></category><category id="46" parentID="16"><name>Wireless network component</name><normalizedName>wireless network component</normalizedName></category><tree><item id="1">
	<category>_ROOT_</category>
	<description>Categories tree root</description>
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
			<tree><item id="53">
				<category>WNC child 2</category></item></tree>
			<tree><item id="52">
				<category>WNC child Y</category></item></tree>
			<tree><item id="54">
				<category>WNC child 3</category></item></tree>
		</item></tree>
		<tree><item id="48">
			<category>Microcontroller</category></item></tree>
		<tree><item id="50">
			<category>Nanocomputer</category></item></tree>
	</item></tree>
</item></tree><tree><item id="1">
	<feature>_ROOT_</feature>
	<description>Features tree root</description>
	<tree><item id="2">
		<feature>Color</feature>
		<tree><item id="3">
			<feature>green</feature></item></tree>
		<tree><item id="4">
			<feature>blue</feature></item></tree>
		<tree><item id="5">
			<feature>gray</feature></item></tree>
		<tree><item id="6">
			<feature>red</feature></item></tree>
	</item></tree>
	<tree><item id="7">
		<feature>Size</feature>
		<tree><item id="8">
			<feature>big</feature></item></tree>
		<tree><item id="9">
			<feature>small</feature></item></tree>
		<tree><item id="10">
			<feature>tiny</feature></item></tree>
		<tree><item id="11">
			<feature>pocket size</feature></item></tree>
	</item></tree>
	<tree><item id="12">
		<feature>Noise</feature>
		<tree><item id="13">
			<feature>loud</feature></item></tree>
		<tree><item id="14">
			<feature>silent</feature></item></tree>
	</item></tree>
</item></tree><tree><item id="7">
	<feature>Size</feature>
	<tree><item id="10">
		<feature>tiny</feature></item></tree>
	<tree><item id="9">
		<feature>small</feature></item></tree>
	<tree><item id="11">
		<feature>pocket size</feature></item></tree>
	<tree><item id="8">
		<feature>big</feature></item></tree>
</item></tree><tree><item id="1">
	<feature>_ROOT_</feature>
	<description>Features tree root</description>
	<tree><item id="2">
		<feature>Color</feature>
		<tree><item id="3">
			<feature>green</feature></item></tree>
		<tree><item id="4">
			<feature>blue</feature></item></tree>
		<tree><item id="5">
			<feature>gray</feature></item></tree>
		<tree><item id="6">
			<feature>pink</feature></item></tree>
		<tree><item id="18">
			<feature>yellow</feature></item></tree>
	</item></tree>
	<tree><item id="7">
		<feature>Size</feature>
		<tree><item id="8">
			<feature>big</feature></item></tree>
		<tree><item id="9">
			<feature>small</feature></item></tree>
		<tree><item id="10">
			<feature>tiny</feature></item></tree>
		<tree><item id="11">
			<feature>pocket size</feature></item></tree>
	</item></tree>
	<tree><item id="12">
		<feature>Noise</feature>
		<tree><item id="13">
			<feature>loud</feature></item></tree>
		<tree><item id="14">
			<feature>silent</feature></item></tree>
		<tree><item id="16">
			<feature>mumling</feature></item></tree>
	</item></tree>
	<tree><item id="17">
		<feature>Space</feature></item></tree>
</item></tree><tree><item id="1">
	<tag>_ROOT_</tag>
	<description>Tags tree root</description>
	<tree><item id="2">
		<tag>Tag1</tag>
		<tree><item id="3">
			<tag>Tag1.1</tag></item></tree>
		<tree><item id="4">
			<tag>Tag1.2</tag></item></tree>
		<tree><item id="5">
			<tag>Tag1.3</tag></item></tree>
		<tree><item id="6">
			<tag>Tag1.4</tag></item></tree>
	</item></tree>
	<tree><item id="7">
		<tag>Tag2</tag>
		<tree><item id="8">
			<tag>Tag2.1</tag></item></tree>
		<tree><item id="9">
			<tag>Tag2.2</tag></item></tree>
		<tree><item id="11">
			<tag>Tag2.4</tag></item></tree>
	</item></tree>
	<tree><item id="12">
		<tag>Tag3</tag>
		<tree><item id="13">
			<tag>Tag3.1</tag></item></tree>
		<tree><item id="14">
			<tag>Tag3.2</tag></item></tree>
		<tree><item id="15">
			<tag>Tag3.3</tag></item></tree>
		<tree><item id="16">
			<tag>Tag3.4</tag></item></tree>
	</item></tree>
</item></tree><tree><item id="7">
	<tag>Tag2</tag>
	<tree><item id="9">
		<tag>Tag2.2</tag></item></tree>
	<tree><item id="8">
		<tag>Tag2.1</tag></item></tree>
	<tree><item id="11">
		<tag>Tag2.4</tag></item></tree>
</item></tree><tree><item id="1">
	<tag>_ROOT_</tag>
	<description>Tags tree root</description>
	<tree><item id="2">
		<tag>Tag1</tag>
		<tree><item id="3">
			<tag>Tag1.1</tag></item></tree>
		<tree><item id="4">
			<tag>Tag1.2</tag></item></tree>
		<tree><item id="5">
			<tag>Tag1.3</tag></item></tree>
		<tree><item id="6">
			<tag>Tag1.4x</tag></item></tree>
		<tree><item id="19">
			<tag>Tag1.5x</tag></item></tree>
	</item></tree>
	<tree><item id="7">
		<tag>Tag2</tag>
		<tree><item id="8">
			<tag>Tag2.1</tag></item></tree>
		<tree><item id="9">
			<tag>Tag2.2</tag></item></tree>
		<tree><item id="11">
			<tag>Tag2.4</tag></item></tree>
	</item></tree>
	<tree><item id="12">
		<tag>Tag3</tag>
		<tree><item id="13">
			<tag>Tag3.1</tag></item></tree>
		<tree><item id="14">
			<tag>Tag3.2</tag></item></tree>
		<tree><item id="15">
			<tag>Tag3.3</tag></item></tree>
		<tree><item id="16">
			<tag>Tag3.4</tag></item></tree>
		<tree><item id="17">
			<tag>Tag3.5x</tag></item></tree>
	</item></tree>
	<tree><item id="18">
		<tag>Tag5</tag></item></tree>
</item></tree></result>
Tag:
'1', NULL, '_ROOT_', '_ROOT_', 'Tags tree root', '1', '36'
'2', '1', 'Tag1', 'tag1', NULL, '2', '13'
'3', '2', 'Tag1.1', 'tag1 1', NULL, '3', '4'
'4', '2', 'Tag1.2', 'tag1 2', NULL, '5', '6'
'5', '2', 'Tag1.3', 'tag1 3', NULL, '7', '8'
'6', '2', 'Tag1.4x', 'tag1 4x', NULL, '9', '10'
'7', '1', 'Tag2', 'tag2', NULL, '14', '21'
'8', '7', 'Tag2.1', 'tag2 1', NULL, '15', '16'
'9', '7', 'Tag2.2', 'tag2 2', NULL, '17', '18'
'11', '7', 'Tag2.4', 'tag2 4', NULL, '19', '20'
'12', '1', 'Tag3', 'tag3', NULL, '22', '33'
'13', '12', 'Tag3.1', 'tag3 1', NULL, '23', '24'
'14', '12', 'Tag3.2', 'tag3 2', NULL, '25', '26'
'15', '12', 'Tag3.3', 'tag3 3', NULL, '27', '28'
'16', '12', 'Tag3.4', 'tag3 4', NULL, '29', '30'
'17', '12', 'Tag3.5x', 'tag3 5x', NULL, '31', '32'
'18', '1', 'Tag5', 'tag5', NULL, '34', '35'
'19', '2', 'Tag1.5x', 'tag1 5x', NULL, '11', '12'

sqlite_sequence:
'Tag', '19'
'Category', '54'
'Feature', '18'
'Picture', '3'

Picture:
'1', 'WNC caption X', 'WNC info X', '11', '90', 'WNC image X', 'WNC image X'
'2', 'WNC caption X', 'WNC info X', '11', '90', 'WNC image X', 'WNC image X'
'3', 'WNC caption Z', 'WNC info Z', '11', '90', 'WNC image Z', 'WNC image Z'

PictureTag:
'2', NULL
'3', NULL
'1', NULL

Category:
'1', NULL, '_ROOT_', '_ROOT_', 'Categories tree root', '1', '64'
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
'16', '1', 'Mobile', 'mobile', NULL, '32', '63'
'17', '16', 'Desknote', 'desknote', NULL, '33', '34'
'18', '16', 'Laptop', 'laptop', NULL, '35', '44'
'19', '18', 'Notebook', 'notebook', NULL, '36', '39'
'20', '19', 'Subnotebook', 'subnotebook', NULL, '37', '38'
'21', '18', 'Tablet personal computer', 'tablet personal computer', NULL, '40', '41'
'44', '18', 'Wearable computer', 'wearable computer', NULL, '42', '43'
'45', '16', 'Single board computer', 'single board computer', NULL, '45', '46'
'46', '16', 'Wireless network component', 'wireless network component', NULL, '47', '54'
'47', '16', 'Plug computer', 'plug computer', NULL, '55', '56'
'48', '16', 'Microcontroller', 'microcontroller', NULL, '57', '58'
'49', '16', 'Smartdust', 'smartdust', NULL, '59', '60'
'50', '16', 'Nanocomputer', 'nanocomputer', NULL, '61', '62'
'51', '7', 'Device from outer space', 'device from outer space', NULL, '29', '30'
'52', '46', 'WNC child Y', 'wnc child y', NULL, '48', '49'
'53', '46', 'WNC child 2', 'wnc child 2', NULL, '50', '51'
'54', '46', 'WNC child 3', 'wnc child 3', NULL, '52', '53'

CategoryPicture:
'53', '1'
'54', '1'
'52', '3'

Feature:
'1', NULL, '_ROOT_', '_ROOT_', 'Features tree root', '1', '34'
'2', '1', 'Color', 'color', NULL, '2', '13'
'3', '2', 'green', 'green', NULL, '3', '4'
'4', '2', 'blue', 'blue', NULL, '5', '6'
'5', '2', 'gray', 'gray', NULL, '7', '8'
'6', '2', 'pink', 'pink', NULL, '9', '10'
'7', '1', 'Size', 'size', NULL, '14', '23'
'8', '7', 'big', 'big', NULL, '15', '16'
'9', '7', 'small', 'small', NULL, '17', '18'
'10', '7', 'tiny', 'tiny', NULL, '19', '20'
'11', '7', 'pocket size', 'pocket size', NULL, '21', '22'
'12', '1', 'Noise', 'noise', NULL, '24', '31'
'13', '12', 'loud', 'loud', NULL, '25', '26'
'14', '12', 'silent', 'silent', NULL, '27', '28'
'16', '12', 'mumling', 'mumling', NULL, '29', '30'
'17', '1', 'Space', 'space', NULL, '32', '33'
'18', '2', 'yellow', 'yellow', NULL, '11', '12'

FeaturePicture:

Manufacturer:

Component:

ComponentPriceHistory:

ComponentPicture:

CategRequires:

CategProvides:

CategoryCheck:

ComponentRequires:

ComponentProvides:

ComponentCheck:

Recipe:

RecipePicture:

RecipeContent:

Configuration:

ConfigComponent:

ComposedConfig:

**end

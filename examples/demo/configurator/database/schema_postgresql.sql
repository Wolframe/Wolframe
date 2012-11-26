-- The tags tree
--
CREATE TABLE Tag	(
	ID SERIAL NOT NULL PRIMARY KEY,
	parentID	INT	REFERENCES Tag( ID ),
	name		TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL UNIQUE,
	description	TEXT,
	lft		INT	NOT NULL,
	rgt		INT	NOT NULL,
	CONSTRAINT order_check CHECK ( rgt > lft )
);
ALTER SEQUENCE Tag_ID_seq RESTART WITH 1;

INSERT INTO Tag( parentID, name, normalizedName, description, lft, rgt )
	VALUES ( NULL, '_ROOT_', '_ROOT_', 'Tags tree root', 1, 2 );


-- The list of images used
--
CREATE TABLE Picture	(
	ID SERIAL NOT NULL PRIMARY KEY,
	caption		TEXT,
	info		TEXT,
	width		INT,
	height		INT,
	image		TEXT,
	thumbnail	TEXT
);
ALTER SEQUENCE Picture_ID_seq RESTART WITH 1;

CREATE TABLE PictureTag	(
	pictureID	INT	REFERENCES Picture( ID ),
	tagID		INT	REFERENCES Tag( ID ),
	UNIQUE ( pictureID, tagID )
);


-- The categories tree
--
CREATE TABLE Category	(
	ID SERIAL NOT NULL PRIMARY KEY,
	parentID	INT	REFERENCES Category( ID ),
	name		TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL UNIQUE,
	description	TEXT,
	lft		INT	NOT NULL,
	rgt		INT	NOT NULL,
	CONSTRAINT order_check CHECK ( rgt > lft )
);
ALTER SEQUENCE Category_ID_seq RESTART WITH 1;

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
	ID SERIAL NOT NULL PRIMARY KEY,
	parentID	INT	REFERENCES Feature( ID ),
	name		TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL UNIQUE,
	description	TEXT,
	lft		INT	NOT NULL,
	rgt		INT	NOT NULL,
	CONSTRAINT order_check CHECK ( rgt > lft )
);
ALTER SEQUENCE Feature_ID_seq RESTART WITH 1;

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
	ID SERIAL NOT NULL PRIMARY KEY,
	name			TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL UNIQUE,
	webPage			TEXT,
	logo			INT	REFERENCES Picture( ID )
);

-- The list of components
--
CREATE TABLE Component	(
	ID SERIAL NOT NULL PRIMARY KEY,
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

-- Receipes
--
CREATE TABLE Receipe	(
	ID SERIAL NOT NULL PRIMARY KEY,
	name		TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL UNIQUE,
	description	TEXT,
	categoryID	INT	REFERENCES Category( ID ),
	creationDate	TIMESTAMP,
	comment		TEXT
);

CREATE TABLE RecipePicture	(
	receipeID	INT	REFERENCES Receipe( ID ),
	pictureID	INT	REFERENCES Picture( ID ),
	UNIQUE ( receipeID, pictureID )
);

CREATE TABLE ReceipeContent	(
	receipeID	INT	REFERENCES Receipe( ID ),
	categoryID	INT	REFERENCES Category( ID ),
	minQuantity	INT,
	maxQuantity	INT,
	comment		TEXT,
	UNIQUE ( receipeID, categoryID )
);


-- Configuration
--
CREATE TABLE Configuration	(
	ID SERIAL NOT NULL PRIMARY KEY,
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


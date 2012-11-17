
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
	user		TEXT
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


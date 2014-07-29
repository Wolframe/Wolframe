-- SQLite test database creation script
--

-- User Interface
--
DROP TABLE IF EXISTS UIobject;
DROP TABLE IF EXISTS UIobjectName;
DROP TABLE IF EXISTS UIobjectType;
DROP TABLE IF EXISTS Locale;
DROP TABLE IF EXISTS Platform;

-- The platforms tree
--
CREATE TABLE Platform	(
	ID		INTEGER	PRIMARY KEY AUTOINCREMENT,
	parent		INTEGER	REFERENCES Platform( ID ),
	name		TEXT,
	lft		INTEGER	NOT NULL UNIQUE DEFERRABLE CHECK ( lft > 0 ),
	rgt		INTEGER	NOT NULL UNIQUE DEFERRABLE CHECK ( rgt > 1 ),
	CONSTRAINT order_check CHECK ( rgt > lft )
);

INSERT INTO Platform ( ID, parent, name, lft, rgt ) VALUES ( 1, NULL, 'Android', 1, 2 );
INSERT INTO Platform ( ID, parent, name, lft, rgt ) VALUES ( 2, NULL, 'iOS', 3, 4 );
INSERT INTO Platform ( ID, parent, name, lft, rgt ) VALUES ( 3, NULL, 'Windows Mobile', 5, 6 );
INSERT INTO Platform ( ID, parent, name, lft, rgt ) VALUES ( 4, NULL, 'Desktop', 7, 14 );
INSERT INTO Platform ( ID, parent, name, lft, rgt ) VALUES ( 5, 4, 'Linux', 8, 9 );
INSERT INTO Platform ( ID, parent, name, lft, rgt ) VALUES ( 6, 4, 'Windows', 10, 11 );
INSERT INTO Platform ( ID, parent, name, lft, rgt ) VALUES ( 7, 4, 'Mac OS', 12, 13 );

-- Table of locales
--
CREATE TABLE Locale (
	shortName	TEXT	PRIMARY KEY,
	ENname		TEXT,
	localName	TEXT
);

INSERT INTO locale ( shortName, ENname, localName ) VALUES ( 'en', 'English', 'English' );
INSERT INTO locale ( shortName, ENname, localName ) VALUES ( 'en-GB', 'English (United Kingdom)', 'English (United Kingdom)' );
INSERT INTO locale ( shortName, ENname, localName ) VALUES ( 'en-US', 'English (United States)', 'English (United States)' );
INSERT INTO locale ( shortName, ENname, localName ) VALUES ( 'fi', 'Finnish', 'suomi' );
INSERT INTO locale ( shortName, ENname, localName ) VALUES ( 'fi-FI', 'Finnish (Finland)', 'suomi (Suomi)' );
INSERT INTO locale ( shortName, ENname, localName ) VALUES ( 'fr', 'French', 'français' );
INSERT INTO locale ( shortName, ENname, localName ) VALUES ( 'fr-FR', 'French (France)', 'français (France)' );
INSERT INTO locale ( shortName, ENname, localName ) VALUES ( 'fr-CH', 'French (Switzerland)', 'français (Suisse)' );
INSERT INTO locale ( shortName, ENname, localName ) VALUES ( 'de', 'German', 'Deutsch' );
INSERT INTO locale ( shortName, ENname, localName ) VALUES ( 'de-DE', 'German (Germany)', 'Deutsch (Deutschland)' );
INSERT INTO locale ( shortName, ENname, localName ) VALUES ( 'de-LI', 'German (Liechtenstein)', 'Deutsch (Liechtenstein)' );
INSERT INTO locale ( shortName, ENname, localName ) VALUES ( 'de-CH', 'German (Switzerland)', 'Deutsch (Schweiz)' );
INSERT INTO locale ( shortName, ENname, localName ) VALUES ( 'it', 'Italian', 'italiano' );
INSERT INTO locale ( shortName, ENname, localName ) VALUES ( 'it-IT', 'Italian (Italy)', 'italiano (Italia)' );
INSERT INTO locale ( shortName, ENname, localName ) VALUES ( 'it-CH', 'Italian (Switzerland)', 'italiano (Svizzera)' );
INSERT INTO locale ( shortName, ENname, localName ) VALUES ( 'ro', 'Romanian', 'română' );
INSERT INTO locale ( shortName, ENname, localName ) VALUES ( 'ro-RO', 'Romanian (Romania)', 'română (România)' );
INSERT INTO locale ( shortName, ENname, localName ) VALUES ( 'rm', 'Romansh', 'Rumantsch' );
INSERT INTO locale ( shortName, ENname, localName ) VALUES ( 'rm-CH', 'Romansh (Switzerland)', 'Rumantsch (Svizra)' );


CREATE TABLE UIobjectType (
	ID		INTEGER	PRIMARY KEY AUTOINCREMENT,
	typeName	TEXT
);

INSERT INTO UIobjectType ( typeName ) VALUES ( 'Menu' );
INSERT INTO UIobjectType ( typeName ) VALUES ( 'Form' );


CREATE TABLE UIobjectName (
	ID		INTEGER	PRIMARY KEY AUTOINCREMENT,
	ENname		TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL UNIQUE
);

INSERT INTO UIobjectName ( ENname, normalizedName ) VALUES ( 'Pictures Management', 'PICTURES MANAGEMENT' );
INSERT INTO UIobjectName ( ENname, normalizedName ) VALUES ( 'Categories Management', 'CATEGORIES MANAGEMENT' );
INSERT INTO UIobjectName ( ENname, normalizedName ) VALUES ( 'Features Management', 'FEATURES MANAGEMENT' );


CREATE TABLE UIobject (
	ID		INTEGER	REFERENCES UIobjectName( ID ),
	platformID	INTEGER	REFERENCES Platform( ID ),
	typeID		INTEGER	REFERENCES UIobjectType( ID ),
	locale		TEXT	REFERENCES locale( shortName ),
	version		INTEGER,
	body		TEXT
);

INSERT INTO UIobject ( ID, platformID, typeID, locale, version, body )
	VALUES ( 1, 1, 1, 'en', 10000000, 'bla bla' );

-- Authentication
--
DROP TABLE IF EXISTS Password;

CREATE TABLE Password (
	username	TEXT	UNIQUE,
	salt		TEXT,
	pwdHash		TEXT
);


-- Authorization tables
--
DROP TABLE IF EXISTS AuthzMatrix;
DROP TABLE IF EXISTS AuthorizationName;
DROP TABLE IF EXISTS Role;

CREATE TABLE AuthorizationName (
	ID		INTEGER	PRIMARY KEY AUTOINCREMENT,
	name		TEXT	NOT NULL UNIQUE,
	description	TEXT
);

CREATE TABLE Role (
	ID		INTEGER	PRIMARY KEY AUTOINCREMENT,
	name		TEXT	NOT NULL UNIQUE,
	description	TEXT
);

CREATE TABLE AuthzMatrix (
	roleID		INTEGER	REFERENCES Role( ID ),
	authzNameID	INTEGER	REFERENCES AuthorizationName( ID ),
	UNIQUE (roleID, authzNameID)
);


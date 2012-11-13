-- The platforms tree
--
CREATE TABLE Platform	(
	ID		INTEGER	PRIMARY KEY AUTOINCREMENT,
	parent		INTEGER	REFERENCES Platform( ID ),
	name		TEXT,
	lft		INTEGER,
	rgt		INTEGER,
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
	ID		TEXT	PRIMARY KEY,
	ENname		TEXT,
	localName	TEXT
);

INSERT INTO locale ( ID, ENname, localName ) VALUES ( 'en', 'English', 'English' );
INSERT INTO locale ( ID, ENname, localName ) VALUES ( 'en-GB', 'English (United Kingdom)', 'English (United Kingdom)' );
INSERT INTO locale ( ID, ENname, localName ) VALUES ( 'en-US', 'English (United States)', 'English (United States)' );
INSERT INTO locale ( ID, ENname, localName ) VALUES ( 'fi', 'Finnish', 'suomi' );
INSERT INTO locale ( ID, ENname, localName ) VALUES ( 'fi-FI', 'Finnish (Finland)', 'suomi (Suomi)' );
INSERT INTO locale ( ID, ENname, localName ) VALUES ( 'fr', 'French', 'français' );
INSERT INTO locale ( ID, ENname, localName ) VALUES ( 'fr-FR', 'French (France)', 'français (France)' );
INSERT INTO locale ( ID, ENname, localName ) VALUES ( 'fr-CH', 'French (Switzerland)', 'français (Suisse)' );
INSERT INTO locale ( ID, ENname, localName ) VALUES ( 'de', 'German', 'Deutsch' );
INSERT INTO locale ( ID, ENname, localName ) VALUES ( 'de-DE', 'German (Germany)', 'Deutsch (Deutschland)' );
INSERT INTO locale ( ID, ENname, localName ) VALUES ( 'de-LI', 'German (Liechtenstein)', 'Deutsch (Liechtenstein)' );
INSERT INTO locale ( ID, ENname, localName ) VALUES ( 'de-CH', 'German (Switzerland)', 'Deutsch (Schweiz)' );
INSERT INTO locale ( ID, ENname, localName ) VALUES ( 'it', 'Italian', 'italiano' );
INSERT INTO locale ( ID, ENname, localName ) VALUES ( 'it-IT', 'Italian (Italy)', 'italiano (Italia)' );
INSERT INTO locale ( ID, ENname, localName ) VALUES ( 'it-CH', 'Italian (Switzerland)', 'italiano (Svizzera)' );
INSERT INTO locale ( ID, ENname, localName ) VALUES ( 'ro', 'Romanian', 'română' );
INSERT INTO locale ( ID, ENname, localName ) VALUES ( 'ro-RO', 'Romanian (Romania)', 'română (România)' );
INSERT INTO locale ( ID, ENname, localName ) VALUES ( 'rm', 'Romansh', 'Rumantsch' );
INSERT INTO locale ( ID, ENname, localName ) VALUES ( 'rm-CH', 'Romansh (Switzerland)', 'Rumantsch (Svizra)' );


CREATE TABLE UIobjectType (
	ID		INTEGER	PRIMARY KEY AUTOINCREMENT,
	typeName	TEXT
);


CREATE TABLE UIobjectName (
	ID		INTEGER	PRIMARY KEY AUTOINCREMENT,
	ENname		TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL UNIQUE
);


CREATE TABLE UIobject (
	ID		INTEGER	REFERENCES UIobjectName( ID ),
	platformID	INTEGER	REFERENCES Platform( ID ),
	typeID		INTEGER	REFERENCES UIobjectType( ID ),
	locale		TEXT	REFERENCES locale( ID ),
	version		INTEGER,
	body		TEXT
);

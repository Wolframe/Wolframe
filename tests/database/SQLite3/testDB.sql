-- The platforms tree
--
CREATE TABLE Platform	(
	ID		SERIAL	PRIMARY KEY,
	parent		INT	REFERENCES Platform( ID ),
	name		TEXT,
	lft		INT,
	rgt		INT,
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
CREATE TABLE locale (
	ID		TEXT	PRIMARY KEY,
	name		TEXT,
	localName	TEXT
);

INSERT INTO locale ( ID, name, localName ) VALUES ( 'en', 'English', 'English' );
INSERT INTO locale ( ID, name, localName ) VALUES ( 'en-GB', 'English (United Kingdom)', 'English (United Kingdom)' );
INSERT INTO locale ( ID, name, localName ) VALUES ( 'en-US', 'English (United States)', 'English (United States)' );
INSERT INTO locale ( ID, name, localName ) VALUES ( 'fi', 'Finnish', 'suomi' );
INSERT INTO locale ( ID, name, localName ) VALUES ( 'fi-FI', 'Finnish (Finland)', 'suomi (Suomi)' );
INSERT INTO locale ( ID, name, localName ) VALUES ( 'fr', 'French', 'français' );
INSERT INTO locale ( ID, name, localName ) VALUES ( 'fr-FR', 'French (France)', 'français (France)' );
INSERT INTO locale ( ID, name, localName ) VALUES ( 'fr-CH', 'French (Switzerland)', 'français (Suisse)' );
INSERT INTO locale ( ID, name, localName ) VALUES ( 'de', 'German', 'Deutsch' );
INSERT INTO locale ( ID, name, localName ) VALUES ( 'de-DE', 'German (Germany)', 'Deutsch (Deutschland)' );
INSERT INTO locale ( ID, name, localName ) VALUES ( 'de-LI', 'German (Liechtenstein)', 'Deutsch (Liechtenstein)' );
INSERT INTO locale ( ID, name, localName ) VALUES ( 'de-CH', 'German (Switzerland)', 'Deutsch (Schweiz)' );
INSERT INTO locale ( ID, name, localName ) VALUES ( 'it', 'Italian', 'italiano' );
INSERT INTO locale ( ID, name, localName ) VALUES ( 'it-IT', 'Italian (Italy)', 'italiano (Italia)' );
INSERT INTO locale ( ID, name, localName ) VALUES ( 'it-CH', 'Italian (Switzerland)', 'italiano (Svizzera)' );
INSERT INTO locale ( ID, name, localName ) VALUES ( 'ro', 'Romanian', 'română' );
INSERT INTO locale ( ID, name, localName ) VALUES ( 'ro-RO', 'Romanian (Romania)', 'română (România)' );
INSERT INTO locale ( ID, name, localName ) VALUES ( 'rm', 'Romansh', 'Rumantsch' );
INSERT INTO locale ( ID, name, localName ) VALUES ( 'rm-CH', 'Romansh (Switzerland)', 'Rumantsch (Svizra)' );

CREATE TABLE formName (
	ID		SERIAL	PRIMARY KEY,
	name		TEXT	NOT NULL,
	normalizedName	TEXT	NOT NULL UNIQUE
);


CREATE TABLE form (
	ID		INT	REFERENCES formName( ID ),
	Platform	INT	REFERENCES Platform( ID ),
	locale		TEXT	REFERENCES locale( ID ),
	bersion		INT,
	body		TEXT
);

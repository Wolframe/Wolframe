-- Language:
CREATE TABLE Status
(
	id		INTEGER	PRIMARY KEY AUTOINCREMENT,
	name		TEXT
);
INSERT INTO Status (name) VALUES ('living');
INSERT INTO Status (name) VALUES ('dead');

CREATE TABLE Language	(
	id		INTEGER	PRIMARY KEY AUTOINCREMENT,
	parentid	INT	REFERENCES Language( id ),
	name		TEXT,
	description	TEXT,
	status		INT	REFERENCES Status( id ),
	lft INT NOT NULL DEFERRABLE CHECK ( lft > 0 ),
	rgt INT NOT NULL DEFERRABLE CHECK ( rgt > 1 )
);

INSERT INTO Language (parentid,name,description,status,lft,rgt) VALUES (NULL,"invisible root",NULL,NULL,'1','2');


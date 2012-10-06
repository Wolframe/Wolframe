--
-- Generic tree implementation for SQL databases
--

CREATE TABLE tree	(
	ID		SERIAL	PRIMARY KEY,
	parent		INT	REFERENCES tree( ID ),
	lft		INT,
	rgt		INT
	);

CREATE INDEX tree_left_right ON tree( lft, rgt );

-- CREATE TRIGGER

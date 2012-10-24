--
-- The categories tree
--

CREATE TABLE Category	(
	ID		SERIAL	PRIMARY KEY,
	parent		INT	REFERENCES tree( ID ),
	name		TEXT,
	lft		INT,
	rgt		INT,
	CONSTRAINT order_check CHECK ( rgt > lft )
);


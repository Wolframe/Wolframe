--- Wolframe tutorial part 2
---
--- SQLite database

CREATE TABLE Note	(
	CustomerID	INTEGER,
	UserID		INTEGER,
	Note		TEXT,
	FOREIGN KEY( CustomerID ) REFERENCES Customer( CustomerID ),
	FOREIGN KEY( UserID ) REFERENCES User( UserID )
);

--- Wolframe tutorial part 2
---
--- SQLite database

CREATE TABLE Customer	(
	ID		INTEGER PRIMARY KEY AUTOINCREMENT,
	Name		TEXT,
	Canonical_Name	TEXT UNIQUE,
	Address		TEXT,
	Locality	TEXT,
	Country		TEXT
);

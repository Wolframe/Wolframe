--- Wolframe tutorial part 2
---
--- SQLite database

CREATE TABLE User	(
	ID		INTEGER PRIMARY KEY AUTOINCREMENT,
	Username	TEXT UNIQUE,
	Name		TEXT,
	Canonical_Name	TEXT UNIQUE
);

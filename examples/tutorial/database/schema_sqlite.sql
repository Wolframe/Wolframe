-- Customers
--
CREATE TABLE Customer (
	id		INTEGER	PRIMARY KEY AUTOINCREMENT,
	name		TEXT	NOT NULL,
	address		TEXT
);

-- Notes about customers
--
CREATE TABLE Note (
	id		INTEGER	PRIMARY KEY AUTOINCREMENT,
	what		TEXT,
	"when"		TEXT
);

-- Activities
--
CREATE TABLE Activity (
	id		INTEGER PRIMARY KEY AUTOINCREMENT,
	name		TEXT,
	starttime	TEXT,
	endtime		TEXT
);

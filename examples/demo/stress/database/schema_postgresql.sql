--
-- PostgreSQL schema for stress tests
--

CREATE TABLE Number (
    id		SERIAL NOT NULL PRIMARY KEY,
    mul		INTEGER,
    numbr	INTEGER,
    dup		INTEGER,
    trip	INTEGER
);

--
-- Generic tree implementation for SQL databases
-- (Modified Preorder Tree Traversal)
--
-- Note: the parent restriction assumes that references accept NULL
-- Note: if DEFERRED does not work on UNIQUE constraints then the
--       UNIQUE constraint must be dropped
-- Joe Celko example from http://www.ibase.ru/devinfo/DBMSTrees/sqltrees.html

CREATE TABLE Number (
 ID INTEGER PRIMARY KEY AUTOINCREMENT,
 mul INTEGER,
 numbr INTEGER,
 dup INTEGER,
 trip INTEGER
);


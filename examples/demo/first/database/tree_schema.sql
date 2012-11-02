--
-- Generic tree implementation for SQL databases
-- (Modified Preorder Tree Traversal)
--
-- Note: the parent restriction assumes that references accept NULL
-- Note: if DEFERRED does not work on UNIQUE constraints then the
--       UNIQUE constraint must be dropped
-- Joe Celko example from http://www.ibase.ru/devinfo/DBMSTrees/sqltrees.html

CREATE TABLE tree (
 ID SERIAL PRIMARY KEY,
 parent INT REFERENCES tree( ID ),
 name TEXT,
 lft INT NOT NULL UNIQUE DEFERRABLE CHECK ( lft > 0 ),
 rgt INT NOT NULL UNIQUE DEFERRABLE CHECK ( rgt > 1 ),
 CONSTRAINT order_check CHECK ( rgt > lft )
);


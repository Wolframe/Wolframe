--
-- Generic tree implementation for SQL databases
-- (Modified Preorder Tree Traversal)
--
-- Note: the parent restriction assumes that references accept NULL
-- Note: if DEFERRED does not work on UNIQUE constraints then the
--       UNIQUE constraint must be dropped
-- Joe Celko example from http://www.ibase.ru/devinfo/DBMSTrees/sqltrees.html

CREATE TABLE tree (
 ID INTEGER NOT NULL PRIMARY KEY,
 parent INTEGER REFERENCES tree( ID ),
 name VARCHAR(32),
 lft INT NOT NULL CHECK ( lft > 0 ),
 rgt INT NOT NULL CHECK ( rgt > 1 ),
 CONSTRAINT order_check CHECK ( rgt > lft )
);
CREATE SEQUENCE tree_ID_seq START WITH 1 INCREMENT BY 1;
CREATE TRIGGER tree_Insert
BEFORE INSERT ON tree
FOR EACH ROW
BEGIN
	SELECT tree_ID_seq.nextval into :new.id FROM dual;
END;
/

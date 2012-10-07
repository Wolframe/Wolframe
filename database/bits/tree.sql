--
-- Generic tree implementation for SQL databases
--
-- Note: the parent restriction assumes that references accept NULL
-- Note: if DEFERRED does not work on UNIQUE constraints then the
--       UNIQUE constraint must be dropped

CREATE TABLE tree	(
	ID		SERIAL	PRIMARY KEY,
	parent		INT	REFERENCES tree( ID ),
	name		TEXT,
	lft		INT	NOT NULL UNIQUE DEFERRABLE CHECK ( lft > 0 ),
	rgt		INT	NOT NULL UNIQUE DEFERRABLE CHECK ( rgt > 1 ),
	CONSTRAINT order_check CHECK ( rgt > lft )
);

-- Joe Celko example from http://www.ibase.ru/devinfo/DBMSTrees/sqltrees.html

INSERT INTO tree ( ID, parent, name, lft, rgt ) VALUES ( 1, NULL, 'Albert', 1, 12 );
INSERT INTO tree ( ID, parent, name, lft, rgt ) VALUES ( 2, 1, 'Bert', 2, 3 );
INSERT INTO tree ( ID, parent, name, lft, rgt ) VALUES ( 3, 2, 'Chuck', 4, 11 );
INSERT INTO tree ( ID, parent, name, lft, rgt ) VALUES ( 4, 3, 'Donna', 5, 6 );
INSERT INTO tree ( ID, parent, name, lft, rgt ) VALUES ( 5, 3, 'Eddie', 7, 8 );
INSERT INTO tree ( ID, parent, name, lft, rgt ) VALUES ( 6, 3, 'Fred', 9, 10 );


-- Get the node and its parents
SELECT P2.* FROM tree AS P1, tree AS P2
	WHERE P1.lft BETWEEN P2.lft AND P2.rgt
	AND P1.name ='Fred';

-- Get the parents of a node (without node)
SELECT P2.* FROM tree AS P1, tree AS P2
	WHERE P1.lft > P2.lft AND P1.lft < P2.rgt
	AND P1.name ='Fred';

-- Obviously replacing SELECT P2.* with SELECT count( P2.* ) in the above
-- queries will give the level of a node or the levels above the node

-- Get the node and its children
SELECT P1.* FROM tree AS P1, tree AS P2
	WHERE P1.lft BETWEEN P2.lft AND P2.rgt
	AND P2.name ='Albert';

-- Get the children of a node (without node)
SELECT P1.* FROM tree AS P1, tree AS P2
	WHERE P1.lft > P2.lft AND P1.lft < P2.rgt
	AND P2.name ='Albert';


-- Get the levels of the nodes
SELECT count( P2.* ), P1.name FROM tree AS P1, tree AS P2
	WHERE P1.lft BETWEEN P2.lft AND P2.rgt
	GROUP BY P1.name;


-- Insert a node as a child of PARENT in the last position
CREATE FUNCTION addNodeToParent( parentID int, nodeName text )
RETURNS void
AS $$
DECLARE parentRgt	INTEGER;		-- rgt of the parent
BEGIN
parentRgt := ( SELECT rgt FROM tree WHERE ID = parentID );

UPDATE tree SET rgt = CASE
			WHEN rgt >= parentRgt THEN rgt + 2
			ELSE rgt
		END,
		lft = CASE
			WHEN lft > parentRgt THEN lft + 2
			ELSE lft
		END
	WHERE rgt >= parentRgt;

INSERT INTO tree ( parent, name, lft, rgt )
	VALUES ( parentID, nodeName, parentRgt, ( parentRgt + 1 ));
END;
$$ LANGUAGE plpgsql;

-- some tests for node insertion
ALTER SEQUENCE tree_id_seq RESTART WITH 7;
SELECT addNodeToParent( 5, 'test node 0' );
SELECT addNodeToParent( 5, 'test node 1' );
SELECT addNodeToParent( 2, 'test node 2' );
SELECT addNodeToParent( 2, 'test node 3' );
SELECT addNodeToParent( 9, 'test node 4' );


-- Delete a subtree
CREATE FUNCTION deleteSubtree( topNodeID int )
RETURNS void
AS $$
DECLARE
	leftMargin	INTEGER;		-- the left margin of the subtree
	rightMargin	INTEGER;		-- the right margin of the subtree
	width		INTEGER;		-- the total width of the subtree
BEGIN
SELECT lft, rgt INTO leftMargin, rightMargin FROM tree WHERE ID = topNodeID;
width := rightMargin - leftMargin + 1;

DELETE FROM tree WHERE lft BETWEEN leftMargin AND rightMargin;

UPDATE tree SET lft = lft - width WHERE lft > rightMargin;
UPDATE tree SET rgt = rgt - width WHERE rgt > rightMargin;
END;
$$ LANGUAGE plpgsql;

-- delete what was added before
SELECT deleteSubtree( 7 );
SELECT deleteSubtree( 8 );
SELECT deleteSubtree( 9 );
SELECT deleteSubtree( 10 );

-- NOT READY YET
---- Move a subtree
--CREATE FUNCTION moveSubtree( topNodeID int, newParentID int )
--RETURNS void
--AS $$
--DECLARE
--	width		INTEGER;		-- the total width of the subtree
--	newLeft		INTEGER;		-- the final left margin of the subtree
--	tmpLeft		INTEGER;		-- temporary left and right of the subtree
--	tmpRight	INTEGER;
--	distance	INTEGER;		-- the distance the subtree will be moved

--BEGIN
--newLeft := ( SELECT rgt FROM tree WHERE ID = newParentID );
--width := ( SELECT rgt - lft + 1 FROM tree WHERE ID = topNodeID );

--UPDATE tree SET lft = lft + width WHERE lft >= newLeft;
--UPDATE tree SET rgt = rgt + width WHERE rgt >= newLeft;

--SELECT lft, rgt INTO tmpLeft, tmpRight FROM tree WHERE ID = topNodeID;
--distance := newLeft - tmpLeft;
--UPDATE tree SET lft = lft + distance, rgt = rgt + distance
--	WHERE lft >= tmpLeft AND rgt < tmpRight;

--UPDATE tree SET lft = lft - width WHERE lft > tmpLeft;
--UPDATE tree SET rgt = rgt - width WHERE rgt > tmpRight;
--END;
--$$ LANGUAGE plpgsql;

---- move tests
--SELECT moveSubtree( 3, 2 );

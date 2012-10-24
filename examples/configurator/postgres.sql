--
-- The categories tree
--

CREATE TABLE Category	(
	ID		SERIAL	PRIMARY KEY,
	parent		INT	REFERENCES tree( ID ),
	name		TEXT,
	lft		INT	NOT NULL UNIQUE DEFERRABLE CHECK ( lft > 0 ),
	rgt		INT	NOT NULL UNIQUE DEFERRABLE CHECK ( rgt > 1 ),
	CONSTRAINT order_check CHECK ( rgt > lft )
);

-- Insert a category as a subcategory of PARENT
CREATE FUNCTION addCategory( parentID int, name text )
RETURNS void
AS $$
DECLARE parentRgt	INTEGER;		-- rgt of the parent
BEGIN
parentRgt := ( SELECT rgt FROM Category WHERE ID = parentID );

UPDATE Category SET rgt = CASE
			WHEN rgt >= parentRgt THEN rgt + 2
			ELSE rgt
		END,
		lft = CASE
			WHEN lft > parentRgt THEN lft + 2
			ELSE lft
		END
	WHERE rgt >= parentRgt;

INSERT INTO Category ( parent, name, lft, rgt )
	VALUES ( parentID, name, parentRgt, ( parentRgt + 1 ));
END;
$$ LANGUAGE plpgsql;

-- Delete a category and all its subcategories
CREATE FUNCTION deleteCategory( categoryID int )
RETURNS void
AS $$
DECLARE
	leftMargin	INTEGER;		-- the left margin of the subtree
	rightMargin	INTEGER;		-- the right margin of the subtree
	width		INTEGER;		-- the total width of the subtree
BEGIN
SELECT lft, rgt INTO leftMargin, rightMargin FROM Category WHERE ID = categoryID;
width := rightMargin - leftMargin + 1;

DELETE FROM Category WHERE lft BETWEEN leftMargin AND rightMargin;

UPDATE Category SET lft = lft - width WHERE lft > rightMargin;
UPDATE Category SET rgt = rgt - width WHERE rgt > rightMargin;
END;
$$ LANGUAGE plpgsql;


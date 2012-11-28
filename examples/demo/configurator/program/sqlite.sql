-- Select the last ID created in the Picture table
-- PF:HACK: Because we have no variables, we have also to reserve a result for what we whould store in a 'variable'
PREPARE getLastPictureID AS SELECT DISTINCT $1,last_insert_rowid() FROM Picture;

PREPARE getLastCategoryID AS SELECT DISTINCT last_insert_rowid() FROM Category;
PREPARE getLastFeatureID AS SELECT DISTINCT last_insert_rowid() FROM Feature;
PREPARE getLastTagID AS SELECT DISTINCT last_insert_rowid() FROM Tag;

-- for pictures without cats or feats
PREPARE getLastInsertedPictureID AS SELECT DISTINCT last_insert_rowid( ) FROM Picture;

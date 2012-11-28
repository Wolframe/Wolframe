PREPARE getLastCategoryID AS SELECT DISTINCT last_insert_rowid() FROM Category;
PREPARE getLastFeatureID AS SELECT DISTINCT last_insert_rowid() FROM Feature;
PREPARE getLastTagID AS SELECT DISTINCT last_insert_rowid() FROM Tag;
PREPARE getLastPictureID AS SELECT DISTINCT last_insert_rowid( ) FROM Picture;

PREPARE getLastCategoryID AS SELECT currval( pg_get_serial_sequence( 'category', 'id')) AS "ID";
PREPARE getLastFeatureID AS SELECT currval( pg_get_serial_sequence( 'feature', 'id')) AS "ID";
PREPARE getLastTagID AS SELECT currval( pg_get_serial_sequence( 'tag', 'id')) AS "ID";
PREPARE getLastPictureID AS SELECT DISTINCT currval( pg_get_serial_sequence( 'picture', 'id' ) );


-- Select the last ID created in the Picture table
-- PF:HACK: Because we have no variables, we have also to reserve a result for what we whould store in a 'variable'
PREPARE getLastPictureID AS SELECT $1,currval( pg_get_serial_sequence( 'picture', 'id'));

PREPARE getLastCategoryID AS SELECT currval( pg_get_serial_sequence( 'category', 'id')) AS "ID";
PREPARE getLastFeatureID AS SELECT currval( pg_get_serial_sequence( 'feature', 'id')) AS "ID";
PREPARE getLastTagID AS SELECT currval( pg_get_serial_sequence( 'tag', 'id')) AS "ID";

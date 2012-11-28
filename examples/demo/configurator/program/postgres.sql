-- Select the last ID created in the Picture table
-- PF:HACK: Because we have no variables, we have also to reserve a result for what we whould store in a 'variable'
PREPARE getLastPictureID AS SELECT $1,currval( pg_get_serial_sequence( 'picture', 'id'));

-- for pictures without cats or feats
PREPARE getLastInsertedPictureID AS SELECT DISTINCT currval( pg_get_serial_sequence( 'picture', 'id' ) );

PREPARE getLastIdEmployee AS SELECT currval( pg_get_serial_sequence( 'employee', 'id'));
PREPARE getLastIdCustomer AS SELECT currval( pg_get_serial_sequence( 'customer', 'id'));
PREPARE getLastIdProject AS SELECT currval( pg_get_serial_sequence( 'project', 'id'));
PREPARE getLastIdActivity AS SELECT currval( pg_get_serial_sequence( 'activity', 'id'));
PREPARE getLastIdActivityCategory AS SELECT currval( pg_get_serial_sequence( 'activitycategory', 'id'));
PREPARE getLastIdActivityNote AS SELECT currval( pg_get_serial_sequence( 'activitynote', 'id'));
PREPARE getLastIdActivityNote AS SELECT currval( pg_get_serial_sequence( 'activitynote', 'id'));
PREPARE getLastIdCustomerNote AS SELECT currval( pg_get_serial_sequence( 'customernote', 'id'));
PREPARE getLastIdProjectNote AS SELECT currval( pg_get_serial_sequence( 'projectnote', 'id'));


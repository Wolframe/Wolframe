STATEMENT get_task SELECT * FROM task WHERE id=$1;

TRANSACTION test_transaction
BEGIN
	INTO doc/task WITH //task DO get_task( id);
END


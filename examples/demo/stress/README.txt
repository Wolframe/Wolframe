Stress test of wolframed with cmdline client:

1) Create test document collection:
Run the script scripts/createRequests.pl (without arguments) to create the test documents in request/insert, request/update, request/select, request/delete

2) Create test database:
Example:
	sqlite3 -init database/schema_sqlite.sql


3) Run parallel tests:
Example:
	foreach ff in `ls request/insert/*`; do wolframec -d $ff; done && \
	foreach ff in `ls request/select/*`; do wolframec -d $ff; done


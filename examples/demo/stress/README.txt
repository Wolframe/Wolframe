Stress test of wolframed with cmdline client:

1) Create test document collection:

   Create the requests directories (mkdir -p
   request/{insert,update,select,delete}) if necessary
   Run the script scripts/createRequests.pl (without arguments) to create
   the test documents in request/insert, request/update, request/select,
   request/delete

2) Create test database:

   sqlite3 database/stress.db < database/schema_sqlite.sql

3) Start server:

   wolframed -f -c wolframe.conf

4) Insert the data:
 
   for ff in `ls request/insert/*`; do wolframec -d $ff; done

5) Run parallel tests from different shells:

   shell 1> for ff in `ls request/select/*`; do wolframec -d $ff; done
   shell 2> for ff in `ls request/select/*`; do wolframec -d $ff; done
   shell 3> for ff in `ls request/select/*`; do wolframec -d $ff; done


**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<root><aa>1</aa></root>
**config
--module ../../src/modules/filter/textwolf/mod_filter_textwolf
--module ../wolfilter/modules/database/testtrace/mod_db_testtrace
--module ../../src/modules/transaction/preparedStatement/mod_transaction_pstm
--input-filter 'xml:textwolf'
--database 'id=testdb,outfile=DBOUT'
--transaction 'testcall dbpstm run(//aa: .)'
testcall
**outputfile:DBOUT
**output
run #1

start( 'run' );
bind( 1, '1' );
execute();
nofColumns(); returns 0
**end

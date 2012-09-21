**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<aa>1</aa>
**config
--module ../../src/modules/filter/textwolf/mod_filter_textwolf
--module ../wolfilter/modules/database/testtrace/mod_db_testtrace
--module ../../src/modules/transaction/preparedStatement/mod_transaction_pstm
--input-filter 'xml:textwolf'
--database 'id=testdb,outfile=DBOUT,file=DBRES'
--transaction 'testcall dbpstm result=run(: /aa)'
testcall
**file: DBRES
#id#1
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<result><id>1</id></result>
begin();
start( 'run' );
bind( 1, '1' );
execute();
nofColumns(); returns 1
get( 1 ); returns 1
next(); returns 0
getLastError(); returns 0
nofColumns(); returns 1
columnName( 1 ); returns id
commit();
**end

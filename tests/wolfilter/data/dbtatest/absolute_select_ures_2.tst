**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<aa>1</aa><bb>2</bb>
**config
--module ../../src/modules/filter/textwolf/mod_filter_textwolf
--module ../wolfilter/modules/database/testtrace/mod_db_testtrace
--module ../../src/modules/transaction/preparedStatement/mod_transaction_pstm
--input-filter 'xml:textwolf'
--database 'id=testdb,outfile=DBOUT,file=DBRES'
--transaction 'testcall dbpstm result/item=run( : /aa , /bb ); exec ( : $1,$2)'
testcall
**file: DBRES
#id name#1 hugo#2 barbara
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<result><item><id>1</id><name>hugo</name></item><item><id>2</id><name>barbara</name></item></result>
run #1#2
exec #[1]#[2]

start( 'run' );
bind( 1, '1' );
bind( 2, '2' );
execute();
nofColumns(); returns 2
columnName( 1); returns id
columnName( 2); returns name
get( 1 ); returns 1
get( 2 ); returns hugo
next(); returns 1
get( 1 ); returns 2
get( 2 ); returns barbara
next(); returns 0
start( 'exec' );
bind( 1, '1' );
bind( 2, 'hugo' );
execute();
nofColumns(); returns 0
bind( 1, '2' );
bind( 2, 'barbara' );
execute();
nofColumns(); returns 0
**end

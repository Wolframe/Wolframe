**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<aa>1</aa><bb>2</bb>
**config
--config wolframe.conf --filter textwolf testcall
**file:wolframe.conf
LoadModules
{
	module ../../src/modules/filter/textwolf/mod_filter_textwolf
	module ../wolfilter/modules/database/testtrace/mod_db_testtrace
}
Database
{
	test
	{
		identifier testdb
		outfile DBOUT
		file DBRES
	}
}
Processor
{
	database testdb
	program DBIN.tdl
}
**file:DBIN.tdl
TRANSACTION testcall
RESULT INTO result
BEGIN
	INTO item DO SELECT run( $(/aa) , $(/bb) );
	FOREACH RESULT DO SELECT exec ( $1,$2);
END
**file: DBRES
#id name#1 hugo#2 barbara
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<result><item><id>1</id><name>hugo</name></item><item><id>2</id><name>barbara</name></item></result>
Code:
[0] GOTO @1
[1] OUTPUT_OPEN TAG result
[2] RESULT_SET_INIT
[3] DBSTM_START STM (SELECT run( $1 , $2 ))
[4] DBSTM_BIND_CONST CONST '1'
[5] DBSTM_BIND_CONST CONST '2'
[6] DBSTM_EXEC
[7] OPEN_ITER_LAST_RESULT
[8] NOT_IF_COND GOTO @14
[9] OUTPUT_OPEN TAG item
[10] OUTPUT_ITR_COLUMN
[11] OUTPUT_CLOSE
[12] NEXT
[13] IF_COND GOTO @9
[14] RESULT_SET_INIT
[15] OPEN_ITER_LAST_RESULT
[16] NOT_IF_COND GOTO @23
[17] DBSTM_START STM (SELECT exec ( $1,$2))
[18] DBSTM_BIND_ITR_IDX COLIDX 1
[19] DBSTM_BIND_ITR_IDX COLIDX 2
[20] DBSTM_EXEC
[21] NEXT
[22] IF_COND GOTO @17
[23] OUTPUT_CLOSE
[24] RETURN
start( 'SELECT run( $1 , $2 )' );
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
start( 'SELECT exec ( $1,$2)' );
bind( 1, '1' );
bind( 2, 'hugo' );
execute();
start( 'SELECT exec ( $1,$2)' );
bind( 1, '2' );
bind( 2, 'barbara' );
execute();
**end

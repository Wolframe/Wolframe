**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><item></item><item><aa>1</aa><bb>2</bb></item><item><aa>11</aa><bb>22</bb></item><item><aa>111</aa><bb>222</bb></item><item><bb></bb></item></doc>
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
RESULT INTO da
BEGIN
	FOREACH /doc/item/aa DO SELECT run($(.) ,$(../bb) ) ;
	FOREACH RESULT INTO da DO SELECT get($1);
	FOREACH RESULT INTO do DO SELECT run( $1 );
END
**file: DBRES
#res#1
#res#2
#res#3
#res#a
#res#b
#res#c
#res#111
#res#222
#res#333
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<da><da><res>a</res></da><da><res>b</res></da><da><res>c</res></da><do><res>111</res></do><do><res>222</res></do><do><res>333</res></do></da>
Code:
[0] OUTPUT_OPEN TAG da
[1] RESULT_SET_INIT
[2] OPEN_ITER_TUPLESET TUPLESET 0
[3] NOT_IF_COND GOTO @10
[4] DBSTM_START STM (SELECT run($1 ,$2 ) )
[5] DBSTM_BIND_ITR_IDX COLIDX 1
[6] DBSTM_BIND_ITR_IDX COLIDX 2
[7] DBSTM_EXEC
[8] NEXT
[9] IF_COND GOTO @4
[10] RESULT_SET_INIT
[11] OPEN_ITER_LAST_RESULT
[12] NOT_IF_COND GOTO @18
[13] DBSTM_START STM (SELECT get($1))
[14] DBSTM_BIND_ITR_IDX COLIDX 1
[15] DBSTM_EXEC
[16] NEXT
[17] IF_COND GOTO @13
[18] OPEN_ITER_LAST_RESULT
[19] NOT_IF_COND GOTO @25
[20] OUTPUT_OPEN TAG da
[21] OUTPUT_ITR_COLUMN
[22] OUTPUT_CLOSE
[23] NEXT
[24] IF_COND GOTO @20
[25] RESULT_SET_INIT
[26] OPEN_ITER_LAST_RESULT
[27] NOT_IF_COND GOTO @33
[28] DBSTM_START STM (SELECT run( $1 ))
[29] DBSTM_BIND_ITR_IDX COLIDX 1
[30] DBSTM_EXEC
[31] NEXT
[32] IF_COND GOTO @28
[33] OPEN_ITER_LAST_RESULT
[34] NOT_IF_COND GOTO @40
[35] OUTPUT_OPEN TAG do
[36] OUTPUT_ITR_COLUMN
[37] OUTPUT_CLOSE
[38] NEXT
[39] IF_COND GOTO @35
[40] OUTPUT_CLOSE
[41] RETURN
Input Data:
SET 0: ., bb
  '1', '2'
  '11', '22'
  '111', '222'
start( 'SELECT run($1 ,$2 ) ' );
bind( 1, '1' );
bind( 2, '2' );
execute();
nofColumns(); returns 1
columnName( 1); returns res
get( 1 ); returns 1
next(); returns 0
start( 'SELECT run($1 ,$2 ) ' );
bind( 1, '11' );
bind( 2, '22' );
execute();
nofColumns(); returns 1
columnName( 1); returns res
get( 1 ); returns 2
next(); returns 0
start( 'SELECT run($1 ,$2 ) ' );
bind( 1, '111' );
bind( 2, '222' );
execute();
nofColumns(); returns 1
columnName( 1); returns res
get( 1 ); returns 3
next(); returns 0
start( 'SELECT get($1)' );
bind( 1, '1' );
execute();
nofColumns(); returns 1
columnName( 1); returns res
get( 1 ); returns a
next(); returns 0
start( 'SELECT get($1)' );
bind( 1, '2' );
execute();
nofColumns(); returns 1
columnName( 1); returns res
get( 1 ); returns b
next(); returns 0
start( 'SELECT get($1)' );
bind( 1, '3' );
execute();
nofColumns(); returns 1
columnName( 1); returns res
get( 1 ); returns c
next(); returns 0
start( 'SELECT run( $1 )' );
bind( 1, 'a' );
execute();
nofColumns(); returns 1
columnName( 1); returns res
get( 1 ); returns 111
next(); returns 0
start( 'SELECT run( $1 )' );
bind( 1, 'b' );
execute();
nofColumns(); returns 1
columnName( 1); returns res
get( 1 ); returns 222
next(); returns 0
start( 'SELECT run( $1 )' );
bind( 1, 'c' );
execute();
nofColumns(); returns 1
columnName( 1); returns res
get( 1 ); returns 333
next(); returns 0
**end

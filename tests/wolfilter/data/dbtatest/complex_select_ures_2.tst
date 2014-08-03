**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<root><doc><item></item><item><aa>1</aa><bb>2</bb></item><item><aa>11</aa><bb>22</bb></item><item><aa>111</aa><bb>222</bb></item><item><bb></bb></item></doc></root>
**config
--config wolframe.conf --filter textwolf testcall
**file:wolframe.conf
LoadModules
{
	module ../../src/modules/filter/textwolf/mod_filter_textwolf
<<<<<<< HEAD
=======
	module ../../src/modules/cmdbind/tdl/mod_command_tdl
>>>>>>> 974f42f509317b90fc22b74f21479acc18fabb3e
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
BEGIN
INTO da
BEGIN
	FOREACH /doc/item/aa DO SELECT run($(.) ,$(../bb) ) ;
	FOREACH RESULT INTO da DO SELECT get($1);
	FOREACH RESULT INTO do DO SELECT run( $1 );
END
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
<root><da><da><res>a</res></da><da><res>b</res></da><da><res>c</res></da><do><res>111</res></do><do><res>222</res></do><do><res>333</res></do></da></root>
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
[18] OUTPUT_OPEN_ARRAY TAG da
[19] OPEN_ITER_LAST_RESULT
[20] NOT_IF_COND GOTO @26
[21] OUTPUT_OPEN_ELEM
[22] OUTPUT_ITR_COLUMN
[23] OUTPUT_CLOSE_ELEM
[24] NEXT
[25] IF_COND GOTO @21
[26] OUTPUT_CLOSE_ARRAY
[27] RESULT_SET_INIT
[28] OPEN_ITER_LAST_RESULT
[29] NOT_IF_COND GOTO @35
[30] DBSTM_START STM (SELECT run( $1 ))
[31] DBSTM_BIND_ITR_IDX COLIDX 1
[32] DBSTM_EXEC
[33] NEXT
[34] IF_COND GOTO @30
[35] OUTPUT_OPEN_ARRAY TAG do
[36] OPEN_ITER_LAST_RESULT
[37] NOT_IF_COND GOTO @43
[38] OUTPUT_OPEN_ELEM
[39] OUTPUT_ITR_COLUMN
[40] OUTPUT_CLOSE_ELEM
[41] NEXT
[42] IF_COND GOTO @38
[43] OUTPUT_CLOSE_ARRAY
[44] OUTPUT_CLOSE
[45] RETURN
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

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
[0] GOTO @1
[1] OUTPUT_OPEN TAG da
[2] RESULT_SET_INIT
[3] OPEN_ITER_TUPLESET TUPLESET 0
[4] NOT_IF_COND GOTO @11
[5] DBSTM_START STM (SELECT run($1 ,$2 ) )
[6] DBSTM_BIND_ITR_IDX COLIDX 1
[7] DBSTM_BIND_ITR_IDX COLIDX 2
[8] DBSTM_EXEC
[9] NEXT
[10] IF_COND GOTO @5
[11] RESULT_SET_INIT
[12] OPEN_ITER_LAST_RESULT
[13] NOT_IF_COND GOTO @19
[14] DBSTM_START STM (SELECT get($1))
[15] DBSTM_BIND_ITR_IDX COLIDX 1
[16] DBSTM_EXEC
[17] NEXT
[18] IF_COND GOTO @14
[19] OPEN_ITER_LAST_RESULT
[20] NOT_IF_COND GOTO @26
[21] OUTPUT_OPEN TAG da
[22] OUTPUT_ITR_COLUMN
[23] OUTPUT_CLOSE
[24] NEXT
[25] IF_COND GOTO @21
[26] RESULT_SET_INIT
[27] OPEN_ITER_LAST_RESULT
[28] NOT_IF_COND GOTO @34
[29] DBSTM_START STM (SELECT run( $1 ))
[30] DBSTM_BIND_ITR_IDX COLIDX 1
[31] DBSTM_EXEC
[32] NEXT
[33] IF_COND GOTO @29
[34] OPEN_ITER_LAST_RESULT
[35] NOT_IF_COND GOTO @41
[36] OUTPUT_OPEN TAG do
[37] OUTPUT_ITR_COLUMN
[38] OUTPUT_CLOSE
[39] NEXT
[40] IF_COND GOTO @36
[41] OUTPUT_CLOSE
[42] RETURN
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

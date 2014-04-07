**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<root><item><aa>1</aa></item><item></item><item><bb></bb></item></root>
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
TRANSACTION testcall RESULT INTO result BEGIN
	FOREACH //aa DO SELECT run($(.));
	FOREACH RESULT INTO person DO SELECT call( $(//aa) , $1 ) ;
	FOREACH RESULT INTO age DO SELECT get ( $(//aa),$1 ,$2 ,$3);
END
**file: DBRES
#res#1#2
#vorname name beruf#karin fischer beamte
#vorname name beruf#peter mueller polizist
#alter#42
#alter#31
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<result><person><vorname>karin</vorname><name>fischer</name><beruf>beamte</beruf></person><person><vorname>peter</vorname><name>mueller</name><beruf>polizist</beruf></person><age><alter>42</alter></age><age><alter>31</alter></age></result>
Code:
[0] GOTO @1
[1] OUTPUT_OPEN TAG result
[2] RESULT_SET_INIT
[3] OPEN_ITER_TUPLESET TUPLESET 0
[4] NOT_IF_COND GOTO @10
[5] DBSTM_START STM (SELECT run($1))
[6] DBSTM_BIND_ITR_IDX COLIDX 1
[7] DBSTM_EXEC
[8] NEXT
[9] IF_COND GOTO @5
[10] RESULT_SET_INIT
[11] OPEN_ITER_LAST_RESULT
[12] NOT_IF_COND GOTO @19
[13] DBSTM_START STM (SELECT call( $1 , $2 ) )
[14] DBSTM_BIND_CONST CONST '1'
[15] DBSTM_BIND_ITR_IDX COLIDX 1
[16] DBSTM_EXEC
[17] NEXT
[18] IF_COND GOTO @13
[19] OPEN_ITER_LAST_RESULT
[20] NOT_IF_COND GOTO @26
[21] OUTPUT_OPEN TAG person
[22] OUTPUT_ITR_COLUMN
[23] OUTPUT_CLOSE
[24] NEXT
[25] IF_COND GOTO @21
[26] RESULT_SET_INIT
[27] OPEN_ITER_LAST_RESULT
[28] NOT_IF_COND GOTO @37
[29] DBSTM_START STM (SELECT get ( $1,$2 ,$3 ,$4))
[30] DBSTM_BIND_CONST CONST '1'
[31] DBSTM_BIND_ITR_IDX COLIDX 1
[32] DBSTM_BIND_ITR_IDX COLIDX 2
[33] DBSTM_BIND_ITR_IDX COLIDX 3
[34] DBSTM_EXEC
[35] NEXT
[36] IF_COND GOTO @29
[37] OPEN_ITER_LAST_RESULT
[38] NOT_IF_COND GOTO @44
[39] OUTPUT_OPEN TAG age
[40] OUTPUT_ITR_COLUMN
[41] OUTPUT_CLOSE
[42] NEXT
[43] IF_COND GOTO @39
[44] OUTPUT_CLOSE
[45] RETURN
Input Data:
SET 0: .
  '1'
start( 'SELECT run($1)' );
bind( 1, '1' );
execute();
nofColumns(); returns 1
columnName( 1); returns res
get( 1 ); returns 1
next(); returns 1
get( 1 ); returns 2
next(); returns 0
start( 'SELECT call( $1 , $2 ) ' );
bind( 1, '1' );
bind( 2, '1' );
execute();
nofColumns(); returns 3
columnName( 1); returns vorname
columnName( 2); returns name
columnName( 3); returns beruf
get( 1 ); returns karin
get( 2 ); returns fischer
get( 3 ); returns beamte
next(); returns 0
start( 'SELECT call( $1 , $2 ) ' );
bind( 1, '1' );
bind( 2, '2' );
execute();
nofColumns(); returns 3
columnName( 1); returns vorname
columnName( 2); returns name
columnName( 3); returns beruf
get( 1 ); returns peter
get( 2 ); returns mueller
get( 3 ); returns polizist
next(); returns 0
start( 'SELECT get ( $1,$2 ,$3 ,$4)' );
bind( 1, '1' );
bind( 2, 'karin' );
bind( 3, 'fischer' );
bind( 4, 'beamte' );
execute();
nofColumns(); returns 1
columnName( 1); returns alter
get( 1 ); returns 42
next(); returns 0
start( 'SELECT get ( $1,$2 ,$3 ,$4)' );
bind( 1, '1' );
bind( 2, 'peter' );
bind( 3, 'mueller' );
bind( 4, 'polizist' );
execute();
nofColumns(); returns 1
columnName( 1); returns alter
get( 1 ); returns 31
next(); returns 0
**end

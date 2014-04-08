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
[0] OUTPUT_OPEN TAG result
[1] RESULT_SET_INIT
[2] OPEN_ITER_TUPLESET TUPLESET 0
[3] NOT_IF_COND GOTO @9
[4] DBSTM_START STM (SELECT run($1))
[5] DBSTM_BIND_ITR_IDX COLIDX 1
[6] DBSTM_EXEC
[7] NEXT
[8] IF_COND GOTO @4
[9] RESULT_SET_INIT
[10] OPEN_ITER_LAST_RESULT
[11] NOT_IF_COND GOTO @18
[12] DBSTM_START STM (SELECT call( $1 , $2 ) )
[13] DBSTM_BIND_CONST CONST '1'
[14] DBSTM_BIND_ITR_IDX COLIDX 1
[15] DBSTM_EXEC
[16] NEXT
[17] IF_COND GOTO @12
[18] OPEN_ITER_LAST_RESULT
[19] NOT_IF_COND GOTO @25
[20] OUTPUT_OPEN TAG person
[21] OUTPUT_ITR_COLUMN
[22] OUTPUT_CLOSE
[23] NEXT
[24] IF_COND GOTO @20
[25] RESULT_SET_INIT
[26] OPEN_ITER_LAST_RESULT
[27] NOT_IF_COND GOTO @36
[28] DBSTM_START STM (SELECT get ( $1,$2 ,$3 ,$4))
[29] DBSTM_BIND_CONST CONST '1'
[30] DBSTM_BIND_ITR_IDX COLIDX 1
[31] DBSTM_BIND_ITR_IDX COLIDX 2
[32] DBSTM_BIND_ITR_IDX COLIDX 3
[33] DBSTM_EXEC
[34] NEXT
[35] IF_COND GOTO @28
[36] OPEN_ITER_LAST_RESULT
[37] NOT_IF_COND GOTO @43
[38] OUTPUT_OPEN TAG age
[39] OUTPUT_ITR_COLUMN
[40] OUTPUT_CLOSE
[41] NEXT
[42] IF_COND GOTO @38
[43] OUTPUT_CLOSE
[44] RETURN
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

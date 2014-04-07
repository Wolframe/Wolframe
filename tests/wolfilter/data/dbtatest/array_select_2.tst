**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<root><item><aa><xx>1</xx><yy>1</yy></aa></item><item><aa><xx>2</xx></aa></item><item><aa><xx>3</xx><yy>3</yy></aa></item></root>
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
	}
}
Processor
{
	database testdb
	program DBIN.tdl
}
**file:DBIN.tdl
TRANSACTION testcall
BEGIN FOREACH //aa DO SELECT run( $(xx), $(yy));
END
**outputfile:DBOUT
**output
Code:
[0] GOTO @1
[1] RESULT_SET_INIT
[2] OPEN_ITER_TUPLESET TUPLESET 0
[3] NOT_IF_COND GOTO @10
[4] DBSTM_START STM (SELECT run( $1, $2))
[5] DBSTM_BIND_ITR_IDX COLIDX 1
[6] DBSTM_BIND_ITR_IDX COLIDX 2
[7] DBSTM_EXEC
[8] NEXT
[9] IF_COND GOTO @4
[10] RETURN
Input Data:
SET 0: xx, yy
  '1', '1'
  '2', NULL
  '3', '3'
start( 'SELECT run( $1, $2)' );
bind( 1, '1' );
bind( 2, '1' );
execute();
start( 'SELECT run( $1, $2)' );
bind( 1, '2' );
bind( 2, NULL );
execute();
start( 'SELECT run( $1, $2)' );
bind( 1, '3' );
bind( 2, '3' );
execute();
**end

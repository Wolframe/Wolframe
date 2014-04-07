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
	}
}
Processor
{
	database testdb
	program DBIN.tdl
}
**file:DBIN.tdl
TRANSACTION testcall BEGIN FOREACH/doc/item/aa DO SELECT run($(.), $(../bb));END
**outputfile:DBOUT
**output
Code:
[0] GOTO @1
[1] RESULT_SET_INIT
[2] OPEN_ITER_TUPLESET TUPLESET 0
[3] NOT_IF_COND GOTO @10
[4] DBSTM_START STM (SELECT run($1, $2))
[5] DBSTM_BIND_ITR_IDX COLIDX 1
[6] DBSTM_BIND_ITR_IDX COLIDX 2
[7] DBSTM_EXEC
[8] NEXT
[9] IF_COND GOTO @4
[10] RETURN
Input Data:
SET 0: ., bb
  '1', '2'
  '11', '22'
  '111', '222'
start( 'SELECT run($1, $2)' );
bind( 1, '1' );
bind( 2, '2' );
execute();
start( 'SELECT run($1, $2)' );
bind( 1, '11' );
bind( 2, '22' );
execute();
start( 'SELECT run($1, $2)' );
bind( 1, '111' );
bind( 2, '222' );
execute();
**end

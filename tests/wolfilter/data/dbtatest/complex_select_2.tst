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
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<root/>
Code:
[0] RESULT_SET_INIT
[1] OPEN_ITER_TUPLESET TUPLESET 0
[2] NOT_IF_COND GOTO @9
[3] DBSTM_START STM (SELECT run($1, $2))
[4] DBSTM_BIND_ITR_IDX COLIDX 1
[5] DBSTM_BIND_ITR_IDX COLIDX 2
[6] DBSTM_EXEC
[7] NEXT
[8] IF_COND GOTO @3
[9] RETURN
Input Data:
SET 0: ., bb
  '1', '2'
  '11', '22'
  '111', '222'
start( 'SELECT run($1, $2)' );
bind( 1, '1' );
bind( 2, '2' );
execute();
nofColumns(); returns 0
start( 'SELECT run($1, $2)' );
bind( 1, '11' );
bind( 2, '22' );
execute();
nofColumns(); returns 0
start( 'SELECT run($1, $2)' );
bind( 1, '111' );
bind( 2, '222' );
execute();
nofColumns(); returns 0
**end

**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<main><item><aa>1</aa><cc>3</cc><bb>2</bb><item><item><aa>11</aa><cc>33</cc><bb>22</bb><item><item><aa>111</aa><cc>333</cc><bb>222</bb><item></main>
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
TRANSACTION testcall BEGIN
	FOREACH //aa DO SELECT run ($(.), $(../bb), $(../aa/../cc));
END
**outputfile:DBOUT
**output
Code:
[0] GOTO @1
[1] RESULT_SET_INIT
[2] OPEN_ITER_TUPLESET TUPLESET 0
[3] NOT_IF_COND GOTO @11
[4] DBSTM_START STM (SELECT run ($1, $2, $3))
[5] DBSTM_BIND_ITR_IDX COLIDX 1
[6] DBSTM_BIND_ITR_IDX COLIDX 2
[7] DBSTM_BIND_ITR_IDX COLIDX 3
[8] DBSTM_EXEC
[9] NEXT
[10] IF_COND GOTO @4
[11] RETURN
Input Data:
SET 0: ., bb, cc
  '1', '2', '3'
  '11', '22', '33'
  '111', '222', '333'
start( 'SELECT run ($1, $2, $3)' );
bind( 1, '1' );
bind( 2, '2' );
bind( 3, '3' );
execute();
start( 'SELECT run ($1, $2, $3)' );
bind( 1, '11' );
bind( 2, '22' );
bind( 3, '33' );
execute();
start( 'SELECT run ($1, $2, $3)' );
bind( 1, '111' );
bind( 2, '222' );
bind( 3, '333' );
execute();
**end

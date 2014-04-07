**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<root><aa>1</aa></root>
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
	FOREACH //aa DO SELECT run($(.));
END
**outputfile:DBOUT
**output
Code:
[0] GOTO @1
[1] RESULT_SET_INIT
[2] OPEN_ITER_TUPLESET TUPLESET 0
[3] NOT_IF_COND GOTO @9
[4] DBSTM_START STM (SELECT run($1))
[5] DBSTM_BIND_ITR_IDX COLIDX 1
[6] DBSTM_EXEC
[7] NEXT
[8] IF_COND GOTO @4
[9] RETURN
Input Data:
SET 0: .
  '1'
start( 'SELECT run($1)' );
bind( 1, '1' );
execute();
**end

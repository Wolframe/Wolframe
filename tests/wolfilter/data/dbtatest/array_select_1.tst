**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><root><item><aa>1</aa><aa>2</aa></item><item></item><item><aa>1</aa><aa>2</aa><aa>3</aa></item></root></doc>
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
BEGIN FOREACH //aa DO SELECT run($(.));
END
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc/>
Code:
[0] RESULT_SET_INIT
[1] OPEN_ITER_TUPLESET TUPLESET 0
[2] NOT_IF_COND GOTO @8
[3] DBSTM_START STM (SELECT run($1))
[4] DBSTM_BIND_ITR_IDX COLIDX 1
[5] DBSTM_EXEC
[6] NEXT
[7] IF_COND GOTO @3
[8] RETURN
Input Data:
SET 0: .
  '1'
  '2'
  '1'
  '2'
  '3'
start( 'SELECT run($1)' );
bind( 1, '1' );
execute();
nofColumns(); returns 0
start( 'SELECT run($1)' );
bind( 1, '2' );
execute();
nofColumns(); returns 0
start( 'SELECT run($1)' );
bind( 1, '1' );
execute();
nofColumns(); returns 0
start( 'SELECT run($1)' );
bind( 1, '2' );
execute();
nofColumns(); returns 0
start( 'SELECT run($1)' );
bind( 1, '3' );
execute();
nofColumns(); returns 0
**end

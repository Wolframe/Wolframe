**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><root><item><aa>1</aa></item><item></item><item><bb></bb></item></root></doc>
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
	}
}
Processor
{
	database testdb
	program DBIN.tdl
}
**file:DBIN.tdl
TRANSACTION testcall
BEGIN FOREACH //aa DO SELECT run($(../aa), $(/root/item/aa));
END
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc/>
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
SET 0: aa, aa
  '1', '1'
start( 'SELECT run($1, $2)' );
bind( 1, '1' );
bind( 2, '1' );
execute();
nofColumns(); returns 0
**end

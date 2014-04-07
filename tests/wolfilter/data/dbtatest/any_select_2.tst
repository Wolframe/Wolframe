**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><item><aa>1</aa><cc>3</cc><bb>2</bb><dd></dd></item><item><aa>11</aa><cc>33</cc><bb></bb><dd>44</dd></item></doc>
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
BEGIN
	FOREACH /doc/*/aa DO SELECT run($(../*/../aa), $(../bb), $(../cc), $(..//dd) ) ;
END
**outputfile:DBOUT
**output
Code:
[0] GOTO @1
[1] RESULT_SET_INIT
[2] OPEN_ITER_TUPLESET TUPLESET 0
[3] NOT_IF_COND GOTO @12
[4] DBSTM_START STM (SELECT run($1, $2, $3, $4 ) )
[5] DBSTM_BIND_ITR_IDX COLIDX 1
[6] DBSTM_BIND_ITR_IDX COLIDX 2
[7] DBSTM_BIND_ITR_IDX COLIDX 3
[8] DBSTM_BIND_ITR_IDX COLIDX 4
[9] DBSTM_EXEC
[10] NEXT
[11] IF_COND GOTO @4
[12] RETURN
Input Data:
SET 0: aa, bb, cc, dd
  '1', '2', '3', NULL
  '11', NULL, '33', '44'
start( 'SELECT run($1, $2, $3, $4 ) ' );
bind( 1, '1' );
bind( 2, '2' );
bind( 3, '3' );
bind( 4, NULL );
execute();
start( 'SELECT run($1, $2, $3, $4 ) ' );
bind( 1, '11' );
bind( 2, NULL );
bind( 3, '33' );
bind( 4, '44' );
execute();
**end

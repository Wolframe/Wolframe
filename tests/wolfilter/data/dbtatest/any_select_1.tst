**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<root><doc><item><aa>1</aa><cc>3</cc><bb>2</bb><dd></dd></item><item><aa>11</aa><cc>33</cc><bb></bb><dd>44</dd></item></doc></root>
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
	FOREACH //*/aa DO SELECT run($(.), $(../*/../bb), $(../cc), $(../*/..//dd));
END
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<root/>
Code:
[0] RESULT_SET_INIT
[1] OPEN_ITER_TUPLESET TUPLESET 0
[2] NOT_IF_COND GOTO @11
[3] DBSTM_START STM (SELECT run($1, $2, $3, $4))
[4] DBSTM_BIND_ITR_IDX COLIDX 1
[5] DBSTM_BIND_ITR_IDX COLIDX 2
[6] DBSTM_BIND_ITR_IDX COLIDX 3
[7] DBSTM_BIND_ITR_IDX COLIDX 4
[8] DBSTM_EXEC
[9] NEXT
[10] IF_COND GOTO @3
[11] RETURN
Input Data:
SET 0: ., bb, cc, dd
  '1', '2', '3', NULL
  '11', NULL, '33', '44'
start( 'SELECT run($1, $2, $3, $4)' );
bind( 1, '1' );
bind( 2, '2' );
bind( 3, '3' );
bind( 4, NULL );
execute();
nofColumns(); returns 0
start( 'SELECT run($1, $2, $3, $4)' );
bind( 1, '11' );
bind( 2, NULL );
bind( 3, '33' );
bind( 4, '44' );
execute();
nofColumns(); returns 0
**end

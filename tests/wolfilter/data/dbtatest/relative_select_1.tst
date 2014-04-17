**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<guru>ein parameter text</guru>
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
	DO SELECT run( $(guru));
END
**outputfile:DBOUT
**output
Code:
[0] RESULT_SET_INIT
[1] DBSTM_START STM (SELECT run( $1))
[2] DBSTM_BIND_CONST CONST 'ein parameter text'
[3] DBSTM_EXEC
[4] RETURN
start( 'SELECT run( $1)' );
bind( 1, 'ein parameter text' );
execute();
nofColumns(); returns 0
**end

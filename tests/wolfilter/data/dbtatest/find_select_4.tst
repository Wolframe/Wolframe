**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><main><aa>1</aa><cc>3</cc><bb>2</bb><dd></dd></main></doc>
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
	DO SELECT run( $(.//aa), $(doc/main/bb), $(//cc), $(doc/main//dd));
END
**outputfile:DBOUT
**output
Code:
[0] GOTO @1
[1] RESULT_SET_INIT
[2] DBSTM_START STM (SELECT run( $1, $2, $3, $4))
[3] DBSTM_BIND_CONST CONST '1'
[4] DBSTM_BIND_CONST CONST '2'
[5] DBSTM_BIND_CONST CONST '3'
[6] DBSTM_BIND_CONST CONST ''
[7] DBSTM_EXEC
[8] RETURN
start( 'SELECT run( $1, $2, $3, $4)' );
bind( 1, '1' );
bind( 2, '2' );
bind( 3, '3' );
bind( 4, NULL );
execute();
**end

**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><main><aa>1</aa><cc>3</cc><bb>2</bb></main></doc>
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
TRANSACTION testcall BEGIN
	DO SELECT run( $(//aa), $(main/bb), $(//cc));
END
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc/>
Code:
[0] RESULT_SET_INIT
[1] DBSTM_START STM (SELECT run( $1, $2, $3))
[2] DBSTM_BIND_CONST CONST '1'
[3] DBSTM_BIND_CONST CONST '2'
[4] DBSTM_BIND_CONST CONST '3'
[5] DBSTM_EXEC
[6] RETURN
start( 'SELECT run( $1, $2, $3)' );
bind( 1, '1' );
bind( 2, '2' );
bind( 3, '3' );
execute();
nofColumns(); returns 0
**end

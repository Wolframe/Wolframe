**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><aa>1</aa></doc>
**config
--config wolframe.conf --filter textwolf testcall
**file:wolframe.conf
LoadModules
{
	module ../../src/modules/filter/textwolf/mod_filter_textwolf
	module ../../src/modules/cmdbind/tdl/mod_command_tdl
	module ../wolfilter/modules/database/testtrace/mod_db_testtrace
}
Database
{
	test
	{
		identifier testdb
		outfile DBOUT
		file DBRES
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
	DO SELECT run( $(/aa));
	DO SELECT exec( $1);
END
**file: DBRES
#id#2
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc/>
Code:
[0] RESULT_SET_INIT
[1] DBSTM_START STM (SELECT run( $1))
[2] DBSTM_BIND_CONST CONST '1'
[3] DBSTM_EXEC
[4] RESULT_SET_INIT
[5] DBSTM_START STM (SELECT exec( $1))
[6] SELECT_LAST_RESULT
[7] DBSTM_BIND_SEL_IDX COLIDX 1
[8] DBSTM_EXEC
[9] RETURN
start( 'SELECT run( $1)' );
bind( 1, '1' );
execute();
nofColumns(); returns 1
columnName( 1); returns id
get( 1 ); returns 2
next(); returns 0
start( 'SELECT exec( $1)' );
bind( 1, '2' );
execute();
nofColumns(); returns 0
**end

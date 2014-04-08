**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<garten>1</garten><wald>3</wald><wiese>2</wiese>
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
	DO SELECT run( $(wald), $(wiese), $(garten));
END
**outputfile:DBOUT
**output
Code:
[0] RESULT_SET_INIT
[1] DBSTM_START STM (SELECT run( $1, $2, $3))
[2] DBSTM_BIND_CONST CONST '3'
[3] DBSTM_BIND_CONST CONST '2'
[4] DBSTM_BIND_CONST CONST '1'
[5] DBSTM_EXEC
[6] RETURN
start( 'SELECT run( $1, $2, $3)' );
bind( 1, '3' );
bind( 2, '2' );
bind( 3, '1' );
execute();
**end


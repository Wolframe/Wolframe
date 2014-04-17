**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<aa>1</aa>
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
INTO result
BEGIN
	INTO . DO SELECT run( $(/aa));
END
END
**file: DBRES
#id#1
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<result><id>1</id></result>
Code:
[0] OUTPUT_OPEN TAG result
[1] RESULT_SET_INIT
[2] DBSTM_START STM (SELECT run( $1))
[3] DBSTM_BIND_CONST CONST '1'
[4] DBSTM_EXEC
[5] OPEN_ITER_LAST_RESULT
[6] NOT_IF_COND GOTO @10
[7] OUTPUT_ITR_COLUMN
[8] NEXT
[9] IF_COND GOTO @7
[10] OUTPUT_CLOSE
[11] RETURN
start( 'SELECT run( $1)' );
bind( 1, '1' );
execute();
nofColumns(); returns 1
columnName( 1); returns id
get( 1 ); returns 1
next(); returns 0
**end

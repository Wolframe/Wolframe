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
RESULT INTO result
BEGIN
	INTO . DO SELECT run( $(/aa));
END
**file: DBRES
#id#1
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<result><id>1</id></result>
Code:
[0] GOTO @1
[1] OUTPUT_OPEN TAG result
[2] RESULT_SET_INIT
[3] DBSTM_START STM (SELECT run( $1))
[4] DBSTM_BIND_CONST CONST '1'
[5] DBSTM_EXEC
[6] OPEN_ITER_LAST_RESULT
[7] NOT_IF_COND GOTO @11
[8] OUTPUT_ITR_COLUMN
[9] NEXT
[10] IF_COND GOTO @8
[11] OUTPUT_CLOSE
[12] RETURN
start( 'SELECT run( $1)' );
bind( 1, '1' );
execute();
nofColumns(); returns 1
columnName( 1); returns id
get( 1 ); returns 1
next(); returns 0
**end

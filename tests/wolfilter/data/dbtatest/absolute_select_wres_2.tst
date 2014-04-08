**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<aa>1</aa><bb>2</bb>
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
	INTO item DO SELECT run( $(/aa), $(/bb));
END
**file: DBRES
#id name#1 hugo
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<result><item><id>1</id><name>hugo</name></item></result>
Code:
[0] OUTPUT_OPEN TAG result
[1] RESULT_SET_INIT
[2] DBSTM_START STM (SELECT run( $1, $2))
[3] DBSTM_BIND_CONST CONST '1'
[4] DBSTM_BIND_CONST CONST '2'
[5] DBSTM_EXEC
[6] OPEN_ITER_LAST_RESULT
[7] NOT_IF_COND GOTO @13
[8] OUTPUT_OPEN TAG item
[9] OUTPUT_ITR_COLUMN
[10] OUTPUT_CLOSE
[11] NEXT
[12] IF_COND GOTO @8
[13] OUTPUT_CLOSE
[14] RETURN
start( 'SELECT run( $1, $2)' );
bind( 1, '1' );
bind( 2, '2' );
execute();
nofColumns(); returns 2
columnName( 1); returns id
columnName( 2); returns name
get( 1 ); returns 1
get( 2 ); returns hugo
next(); returns 0
**end

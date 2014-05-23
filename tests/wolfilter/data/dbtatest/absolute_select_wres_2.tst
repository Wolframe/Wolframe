**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><aa>1</aa><bb>2</bb></doc>
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
	INTO item DO SELECT run( $(/aa), $(/bb));
END
END
**file: DBRES
#id name#1 hugo
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><result><item><id>1</id><name>hugo</name></item></result></doc>
Code:
[0] OUTPUT_OPEN TAG result
[1] RESULT_SET_INIT
[2] DBSTM_START STM (SELECT run( $1, $2))
[3] DBSTM_BIND_CONST CONST '1'
[4] DBSTM_BIND_CONST CONST '2'
[5] DBSTM_EXEC
[6] OUTPUT_OPEN_ARRAY TAG item
[7] OPEN_ITER_LAST_RESULT
[8] NOT_IF_COND GOTO @14
[9] OUTPUT_OPEN_ELEM
[10] OUTPUT_ITR_COLUMN
[11] OUTPUT_CLOSE_ELEM
[12] NEXT
[13] IF_COND GOTO @9
[14] OUTPUT_CLOSE_ARRAY
[15] OUTPUT_CLOSE
[16] RETURN
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

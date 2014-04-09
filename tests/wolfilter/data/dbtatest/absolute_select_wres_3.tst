**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<aa>1</aa><cc>3</cc><bb>2</bb>
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
RESULT INTO doc
BEGIN
	INTO address DO SELECT run( $(/aa), $(/bb), $(/cc));
END
**file: DBRES
#id name street#1 hugo "bahnhofstrasse 15"#2 miriam "zum gemsweg 3"#3 sara "tannensteig 12"
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><address><id>1</id><name>hugo</name><street>bahnhofstrasse 15</street></address><address><id>2</id><name>miriam</name><street>zum gemsweg 3</street></address><address><id>3</id><name>sara</name><street>tannensteig 12</street></address></doc>
Code:
[0] OUTPUT_OPEN TAG doc
[1] RESULT_SET_INIT
[2] DBSTM_START STM (SELECT run( $1, $2, $3))
[3] DBSTM_BIND_CONST CONST '1'
[4] DBSTM_BIND_CONST CONST '2'
[5] DBSTM_BIND_CONST CONST '3'
[6] DBSTM_EXEC
[7] OUTPUT_OPEN_ARRAY TAG address
[8] OPEN_ITER_LAST_RESULT
[9] NOT_IF_COND GOTO @15
[10] OUTPUT_OPEN_ELEM
[11] OUTPUT_ITR_COLUMN
[12] OUTPUT_CLOSE_ELEM
[13] NEXT
[14] IF_COND GOTO @10
[15] OUTPUT_CLOSE_ARRAY
[16] OUTPUT_CLOSE
[17] RETURN
start( 'SELECT run( $1, $2, $3)' );
bind( 1, '1' );
bind( 2, '2' );
bind( 3, '3' );
execute();
nofColumns(); returns 3
columnName( 1); returns id
columnName( 2); returns name
columnName( 3); returns street
get( 1 ); returns 1
get( 2 ); returns hugo
get( 3 ); returns bahnhofstrasse 15
next(); returns 1
get( 1 ); returns 2
get( 2 ); returns miriam
get( 3 ); returns zum gemsweg 3
next(); returns 1
get( 1 ); returns 3
get( 2 ); returns sara
get( 3 ); returns tannensteig 12
next(); returns 0
**end

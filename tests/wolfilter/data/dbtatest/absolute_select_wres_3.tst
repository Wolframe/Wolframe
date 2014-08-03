**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><aa>1</aa><cc>3</cc><bb>2</bb></doc>
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
	INTO address DO SELECT run( $(/aa), $(/bb), $(/cc));
END
**file: DBRES
#id name street#1 hugo "bahnhofstrasse 15"#2 miriam "zum gemsweg 3"#3 sara "tannensteig 12"
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><address><id>1</id><name>hugo</name><street>bahnhofstrasse 15</street></address><address><id>2</id><name>miriam</name><street>zum gemsweg 3</street></address><address><id>3</id><name>sara</name><street>tannensteig 12</street></address></doc>
Code:
[0] RESULT_SET_INIT
[1] DBSTM_START STM (SELECT run( $1, $2, $3))
[2] DBSTM_BIND_CONST CONST '1'
[3] DBSTM_BIND_CONST CONST '2'
[4] DBSTM_BIND_CONST CONST '3'
[5] DBSTM_EXEC
[6] OUTPUT_OPEN_ARRAY TAG address
[7] OPEN_ITER_LAST_RESULT
[8] NOT_IF_COND GOTO @14
[9] OUTPUT_OPEN_ELEM
[10] OUTPUT_ITR_COLUMN
[11] OUTPUT_CLOSE_ELEM
[12] NEXT
[13] IF_COND GOTO @9
[14] OUTPUT_CLOSE_ARRAY
[15] RETURN
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

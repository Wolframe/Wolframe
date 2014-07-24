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
	DO SELECT run( $(/aa) ,$(/bb),$(/cc) );
	INTO result FOREACH RESULT DO SELECT exec( $1,$2,$3);
END
**file: DBRES
#id name street#1 hugo "bahnhof strasse 15"#2 miriam "zum gems weg 3"#3 sara "tannen steig 12"
#offen#1
#offen#2
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><result><offen>1</offen></result><result><offen>2</offen></result></doc>
Code:
[0] RESULT_SET_INIT
[1] DBSTM_START STM (SELECT run( $1 ,$2,$3 ))
[2] DBSTM_BIND_CONST CONST '1'
[3] DBSTM_BIND_CONST CONST '2'
[4] DBSTM_BIND_CONST CONST '3'
[5] DBSTM_EXEC
[6] RESULT_SET_INIT
[7] OPEN_ITER_LAST_RESULT
[8] NOT_IF_COND GOTO @16
[9] DBSTM_START STM (SELECT exec( $1,$2,$3))
[10] DBSTM_BIND_ITR_IDX COLIDX 1
[11] DBSTM_BIND_ITR_IDX COLIDX 2
[12] DBSTM_BIND_ITR_IDX COLIDX 3
[13] DBSTM_EXEC
[14] NEXT
[15] IF_COND GOTO @9
[16] OUTPUT_OPEN_ARRAY TAG result
[17] OPEN_ITER_LAST_RESULT
[18] NOT_IF_COND GOTO @24
[19] OUTPUT_OPEN_ELEM
[20] OUTPUT_ITR_COLUMN
[21] OUTPUT_CLOSE_ELEM
[22] NEXT
[23] IF_COND GOTO @19
[24] OUTPUT_CLOSE_ARRAY
[25] RETURN
start( 'SELECT run( $1 ,$2,$3 )' );
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
get( 3 ); returns bahnhof strasse 15
next(); returns 1
get( 1 ); returns 2
get( 2 ); returns miriam
get( 3 ); returns zum gems weg 3
next(); returns 1
get( 1 ); returns 3
get( 2 ); returns sara
get( 3 ); returns tannen steig 12
next(); returns 0
start( 'SELECT exec( $1,$2,$3)' );
bind( 1, '1' );
bind( 2, 'hugo' );
bind( 3, 'bahnhof strasse 15' );
execute();
nofColumns(); returns 1
columnName( 1); returns offen
get( 1 ); returns 1
next(); returns 0
start( 'SELECT exec( $1,$2,$3)' );
bind( 1, '2' );
bind( 2, 'miriam' );
bind( 3, 'zum gems weg 3' );
execute();
nofColumns(); returns 1
columnName( 1); returns offen
get( 1 ); returns 2
next(); returns 0
start( 'SELECT exec( $1,$2,$3)' );
bind( 1, '3' );
bind( 2, 'sara' );
bind( 3, 'tannen steig 12' );
execute();
nofColumns(); returns 0
**end

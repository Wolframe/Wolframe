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
BEGIN
	DO SELECT run( $(/aa) ,$(/bb),$(/cc) );
	INTO doc FOREACH RESULT DO SELECT exec( $1,$2,$3);
END
**file: DBRES
#id name street#1 hugo "bahnhof strasse 15"#2 miriam "zum gems weg 3"#3 sara "tannen steig 12"
#offen#1
#offen#2
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><offen>1</offen></doc>
<doc><offen>2</offen></doc>
Code:
[0] GOTO @1
[1] RESULT_SET_INIT
[2] DBSTM_START STM (SELECT run( $1 ,$2,$3 ))
[3] DBSTM_BIND_CONST CONST '1'
[4] DBSTM_BIND_CONST CONST '2'
[5] DBSTM_BIND_CONST CONST '3'
[6] DBSTM_EXEC
[7] RESULT_SET_INIT
[8] OPEN_ITER_LAST_RESULT
[9] NOT_IF_COND GOTO @17
[10] DBSTM_START STM (SELECT exec( $1,$2,$3))
[11] DBSTM_BIND_ITR_IDX COLIDX 1
[12] DBSTM_BIND_ITR_IDX COLIDX 2
[13] DBSTM_BIND_ITR_IDX COLIDX 3
[14] DBSTM_EXEC
[15] NEXT
[16] IF_COND GOTO @10
[17] OPEN_ITER_LAST_RESULT
[18] NOT_IF_COND GOTO @24
[19] OUTPUT_OPEN TAG doc
[20] OUTPUT_ITR_COLUMN
[21] OUTPUT_CLOSE
[22] NEXT
[23] IF_COND GOTO @19
[24] RETURN
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
**end

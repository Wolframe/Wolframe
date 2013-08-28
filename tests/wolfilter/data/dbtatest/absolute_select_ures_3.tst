**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<aa>1</aa><cc>3</cc><bb>2</bb>
**config
--module ../../src/modules/filter/textwolf/mod_filter_textwolf
--module ../wolfilter/modules/database/testtrace/mod_db_testtrace
--filter 'xml:textwolf'
--database 'identifier=testdb,outfile=DBOUT,file=DBRES'
--program=DBIN.tdl
testcall
**file:DBIN.tdl
TRANSACTION testcall
BEGIN
	DO run( /aa ,/bb,/cc );
	INTO doc FOREACH /aa DO exec( $1,$2,$3);
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
run #1#2#3
exec #[1]#[2]#[3]

start( 'run' );
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
start( 'exec' );
bind( 1, '1' );
bind( 2, 'hugo' );
bind( 3, 'bahnhof strasse 15' );
execute();
nofColumns(); returns 1
columnName( 1); returns offen
get( 1 ); returns 1
next(); returns 0
start( 'exec' );
bind( 1, '2' );
bind( 2, 'miriam' );
bind( 3, 'zum gems weg 3' );
execute();
nofColumns(); returns 1
get( 1 ); returns 2
next(); returns 0
start( 'exec' );
bind( 1, '3' );
bind( 2, 'sara' );
bind( 3, 'tannen steig 12' );
execute();
nofColumns(); returns 0
**end

**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<aa>1</aa>
**config
--module ../../src/modules/filter/textwolf/mod_filter_textwolf
--module ../wolfilter/modules/database/testtrace/mod_db_testtrace
--filter 'textwolf'
--database 'identifier=testdb,outfile=DBOUT,file=DBRES'
--program=DBIN.tdl
testcall
**file:DBIN.tdl
TRANSACTION testcall
BEGIN
	DO run( /aa);
	DO exec( $1);
END
**file: DBRES
#id#2
**outputfile:DBOUT
**output
run #1
exec #[1]

start( 'run' );
bind( 1, '1' );
execute();
nofColumns(); returns 1
columnName( 1); returns id
get( 1 ); returns 2
next(); returns 0
start( 'exec' );
bind( 1, '2' );
execute();
nofColumns(); returns 0
**end

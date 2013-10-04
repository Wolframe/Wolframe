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
RESULT INTO result
BEGIN
	INTO . DO run( /aa);
END
**file: DBRES
#id#1
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<result><id>1</id></result>
run #1

start( 'run' );
bind( 1, '1' );
execute();
nofColumns(); returns 1
columnName( 1); returns id
get( 1 ); returns 1
next(); returns 0
**end

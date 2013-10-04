**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<aa>1</aa><cc>3</cc><bb>2</bb>
**config
--module ../../src/modules/filter/textwolf/mod_filter_textwolf
--module ../wolfilter/modules/database/testtrace/mod_db_testtrace
--filter 'textwolf'
--database 'identifier=testdb,outfile=DBOUT,file=DBRES'
--program=DBIN.tdl
testcall
**file:DBIN.tdl
TRANSACTION testcall
RESULT INTO doc
BEGIN
	INTO address DO run( /aa, /bb, /cc);
END
**file: DBRES
#id name street#1 hugo "bahnhofstrasse 15"#2 miriam "zum gemsweg 3"#3 sara "tannensteig 12"
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><address><id>1</id><name>hugo</name><street>bahnhofstrasse 15</street></address><address><id>2</id><name>miriam</name><street>zum gemsweg 3</street></address><address><id>3</id><name>sara</name><street>tannensteig 12</street></address></doc>
run #1#2#3

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

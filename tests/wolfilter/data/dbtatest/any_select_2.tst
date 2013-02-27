**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><item><aa>1</aa><cc>3</cc><bb>2</bb><dd></dd></item><item><aa>11</aa><cc>33</cc><bb></bb><dd>44</dd></item></doc>
**config
--module ../../src/modules/filter/textwolf/mod_filter_textwolf
--module ../wolfilter/modules/database/testtrace/mod_db_testtrace
--filter 'xml:textwolf'
--database 'identifier=testdb,outfile=DBOUT'
--program=DBIN.tdl
testcall
**file:DBIN.tdl
TRANSACTION testcall
BEGIN
	FOREACH /doc/*/aa DO run(../*/../aa, ../bb, .././cc, ..//dd/. ) ;
END
**outputfile:DBOUT
**output
run #1#2#3#
run #11##33#44

start( 'run' );
bind( 1, '1' );
bind( 2, '2' );
bind( 3, '3' );
bind( 4, '' );
execute();
nofColumns(); returns 0
start( 'run' );
bind( 1, '11' );
bind( 2, '' );
bind( 3, '33' );
bind( 4, '44' );
execute();
nofColumns(); returns 0
**end
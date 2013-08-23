**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<root><item><aa><xx>1</xx><yy>1</yy></aa></item><item><aa><xx>2</xx></aa></item><item><aa><xx>3</xx><yy>3</yy></aa></item></root>
**config
--module ../../src/modules/filter/textwolf/mod_filter_textwolf
--module ../wolfilter/modules/database/testtrace/mod_db_testtrace
--filter 'xml:textwolf'
--database 'identifier=testdb,outfile=DBOUT'
--program=DBIN.tdl
testcall
**file:DBIN.tdl
TRANSACTION testcall
BEGIN FOREACH //aa DO run( xx, yy);
END
**outputfile:DBOUT
**output
run #1#1
run #2#NULL
run #3#3

start( 'run' );
bind( 1, '1' );
bind( 2, '1' );
execute();
nofColumns(); returns 0
start( 'run' );
bind( 1, '2' );
bind( 2, NULL );
execute();
nofColumns(); returns 0
start( 'run' );
bind( 1, '3' );
bind( 2, '3' );
execute();
nofColumns(); returns 0
**end

**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<root><item><aa>1</aa></item><item></item><item><bb></bb></item></root>
**config
--module ../../src/modules/filter/textwolf/mod_filter_textwolf
--module ../wolfilter/modules/database/testtrace/mod_db_testtrace
--filter 'textwolf'
--database 'identifier=testdb,outfile=DBOUT'
--program=DBIN.tdl
testcall
**file:DBIN.tdl
TRANSACTION testcall
BEGIN FOREACH //aa DO run(../aa, /root/item/aa);
END
**outputfile:DBOUT
**output
run #1#1

start( 'run' );
bind( 1, '1' );
bind( 2, '1' );
execute();
nofColumns(); returns 0
**end

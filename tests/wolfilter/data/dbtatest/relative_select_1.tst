**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<guru>ein parameter text</guru>
**config
--module ../../src/modules/filter/textwolf/mod_filter_textwolf
--module ../wolfilter/modules/database/testtrace/mod_db_testtrace
--filter 'textwolf'
--database 'identifier=testdb,outfile=DBOUT'
--program=DBIN.tdl
testcall
**file:DBIN.tdl
TRANSACTION testcall BEGIN
	DO run( guru);
END
**outputfile:DBOUT
**output
run #ein parameter text

start( 'run' );
bind( 1, 'ein parameter text' );
execute();
nofColumns(); returns 0
**end

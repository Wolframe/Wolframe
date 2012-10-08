**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><aa>1</aa><bb>2</bb></doc>
**config
--module ../../src/modules/filter/textwolf/mod_filter_textwolf
--module ../wolfilter/modules/database/testtrace/mod_db_testtrace
--input-filter 'xml:textwolf'
--database 'id=testdb,outfile=DBOUT,program=DBIN'
testcall
**file:DBIN
TRANSACTION testcall BEGIN
	DO run(/doc/aa, //bb);
END
**outputfile:DBOUT
**output
run #1#2

start( 'run' );
bind( 1, '1' );
bind( 2, '2' );
execute();
nofColumns(); returns 0
**end

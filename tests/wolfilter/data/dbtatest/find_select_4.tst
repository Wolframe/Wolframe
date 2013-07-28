**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><main><aa>1</aa><cc>3</cc><bb>2</bb><dd></dd></main></doc>
**config
--module ../../src/modules/filter/textwolf/mod_filter_textwolf
--module ../wolfilter/modules/database/testtrace/mod_db_testtrace
--filter 'xml:textwolf'
--database 'identifier=testdb,outfile=DBOUT'
--program=DBIN.tdl
testcall
**file:DBIN.tdl
TRANSACTION testcall BEGIN
	DO run( .//aa/., ./doc/main/bb/., //cc/., ./doc/main//dd);
END
**outputfile:DBOUT
**output
run #1#2#3#NULL

start( 'run' );
bind( 1, '1' );
bind( 2, '2' );
bind( 3, '3' );
bind( 4, NULL );
execute();
nofColumns(); returns 0
**end

**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<root><item><aa>1</aa><aa>2</aa></item><item></item><item><aa>1</aa><aa>2</aa><aa>3</aa></item></root>
**config
--config wolframe.conf --filter textwolf testcall
**file:wolframe.conf
LoadModules
{
	module ../../src/modules/filter/textwolf/mod_filter_textwolf
	module ../wolfilter/modules/database/testtrace/mod_db_testtrace
}
Database
{
	test
	{
		identifier testdb
		outfile DBOUT
	}
}
Processor
{
	database testdb
	program DBIN.tdl
}
**file:DBIN.tdl
TRANSACTION testcall
BEGIN FOREACH //aa DO run(.);
END
**outputfile:DBOUT
**output
run #1
run #2
run #1
run #2
run #3

start( 'run' );
bind( 1, '1' );
execute();
nofColumns(); returns 0
start( 'run' );
bind( 1, '2' );
execute();
nofColumns(); returns 0
start( 'run' );
bind( 1, '1' );
execute();
nofColumns(); returns 0
start( 'run' );
bind( 1, '2' );
execute();
nofColumns(); returns 0
start( 'run' );
bind( 1, '3' );
execute();
nofColumns(); returns 0
**end
**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><item></item><item><aa>1</aa><bb>2</bb></item><item><aa>11</aa><bb>22</bb></item><item><aa>111</aa><bb>222</bb></item><item><bb></bb></item></doc>
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
TRANSACTION testcall BEGIN FOREACH/doc/item/aa DO run(., ../bb);END
**outputfile:DBOUT
**output
run #1#2
run #11#22
run #111#222

start( 'run' );
bind( 1, '1' );
bind( 2, '2' );
execute();
nofColumns(); returns 0
start( 'run' );
bind( 1, '11' );
bind( 2, '22' );
execute();
nofColumns(); returns 0
start( 'run' );
bind( 1, '111' );
bind( 2, '222' );
execute();
nofColumns(); returns 0
**end

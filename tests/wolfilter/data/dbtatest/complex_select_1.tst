**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<root><item><aa>1</aa></item><item></item><item><bb></bb></item></root>
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

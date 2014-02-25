**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<baum>hier ist ein baum text</baum><haus>hier ist ein haus text</haus>
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
TRANSACTION testcall BEGIN
	DO run( haus, baum);
END
**outputfile:DBOUT
**output
run #hier ist ein haus text#hier ist ein baum text

start( 'run' );
bind( 1, 'hier ist ein haus text' );
bind( 2, 'hier ist ein baum text' );
execute();
nofColumns(); returns 0
**end

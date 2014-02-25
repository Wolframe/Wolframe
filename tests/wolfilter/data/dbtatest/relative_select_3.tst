**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<garten>1</garten><wald>3</wald><wiese>2</wiese>
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
	DO run( wald, wiese, garten);
END
**outputfile:DBOUT
**output
run #3#2#1

start( 'run' );
bind( 1, '3' );
bind( 2, '2' );
bind( 3, '1' );
execute();
nofColumns(); returns 0
**end


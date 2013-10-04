**
**requires:LUA
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><task><title>job 1</title><key>A123</key><customernumber>324</customernumber></task><task><title>job 2</title><key>V456</key><customernumber>567</customernumber></task><employee><firstname>Julia</firstname><surname>Tegel-Sacher</surname><phone>098 765 43 21</phone></employee><issuedate>13.5.2006</issuedate></assignment><assignment><task><title>job 3</title><key>A456</key><customernumber>567</customernumber></task><task><title>job 4</title><key>V789</key><customernumber>890</customernumber></task><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><issuedate>13.5.2006</issuedate></assignment></assignmentlist>**config
--input-filter textwolf --output-filter textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf  --module ../../src/modules/cmdbind/lua/mod_command_lua --module ../wolfilter/modules/database/testtrace/mod_db_testtrace --database 'identifier=testdb,outfile=DBOUT,file=DBRES' --program=DBIN.tdl --cmdprogram transaction_table.lua run

**file: DBRES
#name#job1#job2#job3#job4
**file:DBIN.tdl
TRANSACTION test_transaction
RESULT INTO doc
BEGIN
	INTO title FOREACH //task DO run( title);
END
**file: transaction_table.lua

function run()
	t = input:table()
	r = formfunction("test_transaction")( t)
	output:print( r:table())
end

**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><title><name>job1</name></title><title><name>job2</name></title><title><name>job3</name></title><title><name>job4</name></title></doc>
**end

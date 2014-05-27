**
**requires:LUA
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><task><title>job 1</title><key>A123</key><customernumber>324</customernumber></task><task><title>job 2</title><key>V456</key><customernumber>567</customernumber></task><employee><firstname>Julia</firstname><surname>Tegel-Sacher</surname><phone>098 765 43 21</phone></employee><issuedate>13.5.2006</issuedate></assignment><assignment><task><title>job 3</title><key>A456</key><customernumber>567</customernumber></task><task><title>job 4</title><key>V789</key><customernumber>890</customernumber></task><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><issuedate>13.5.2006</issuedate></assignment></assignmentlist>**config
--input-filter textwolf --output-filter textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf -c wolframe.conf test_transaction
**requires:TEXTWOLF
**file:wolframe.conf
LoadModules
{
	module ../wolfilter/modules/database/testtrace/mod_db_testtrace
	module ../../src/modules/cmdbind/lua/mod_command_lua
}
Database
{
	test
	{
		identifier testdb
		outfile DBOUT
		file DBRES
	}
}
Processor
{
	program DBIN.tdl
	program script.lua
	database testdb
}
**file:script.lua
function printTable( tab)
	-- deterministic print of a table (since lua 5.2.1 table keys order is non deterministic)

	keys = {}
	for key,val in pairs( tab) do
		table.insert( keys, key)
	end
	table.sort( keys)

	for i,t in ipairs( keys) do
		local v = tab[ t]

		if type(v) == "table" then
			if v[ #v] then
				-- print array (keys are indices)
				for eidx,elem in ipairs( v) do
					output:opentag( t)
					if type(elem) == "table" then
						printTable( elem)
					else
						output:print( elem)
					end
					output:closetag()
				end
			else
				-- print table (keys are values)
				output:opentag( t)
				printTable( v)
				output:closetag()
			end
		else
			output:opentag( t)
			output:print( v)
			output:closetag()
		end
	end
end

function run()
	t = input:table()
	r = provider.formfunction("test_transaction")( t)
	printTable( r:table())
end
**file: DBRES
#name#job1#job2#job3#job4
**file:DBIN.tdl
TRANSACTION test_transaction
BEGIN
INTO doc
	BEGIN
		INTO title FOREACH //task DO SELECT run( title);
	END
END
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><doc><title><name>job1</name></title><title><name>job2</name></title><title><name>job3</name></title><title><name>job4</name></title></doc></assignmentlist>
**end

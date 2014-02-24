**
**requires:LUA
**requires:SQLITE3
**exception
error in transaction 'insertCustomer':*Customers must have a unique name.
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<customers><customer><name>Hugo</name></customer><customer><name>Hugo</name></customer></customers>**config
--input-filter textwolf --output-filter textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf -c wolframe.conf run
**requires:TEXTWOLF
**file:wolframe.conf
LoadModules
{
	module ../wolfilter/modules/database/sqlite3/mod_db_sqlite3test
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../../src/modules/normalize/number/mod_normalize_number
	module ../../src/modules/normalize/string/mod_normalize_string
}
Database
{
	SQliteTest
	{
		identifier testdb
		file test.db
		dumpfile DBDUMP
		inputfile DBDATA
	}
}
Processor
{
	database testdb
	program normalize.wnmp
	program DBPRG.tdl
	cmdhandler
	{
		lua
		{
			program script.lua
		}
	}
}
**file:normalize.wnmp
int=integer;
uint=unsigned;
float=floatingpoint;
currency=fixedpoint(13,2);
percent_1=fixedpoint(5,1);
**file:script.lua
function run()
	filter().empty = false
	local itr = input:get()
	for v,t in itr do
		if t == "customer" then
			formfunction( "insertCustomer")( scope( itr))
		end
	end
end
**file: DBDATA

CREATE TABLE Customer (
 ID INTEGER PRIMARY KEY AUTOINCREMENT,
 name TEXT ,
 CONSTRAINT tag_name_check UNIQUE( name )
);
**file:DBPRG.tdl
--
-- insertCustomer
--
TRANSACTION insertCustomer
BEGIN
	DO INSERT INTO Customer (name) VALUES ($(name));
	ON ERROR CONSTRAINT HINT "Customers must have a unique name.";
END
**outputfile:DBDUMP
**output
Customer:
'1', 'Hugo'

sqlite_sequence:
'Customer', '1'

**end

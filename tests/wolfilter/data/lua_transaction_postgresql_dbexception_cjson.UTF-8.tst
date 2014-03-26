**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**requires:PGSQL
**exception
error in transaction 'insertCustomer':*Customers must have a unique name.
**input
{
  "customers": {
    "customer": [
      { "name": "Hugo" },
      { "name": "Hugo" }
    ]
  }
}**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson -c wolframe.conf run

**file:wolframe.conf
LoadModules
{
	module ../wolfilter/modules/database/postgresql/mod_db_postgresqltest
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../../src/modules/normalize/number/mod_normalize_number
	module ../../src/modules/normalize/string/mod_normalize_string
	module ../../src/modules/datatype/bcdnumber/mod_datatype_bcdnumber
}
Database
{
	PostgreSQLTest
	{
		identifier testdb
		host localhost
		port 5432
		database wolframe
		user wolfusr
		password wolfpwd
		dumpfile DBDUMP
		inputfile DBDATA
	}
}
Processor
{
	program		normalize.wnmp
	program		DBPRG.tdl
	database	testdb
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
currency=bigfxp(2);
percent_1=bigfxp(2);
**file:script.lua
function run()
	provider.filter().empty = false
	local itr = input:get()
	for v,t in itr do
		if t == "customer" then
			provider.formfunction( "insertCustomer")( iterator.scope( itr))
		end
	end
end
**file: DBDATA

CREATE TABLE Customer (
 ID SERIAL NOT NULL PRIMARY KEY,
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
customer:
id, name
'1', 'Hugo'
**end

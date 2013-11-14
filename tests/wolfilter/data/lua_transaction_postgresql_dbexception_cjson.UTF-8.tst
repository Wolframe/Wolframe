**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**requires:PGSQL
**exception
error in transaction insertCustomer:*Customers must have a unique name.
**input
{
  "customers": {
    "customer": [
      { "name": "Hugo" },
      { "name": "Hugo" }
    ]
  }
}**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson  --module ../../src/modules/cmdbind/lua/mod_command_lua --cmdprogram=transaction_dbexception.lua --program simpleform.wnmp --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/normalize//string/mod_normalize_string --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../wolfilter/modules/database/postgresql/mod_db_postgresqltest --database 'identifier=testdb,host=localhost,port=5432,database=wolframe,user=wolfusr,password=wolfpwd,dumpfile=DBDUMP,inputfile=DBDATA' --program=DBPRG.tdl run

**file:simpleform.wnmp
int=number:integer;
uint=number:unsigned;
float=number:float;
currency=number:fixedpoint(13,2);
percent_1=number:fixedpoint(5,1);
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
	ON ERROR CONSTRAINT HINT ". Customers must have a unique name.";
END
**outputfile:DBDUMP
**file: transaction_dbexception.lua
function run()
	filter().empty = false
	local itr = input:get()
	for v,t in itr do
		if t == "customer" then
			formfunction( "insertCustomer")( scope( itr))
		end
	end
end

**output
customer:
id, name
'1', 'Hugo'
**end

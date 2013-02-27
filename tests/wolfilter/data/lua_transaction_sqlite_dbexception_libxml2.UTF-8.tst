**
**requires:LUA
**requires:LIBXML2
**requires:SQLITE3
**exception
error in transaction insertCustomer:column name is not unique . Customers must have a unique name.
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<customers><customer><name>Hugo</name></customer><customer><name>Hugo</name></customer></customers>**config
--input-filter xml:libxml2 --output-filter xml:libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2  --module ../../src/modules/cmdbind/lua/mod_command_lua --program=transaction_dbexception.lua --program simpleform.normalize --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/normalize//string/mod_normalize_string --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../wolfilter/modules/database/sqlite3/mod_db_sqlite3test --database 'identifier=testdb,file=test.db,dumpfile=DBDUMP,inputfile=DBDATA' --program=DBPRG.tdl run

**file:simpleform.normalize
int=number:integer;
uint=number:unsigned;
float=number:float;
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
	ON ERROR CONSTRAINT HINT ". Customers must have a unique name."
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
Customer:
'1', 'Hugo'

sqlite_sequence:
'Customer', '1'

**end

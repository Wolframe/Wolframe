**
**requires:LUA
**requires:ORACLE
**exception
error in transaction 'insertCustomer':*Customers must have a unique name.
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<customers><customer><name>Hugo</name></customer><customer><name>Hugo</name></customer></customers>**config
--input-filter textwolf --output-filter textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf  --module ../../src/modules/cmdbind/lua/mod_command_lua --cmdprogram=transaction_dbexception.lua --program simpleform.wnmp --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/normalize//string/mod_normalize_string --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../wolfilter/modules/database/oracle/mod_db_oracletest --database 'identifier=testdb,host=andreasbaumann.dyndns.org,port=1521,database=orcl,user=wolfusr,password=wolfpwd,dumpfile=DBDUMP,inputfile=DBDATA' --program=DBPRG.tdl run
**requires:TEXTWOLF
**file:simpleform.wnmp
int=integer;
uint=unsigned;
float=floatingpoint;
currency=fixedpoint(13,2);
percent_1=fixedpoint(5,1);
**file: DBDATA

CREATE TABLE Customer (
 ID INTEGER NOT NULL PRIMARY KEY,
 name VARCHAR(32) ,
 CONSTRAINT tag_name_check UNIQUE( name )
);
CREATE SEQUENCE Customer_ID_Seq START WITH 1 INCREMENT BY 1;
CREATE TRIGGER Customer_Insert
BEFORE INSERT ON Customer
FOR EACH ROW
BEGIN
	SELECT Customer_ID_Seq.nextval into :new.id FROM dual;
END;
/
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
CUSTOMER:
ID, NAME
'1', 'Hugo'
**end

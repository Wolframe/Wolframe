**
**requires:LIBXML2
**requires:PGSQL
**input
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE person SYSTEM 'Person'>
<person><name>Hugi Turbo</name><address>Gurkenstrasse 7a 3145 Gumligen</address></person>**config
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2 --module ../../src/modules/doctype/xml/mod_doctype_xml -c wolframe.conf InsertPerson

**file:wolframe.conf
LoadModules
{
	module ./../wolfilter/modules/database/postgresql/mod_db_postgresqltest
	module ./../../src/modules/cmdbind/directmap/mod_command_directmap
	module ./../../src/modules/cmdbind/tdl/mod_command_tdl
	module ./../wolfilter/modules/functions/audit/mod_audit
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
	database testdb
	program audit.tdl

	cmdhandler
	{
		directmap
		{
			program audit.dmap
			filter libxml2
		}
	}
}
**file:DBDATA
CREATE TABLE Person
(
ID SERIAL NOT NULL PRIMARY KEY,
 name TEXT,
 address TEXT
);

INSERT INTO Person (name,address) VALUES ('Aufru','Amselstrasse 12 Aulach');
INSERT INTO Person (name,address) VALUES ('Beno','Butterweg 23 Bendorf');
INSERT INTO Person (name,address) VALUES ('Carla','Camelstreet 34 Carassa');
INSERT INTO Person (name,address) VALUES ('Dorothe','Demotastrasse 45 Durnfo');
INSERT INTO Person (name,address) VALUES ('Erik','Erakimolstrasse 56 Enden');
**file:audit.dmap
COMMAND (Insert Person) SKIP CALL insertPerson;
**file:audit.tdl

TRANSACTION insertPerson
BEGIN
	DO INSERT INTO Person (name, address) VALUES ($(name), $(address));
	DO UNIQUE SELECT id from Person WHERE name = $(name);
END
AUDIT CRITICAL auditMutation1( operation="insert", $RESULT.id, newvalue=$(name))
**outputfile:DBDUMP
**outputfile:audit.log
**output
person:
id, name, address
'1', 'Aufru', 'Amselstrasse 12 Aulach'
'2', 'Beno', 'Butterweg 23 Bendorf'
'3', 'Carla', 'Camelstreet 34 Carassa'
'4', 'Dorothe', 'Demotastrasse 45 Durnfo'
'5', 'Erik', 'Erakimolstrasse 56 Enden'
'6', 'Hugi Turbo', 'Gurkenstrasse 7a 3145 Gumligen'
audit insert user '' id 6 old '' new 'Hugi Turbo'
**end

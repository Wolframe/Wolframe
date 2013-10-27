**
**requires:LIBXML2
**requires:SQLITE3
**input
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE data SYSTEM "AllDataRequest">
<data/>**config
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2  --module ../../src/modules/cmdbind/lua/mod_command_lua --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../../src/modules/normalize/number/mod_normalize_number --module ../../src/modules/normalize/string/mod_normalize_string --module ../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform --module ../wolfilter/modules/database/sqlite3/mod_db_sqlite3test --cmdprogram=preprocess.dmap --program=preprocess.sfrm --program=preprocess.tdl --program=preprocess.wnmp --program=preprocess.lua --database 'identifier=testdb,file=test.db,dumpfile=DBDUMP,inputfile=DBDATA' AllDataRequest

**file:preprocess.sfrm
FORM PersonRef
{
	prename string
	surname string
}

FORM AddressRef
{
	street string
	town string
}

FORM CompanyRef
{
	name string
}

FORM Person
{
	company CompanyRef[]
	id int
	parent ?string
	child PersonRef[]
	prename string
	surname string
	location AddressRef[]
	tag ?int
}

FORM Company
{
	id int
	parent string
	name string
	location AddressRef[]
	child CompanyRef[]
	tag int[]
}

FORM Data
{
	data
	{
		person Person[]
	}
}

FORM AllDataRequest
{
	data
	{
	}
}
**file:preprocess.wnmp
int=number:integer;
uint=number:unsigned;
float=number:float;
normname=string:convdia,lcname;
**file: DBDATA
CREATE TABLE Person
(
 ID INTEGER PRIMARY KEY AUTOINCREMENT,
 prename TEXT,
 surname TEXT
);

CREATE TABLE Address
(
 ID INTEGER PRIMARY KEY AUTOINCREMENT,
 street TEXT,
 town TEXT
);

CREATE TABLE Company
(
 ID INTEGER PRIMARY KEY AUTOINCREMENT,
 name TEXT
);

CREATE TABLE PersonChildRel
(
 ID INT NOT NULL,
 childid INT NOT NULL
);

CREATE TABLE PersonAddressRel
(
 ID INT NOT NULL,
 addressid  INT NOT NULL
);

CREATE TABLE CompanyChildRel
(
 ID INT NOT NULL,
 childid  INT NOT NULL
);

CREATE TABLE CompanyAddressRel
(
 ID INT NOT NULL,
 addressid  INT NOT NULL
);

CREATE TABLE PersonCompanyRel
(
 ID INT NOT NULL,
 companyid  INT NOT NULL
);

CREATE TABLE WordTable
(
 name TEXT,
 word TEXT
);

CREATE TABLE NumberTable
(
 name TEXT,
 number INT
);

INSERT INTO Address (street,town) VALUES ('Amselstrasse 12','Aulach');
INSERT INTO Address (street,town) VALUES ('Butterweg 23','Bendorf');
INSERT INTO Address (street,town) VALUES ('Camelstreet 34','Carassa');
INSERT INTO Address (street,town) VALUES ('Demotastrasse 45','Durnfo');
INSERT INTO Address (street,town) VALUES ('Erakimolstrasse 56','Enden');
INSERT INTO Address (street,town) VALUES ('Fabelweg 67','Formkon');
INSERT INTO Address (street,town) VALUES ('Geranienstrasse 78','Ganaus');
INSERT INTO Address (street,town) VALUES ('Hurtika 89','Hof');

INSERT INTO Person (prename,surname) VALUES ('Aufru','Alano');
INSERT INTO Person (prename,surname) VALUES ('Beno','Beret');
INSERT INTO Person (prename,surname) VALUES ('Carla','Carlson');
INSERT INTO Person (prename,surname) VALUES ('Dorothe','Dubi');
INSERT INTO Person (prename,surname) VALUES ('Erik','Ertki');
INSERT INTO Person (prename,surname) VALUES ('Fran','Fuioko');
INSERT INTO Person (prename,surname) VALUES ('Gerd','Golto');
INSERT INTO Person (prename,surname) VALUES ('Hubert','Hauer');

INSERT INTO Company (name) VALUES ('Arindo Inc.');
INSERT INTO Company (name) VALUES ('Baluba Inc.');
INSERT INTO Company (name) VALUES ('Carimba Inc.');
INSERT INTO Company (name) VALUES ('Dereno Inc.');
INSERT INTO Company (name) VALUES ('Etungo Inc.');
INSERT INTO Company (name) VALUES ('Figaji Inc.');
INSERT INTO Company (name) VALUES ('Gaurami Inc.');
INSERT INTO Company (name) VALUES ('Huratz Inc.');

INSERT INTO PersonChildRel (ID,childid) VALUES (1,2);
INSERT INTO PersonChildRel (ID,childid) VALUES (1,3);
INSERT INTO PersonChildRel (ID,childid) VALUES (1,4);
INSERT INTO PersonChildRel (ID,childid) VALUES (2,3);
INSERT INTO PersonChildRel (ID,childid) VALUES (2,4);
INSERT INTO PersonChildRel (ID,childid) VALUES (2,5);
INSERT INTO PersonChildRel (ID,childid) VALUES (3,4);
INSERT INTO PersonChildRel (ID,childid) VALUES (3,5);
INSERT INTO PersonChildRel (ID,childid) VALUES (3,6);
INSERT INTO PersonChildRel (ID,childid) VALUES (4,5);
INSERT INTO PersonChildRel (ID,childid) VALUES (5,6);
INSERT INTO PersonChildRel (ID,childid) VALUES (6,6); -- fake for SQLITE
INSERT INTO PersonChildRel (ID,childid) VALUES (7,8);
INSERT INTO PersonChildRel (ID,childid) VALUES (1,8);
INSERT INTO PersonChildRel (ID,childid) VALUES (2,8);
INSERT INTO PersonChildRel (ID,childid) VALUES (3,8);
INSERT INTO PersonChildRel (ID,childid) VALUES (8,8); -- fake for SQLITE

INSERT INTO CompanyChildRel (ID,childid) VALUES (1,2);
INSERT INTO CompanyChildRel (ID,childid) VALUES (1,3);
INSERT INTO CompanyChildRel (ID,childid) VALUES (1,4);
INSERT INTO CompanyChildRel (ID,childid) VALUES (2,3);
INSERT INTO CompanyChildRel (ID,childid) VALUES (2,4);
INSERT INTO CompanyChildRel (ID,childid) VALUES (2,5);
INSERT INTO CompanyChildRel (ID,childid) VALUES (3,4);
INSERT INTO CompanyChildRel (ID,childid) VALUES (3,5);
INSERT INTO CompanyChildRel (ID,childid) VALUES (3,6);
INSERT INTO CompanyChildRel (ID,childid) VALUES (4,5);
INSERT INTO CompanyChildRel (ID,childid) VALUES (5,6);
INSERT INTO CompanyChildRel (ID,childid) VALUES (6,6); -- fake for SQLITE
INSERT INTO CompanyChildRel (ID,childid) VALUES (7,8);
INSERT INTO CompanyChildRel (ID,childid) VALUES (1,8);
INSERT INTO CompanyChildRel (ID,childid) VALUES (2,8);
INSERT INTO CompanyChildRel (ID,childid) VALUES (3,8);
INSERT INTO CompanyChildRel (ID,childid) VALUES (8,8); -- fake for SQLITE

INSERT INTO PersonAddressRel (ID,addressid) VALUES (1,2);
INSERT INTO PersonAddressRel (ID,addressid) VALUES (1,3);
INSERT INTO PersonAddressRel (ID,addressid) VALUES (1,4);
INSERT INTO PersonAddressRel (ID,addressid) VALUES (2,3);
INSERT INTO PersonAddressRel (ID,addressid) VALUES (2,4);
INSERT INTO PersonAddressRel (ID,addressid) VALUES (2,5);
INSERT INTO PersonAddressRel (ID,addressid) VALUES (3,4);
INSERT INTO PersonAddressRel (ID,addressid) VALUES (3,5);
INSERT INTO PersonAddressRel (ID,addressid) VALUES (3,6);
INSERT INTO PersonAddressRel (ID,addressid) VALUES (4,5);
INSERT INTO PersonAddressRel (ID,addressid) VALUES (5,6);
INSERT INTO PersonAddressRel (ID,addressid) VALUES (6,6); -- fake for SQLITE
INSERT INTO PersonAddressRel (ID,addressid) VALUES (7,8);
INSERT INTO PersonAddressRel (ID,addressid) VALUES (1,8);
INSERT INTO PersonAddressRel (ID,addressid) VALUES (2,8);
INSERT INTO PersonAddressRel (ID,addressid) VALUES (3,8);
INSERT INTO PersonAddressRel (ID,addressid) VALUES (8,8); -- fake for SQLITE

INSERT INTO CompanyAddressRel (ID,addressid) VALUES (1,2);
INSERT INTO CompanyAddressRel (ID,addressid) VALUES (1,3);
INSERT INTO CompanyAddressRel (ID,addressid) VALUES (1,4);
INSERT INTO CompanyAddressRel (ID,addressid) VALUES (2,3);
INSERT INTO CompanyAddressRel (ID,addressid) VALUES (2,4);
INSERT INTO CompanyAddressRel (ID,addressid) VALUES (2,5);
INSERT INTO CompanyAddressRel (ID,addressid) VALUES (3,4);
INSERT INTO CompanyAddressRel (ID,addressid) VALUES (3,5);
INSERT INTO CompanyAddressRel (ID,addressid) VALUES (3,6);
INSERT INTO CompanyAddressRel (ID,addressid) VALUES (4,5);
INSERT INTO CompanyAddressRel (ID,addressid) VALUES (5,6);
INSERT INTO CompanyAddressRel (ID,addressid) VALUES (6,6); -- fake for SQLITE
INSERT INTO CompanyAddressRel (ID,addressid) VALUES (7,8);
INSERT INTO CompanyAddressRel (ID,addressid) VALUES (1,8);
INSERT INTO CompanyAddressRel (ID,addressid) VALUES (2,8);
INSERT INTO CompanyAddressRel (ID,addressid) VALUES (3,8);
INSERT INTO CompanyAddressRel (ID,addressid) VALUES (8,8); -- fake for SQLITE

INSERT INTO PersonCompanyRel (ID,companyid) VALUES (1,2);
INSERT INTO PersonCompanyRel (ID,companyid) VALUES (1,3);
INSERT INTO PersonCompanyRel (ID,companyid) VALUES (1,4);
INSERT INTO PersonCompanyRel (ID,companyid) VALUES (2,3);
INSERT INTO PersonCompanyRel (ID,companyid) VALUES (2,4);
INSERT INTO PersonCompanyRel (ID,companyid) VALUES (2,5);
INSERT INTO PersonCompanyRel (ID,companyid) VALUES (3,4);
INSERT INTO PersonCompanyRel (ID,companyid) VALUES (3,5);
INSERT INTO PersonCompanyRel (ID,companyid) VALUES (3,6);
INSERT INTO PersonCompanyRel (ID,companyid) VALUES (4,5);
INSERT INTO PersonCompanyRel (ID,companyid) VALUES (5,6);
INSERT INTO PersonCompanyRel (ID,companyid) VALUES (6,6); -- fake for SQLITE
INSERT INTO PersonCompanyRel (ID,companyid) VALUES (7,8);
INSERT INTO PersonCompanyRel (ID,companyid) VALUES (1,8);
INSERT INTO PersonCompanyRel (ID,companyid) VALUES (2,8);
INSERT INTO PersonCompanyRel (ID,companyid) VALUES (3,8);
INSERT INTO PersonCompanyRel (ID,companyid) VALUES (8,8); -- fake for SQLITE
**file:preprocess.tdl

SUBROUTINE getPersonPrename( id)
BEGIN
	INTO . DO UNIQUE SELECT prename FROM Person WHERE Person.ID = $[id];
END

SUBROUTINE getPerson( id)
RESULT INTO person
BEGIN
	INTO company DO SELECT Company.name FROM Company,PersonCompanyRel
		WHERE PersonCompanyRel.companyid = Company.ID
		AND PersonCompanyRel.ID = $[id];

	INTO tag PRINT "1001";							-- print constant

	INTO . DO UNIQUE SELECT surname FROM Person WHERE Person.ID = $[id];	-- embedded command with variable as argument
	INTO . DO UNIQUE getPersonPrename( $[id]);				-- call subroutine with variable as argument

	INTO id PRINT $[id];							-- print variable

	INTO child DO SELECT Person.prename,Person.surname FROM Person,PersonChildRel
		WHERE PersonChildRel.childid = Person.ID
		AND PersonChildRel.ID = $[id];

	INTO location DO SELECT Address.street,Address.town FROM Address,PersonAddressRel
		WHERE PersonAddressRel.addressid = Address.ID
		AND PersonAddressRel.ID = $[id];
END

TRANSACTION getData
BEGIN
	DO SELECT ID FROM Person;
	INTO . DO getPerson( $1);
END

TRANSACTION getDataFiltered
RESULT FILTER addSuffixToName
BEGIN
	DO SELECT ID FROM Person;
	INTO person DO UNIQUE SELECT Person.ID as id,prename,surname FROM Person WHERE Person.ID = $1;
END

TRANSACTION insertWords
PREPROCESS
BEGIN
	FOREACH /data/person DO luanorm( . ) INTO norm;

	FOREACH /data/person/location DO normname( street) INTO norm_street;
	FOREACH /data/person/location DO normname( town) INTO norm_town;
END
BEGIN
	FOREACH /data/person/location DO INSERT INTO WordTable (name,word) VALUES ('select street', $(norm_street));
	FOREACH /data/person/location DO INSERT INTO WordTable (name,word) VALUES ('select town', $(norm_town));

	FOREACH /data/person/norm/location DO INSERT INTO WordTable (name,word) VALUES ('struct street', $(street));
	FOREACH /data/person/norm/location DO INSERT INTO WordTable (name,word) VALUES ('struct town', $(town));
	FOREACH /data/person/norm/surname DO INSERT INTO WordTable (name,word) VALUES ('struct surname', $(.));
	FOREACH /data/person/norm DO INSERT INTO WordTable (name,word) VALUES ('struct prename', $(prename));
	FOREACH /data/person/norm DO INSERT INTO NumberTable (name,number) VALUES ('struct tag', $(tag));
	FOREACH /data/person/norm DO INSERT INTO NumberTable (name,number) VALUES ('struct id', $(id));
	FOREACH /data/person/norm/company DO INSERT INTO WordTable (name,word) VALUES ('company name', $(name));
END
**file:preprocess.dmap
COMMAND (AllDataRequest) CALL(run) RETURN(Data);
**file:preprocess.lua
function run( inp )
	it = inp:table()
	getData = provider.formfunction("getData")
	res = getData( it)
	rt = res:table()
	insertWords = provider.formfunction("insertWords")
	insertWords( { data = rt } )
	getDataFiltered = provider.formfunction("getDataFiltered")
	resfiltered = getDataFiltered( it)
	resfilteredtab = resfiltered:table()
	table.insert( rt, resfilteredtab)
	return rt
end

function luanorm( inp )
	local function luanorm_table( tb )
		local rt = {}
		for k,v in pairs( tb) do
			if type(v) == "table" then
				rt[ k] = luanorm_table( v)
			else
				if k == "id" or k == "tag" then
					rt[ k] = tonumber(v) + 100
				else
					local nf = provider.normalizer( "normname")
					rt[ k] = nf( v)
				end
			end
		end
		return rt
	end
	local intb = inp:table()
	local outtb = luanorm_table( intb)
	return outtb
end

function addSuffixToName( inp)
	rec = inp:table()
	for i,v in ipairs( rec["person"]) do
		v[ "prename"] = v[ "prename"] .. v[ "id"]
	end
	return rec
end
**outputfile:DBDUMP
**output
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE data SYSTEM "Data.simpleform"><data><person><company><name>Baluba Inc.</name></company><company><name>Carimba Inc.</name></company><company><name>Dereno Inc.</name></company><company><name>Huratz Inc.</name></company><id>1</id><child><prename>Beno</prename><surname>Beret</surname></child><child><prename>Carla</prename><surname>Carlson</surname></child><child><prename>Dorothe</prename><surname>Dubi</surname></child><child><prename>Hubert</prename><surname>Hauer</surname></child><prename>Aufru</prename><surname>Alano</surname><location><street>Butterweg 23</street><town>Bendorf</town></location><location><street>Camelstreet 34</street><town>Carassa</town></location><location><street>Demotastrasse 45</street><town>Durnfo</town></location><location><street>Hurtika 89</street><town>Hof</town></location><tag>1001</tag></person><person><company><name>Carimba Inc.</name></company><company><name>Dereno Inc.</name></company><company><name>Etungo Inc.</name></company><company><name>Huratz Inc.</name></company><id>2</id><child><prename>Carla</prename><surname>Carlson</surname></child><child><prename>Dorothe</prename><surname>Dubi</surname></child><child><prename>Erik</prename><surname>Ertki</surname></child><child><prename>Hubert</prename><surname>Hauer</surname></child><prename>Beno</prename><surname>Beret</surname><location><street>Camelstreet 34</street><town>Carassa</town></location><location><street>Demotastrasse 45</street><town>Durnfo</town></location><location><street>Erakimolstrasse 56</street><town>Enden</town></location><location><street>Hurtika 89</street><town>Hof</town></location><tag>1001</tag></person><person><company><name>Dereno Inc.</name></company><company><name>Etungo Inc.</name></company><company><name>Figaji Inc.</name></company><company><name>Huratz Inc.</name></company><id>3</id><child><prename>Dorothe</prename><surname>Dubi</surname></child><child><prename>Erik</prename><surname>Ertki</surname></child><child><prename>Fran</prename><surname>Fuioko</surname></child><child><prename>Hubert</prename><surname>Hauer</surname></child><prename>Carla</prename><surname>Carlson</surname><location><street>Demotastrasse 45</street><town>Durnfo</town></location><location><street>Erakimolstrasse 56</street><town>Enden</town></location><location><street>Fabelweg 67</street><town>Formkon</town></location><location><street>Hurtika 89</street><town>Hof</town></location><tag>1001</tag></person><person><company><name>Etungo Inc.</name></company><id>4</id><child><prename>Erik</prename><surname>Ertki</surname></child><prename>Dorothe</prename><surname>Dubi</surname><location><street>Erakimolstrasse 56</street><town>Enden</town></location><tag>1001</tag></person><person><company><name>Figaji Inc.</name></company><id>5</id><child><prename>Fran</prename><surname>Fuioko</surname></child><prename>Erik</prename><surname>Ertki</surname><location><street>Fabelweg 67</street><town>Formkon</town></location><tag>1001</tag></person><person><company><name>Figaji Inc.</name></company><id>6</id><child><prename>Fran</prename><surname>Fuioko</surname></child><prename>Fran</prename><surname>Fuioko</surname><location><street>Fabelweg 67</street><town>Formkon</town></location><tag>1001</tag></person><person><company><name>Huratz Inc.</name></company><id>7</id><child><prename>Hubert</prename><surname>Hauer</surname></child><prename>Gerd</prename><surname>Golto</surname><location><street>Hurtika 89</street><town>Hof</town></location><tag>1001</tag></person><person><company><name>Huratz Inc.</name></company><id>8</id><child><prename>Hubert</prename><surname>Hauer</surname></child><prename>Hubert</prename><surname>Hauer</surname><location><street>Hurtika 89</street><town>Hof</town></location><tag>1001</tag></person><person><id>1</id><prename>Aufru1</prename><surname>Alano</surname></person><person><id>2</id><prename>Beno2</prename><surname>Beret</surname></person><person><id>3</id><prename>Carla3</prename><surname>Carlson</surname></person><person><id>4</id><prename>Dorothe4</prename><surname>Dubi</surname></person><person><id>5</id><prename>Erik5</prename><surname>Ertki</surname></person><person><id>6</id><prename>Fran6</prename><surname>Fuioko</surname></person><person><id>7</id><prename>Gerd7</prename><surname>Golto</surname></person><person><id>8</id><prename>Hubert8</prename><surname>Hauer</surname></person></data>
Person:
'1', 'Aufru', 'Alano'
'2', 'Beno', 'Beret'
'3', 'Carla', 'Carlson'
'4', 'Dorothe', 'Dubi'
'5', 'Erik', 'Ertki'
'6', 'Fran', 'Fuioko'
'7', 'Gerd', 'Golto'
'8', 'Hubert', 'Hauer'

sqlite_sequence:
'Address', '8'
'Person', '8'
'Company', '8'

Address:
'1', 'Amselstrasse 12', 'Aulach'
'2', 'Butterweg 23', 'Bendorf'
'3', 'Camelstreet 34', 'Carassa'
'4', 'Demotastrasse 45', 'Durnfo'
'5', 'Erakimolstrasse 56', 'Enden'
'6', 'Fabelweg 67', 'Formkon'
'7', 'Geranienstrasse 78', 'Ganaus'
'8', 'Hurtika 89', 'Hof'

Company:
'1', 'Arindo Inc.'
'2', 'Baluba Inc.'
'3', 'Carimba Inc.'
'4', 'Dereno Inc.'
'5', 'Etungo Inc.'
'6', 'Figaji Inc.'
'7', 'Gaurami Inc.'
'8', 'Huratz Inc.'

PersonChildRel:
'1', '2'
'1', '3'
'1', '4'
'2', '3'
'2', '4'
'2', '5'
'3', '4'
'3', '5'
'3', '6'
'4', '5'
'5', '6'
'6', '6'
'7', '8'
'1', '8'
'2', '8'
'3', '8'
'8', '8'

PersonAddressRel:
'1', '2'
'1', '3'
'1', '4'
'2', '3'
'2', '4'
'2', '5'
'3', '4'
'3', '5'
'3', '6'
'4', '5'
'5', '6'
'6', '6'
'7', '8'
'1', '8'
'2', '8'
'3', '8'
'8', '8'

CompanyChildRel:
'1', '2'
'1', '3'
'1', '4'
'2', '3'
'2', '4'
'2', '5'
'3', '4'
'3', '5'
'3', '6'
'4', '5'
'5', '6'
'6', '6'
'7', '8'
'1', '8'
'2', '8'
'3', '8'
'8', '8'

CompanyAddressRel:
'1', '2'
'1', '3'
'1', '4'
'2', '3'
'2', '4'
'2', '5'
'3', '4'
'3', '5'
'3', '6'
'4', '5'
'5', '6'
'6', '6'
'7', '8'
'1', '8'
'2', '8'
'3', '8'
'8', '8'

PersonCompanyRel:
'1', '2'
'1', '3'
'1', '4'
'2', '3'
'2', '4'
'2', '5'
'3', '4'
'3', '5'
'3', '6'
'4', '5'
'5', '6'
'6', '6'
'7', '8'
'1', '8'
'2', '8'
'3', '8'
'8', '8'

WordTable:
'select street', 'butterweg 23'
'select street', 'camelstreet 34'
'select street', 'demotastrasse 45'
'select street', 'hurtika 89'
'select street', 'camelstreet 34'
'select street', 'demotastrasse 45'
'select street', 'erakimolstrasse 56'
'select street', 'hurtika 89'
'select street', 'demotastrasse 45'
'select street', 'erakimolstrasse 56'
'select street', 'fabelweg 67'
'select street', 'hurtika 89'
'select street', 'erakimolstrasse 56'
'select street', 'fabelweg 67'
'select street', 'fabelweg 67'
'select street', 'hurtika 89'
'select street', 'hurtika 89'
'select town', 'bendorf'
'select town', 'carassa'
'select town', 'durnfo'
'select town', 'hof'
'select town', 'carassa'
'select town', 'durnfo'
'select town', 'enden'
'select town', 'hof'
'select town', 'durnfo'
'select town', 'enden'
'select town', 'formkon'
'select town', 'hof'
'select town', 'enden'
'select town', 'formkon'
'select town', 'formkon'
'select town', 'hof'
'select town', 'hof'
'struct street', 'butterweg 23'
'struct street', 'camelstreet 34'
'struct street', 'demotastrasse 45'
'struct street', 'hurtika 89'
'struct street', 'camelstreet 34'
'struct street', 'demotastrasse 45'
'struct street', 'erakimolstrasse 56'
'struct street', 'hurtika 89'
'struct street', 'demotastrasse 45'
'struct street', 'erakimolstrasse 56'
'struct street', 'fabelweg 67'
'struct street', 'hurtika 89'
'struct street', 'erakimolstrasse 56'
'struct street', 'fabelweg 67'
'struct street', 'fabelweg 67'
'struct street', 'hurtika 89'
'struct street', 'hurtika 89'
'struct town', 'bendorf'
'struct town', 'carassa'
'struct town', 'durnfo'
'struct town', 'hof'
'struct town', 'carassa'
'struct town', 'durnfo'
'struct town', 'enden'
'struct town', 'hof'
'struct town', 'durnfo'
'struct town', 'enden'
'struct town', 'formkon'
'struct town', 'hof'
'struct town', 'enden'
'struct town', 'formkon'
'struct town', 'formkon'
'struct town', 'hof'
'struct town', 'hof'
'struct surname', 'alano'
'struct surname', 'beret'
'struct surname', 'carlson'
'struct surname', 'dubi'
'struct surname', 'ertki'
'struct surname', 'fuioko'
'struct surname', 'golto'
'struct surname', 'hauer'
'struct prename', 'aufru'
'struct prename', 'beno'
'struct prename', 'carla'
'struct prename', 'dorothe'
'struct prename', 'erik'
'struct prename', 'fran'
'struct prename', 'gerd'
'struct prename', 'hubert'
'company name', 'baluba inc'
'company name', 'carimba inc'
'company name', 'dereno inc'
'company name', 'huratz inc'
'company name', 'carimba inc'
'company name', 'dereno inc'
'company name', 'etungo inc'
'company name', 'huratz inc'
'company name', 'dereno inc'
'company name', 'etungo inc'
'company name', 'figaji inc'
'company name', 'huratz inc'
'company name', 'etungo inc'
'company name', 'figaji inc'
'company name', 'figaji inc'
'company name', 'huratz inc'
'company name', 'huratz inc'

NumberTable:
'struct tag', '1101'
'struct tag', '1101'
'struct tag', '1101'
'struct tag', '1101'
'struct tag', '1101'
'struct tag', '1101'
'struct tag', '1101'
'struct tag', '1101'
'struct id', '101'
'struct id', '102'
'struct id', '103'
'struct id', '104'
'struct id', '105'
'struct id', '106'
'struct id', '107'
'struct id', '108'

**end

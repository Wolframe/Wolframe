CREATE TABLE Person
(
 ID INTEGER NOT NULL PRIMARY KEY,
 prename VARCHAR2(30),
 surname VARCHAR2(30)
);
CREATE SEQUENCE Person_ID_Seq START WITH 1 INCREMENT BY 1;
CREATE TRIGGER Person_Insert
BEFORE INSERT ON Person
FOR EACH ROW
BEGIN
	SELECT Person_ID_Seq.nextval into :new.id FROM dual;
END;
/

CREATE TABLE Address
(
 ID INTEGER NOT NULL PRIMARY KEY,
 street VARCHAR2(30),
 town VARCHAR2(30)
);
CREATE SEQUENCE Address_ID_Seq START WITH 1 INCREMENT BY 1;
CREATE TRIGGER Address_Insert
BEFORE INSERT ON Address
FOR EACH ROW
BEGIN
	SELECT Address_ID_Seq.nextval into :new.id FROM dual;
END;
/

CREATE TABLE Company
(
 ID INTEGER NOT NULL PRIMARY KEY,
 name VARCHAR2(30)
);
CREATE SEQUENCE Company_ID_Seq START WITH 1 INCREMENT BY 1;
CREATE TRIGGER Company_Insert
BEFORE INSERT ON Company
FOR EACH ROW
BEGIN
	SELECT Company_ID_Seq.nextval into :new.id FROM dual;
END;
/


CREATE TABLE PersonChildRel
(
 ID INTEGER,
 childid  INTEGER
);

CREATE TABLE PersonAddressRel
(
 ID INTEGER,
 addressid  INTEGER
);

CREATE TABLE CompanyChildRel
(
 ID INTEGER,
 childid  INTEGER
);

CREATE TABLE CompanyAddressRel
(
 ID INTEGER,
 addressid  INTEGER
);

CREATE TABLE PersonCompanyRel
(
 ID INTEGER,
 companyid  INTEGER
);

CREATE TABLE WordTable
(
 name VARCHAR2(30),
 word VARCHAR2(30)
);

CREATE TABLE NumberTable
(
 name VARCHAR2(30),
 "number" INTEGER
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
INSERT INTO PersonChildRel (ID,childid) VALUES (7,8);
INSERT INTO PersonChildRel (ID,childid) VALUES (1,8);
INSERT INTO PersonChildRel (ID,childid) VALUES (2,8);
INSERT INTO PersonChildRel (ID,childid) VALUES (3,8);

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
INSERT INTO CompanyChildRel (ID,childid) VALUES (7,8);
INSERT INTO CompanyChildRel (ID,childid) VALUES (1,8);
INSERT INTO CompanyChildRel (ID,childid) VALUES (2,8);
INSERT INTO CompanyChildRel (ID,childid) VALUES (3,8);

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
INSERT INTO PersonAddressRel (ID,addressid) VALUES (7,8);
INSERT INTO PersonAddressRel (ID,addressid) VALUES (1,8);
INSERT INTO PersonAddressRel (ID,addressid) VALUES (2,8);
INSERT INTO PersonAddressRel (ID,addressid) VALUES (3,8);

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
INSERT INTO CompanyAddressRel (ID,addressid) VALUES (7,8);
INSERT INTO CompanyAddressRel (ID,addressid) VALUES (1,8);
INSERT INTO CompanyAddressRel (ID,addressid) VALUES (2,8);
INSERT INTO CompanyAddressRel (ID,addressid) VALUES (3,8);

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
INSERT INTO PersonCompanyRel (ID,companyid) VALUES (7,8);
INSERT INTO PersonCompanyRel (ID,companyid) VALUES (1,8);
INSERT INTO PersonCompanyRel (ID,companyid) VALUES (2,8);
INSERT INTO PersonCompanyRel (ID,companyid) VALUES (3,8);



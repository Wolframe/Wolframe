
CREATE TABLE Customer (
 ID SERIAL NOT NULL PRIMARY KEY,
 name TEXT ,
 CONSTRAINT tag_name_check UNIQUE( name )
);


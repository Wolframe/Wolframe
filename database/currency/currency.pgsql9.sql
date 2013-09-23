-- PostgreSQL 9
--
-- Currency manipulation

-- List of currencies
-- The format of this table will change in the future
-- Although the changes should be cosmetic only
-- (adding subdivisions, local name etc.)
CREATE TABLE Currency	(
	Code		CHAR( 3 )	PRIMARY KEY NOT NULL,	-- ISO code
	Name		TEXT		NOT NULL
);


CREATE TABLE ExchangeRate	(
	Currency	CHAR( 3 )	NOT NULL REFERENCES Currency( Code ),
	Reference	CHAR( 3 )	NOT NULL REFERENCES Currency( Code ),
	Rate		NUMERIC( 10,3 )	NOT NULL,
	Multiplier	INT		NOT NULL DEFAULT( 1 ),
	StartTime	TIMESTAMP	NOT NULL DEFAULT( 'now' ),
	EndTime		TIMESTAMP	NOT NULL DEFAULT( 'infinity' )
);

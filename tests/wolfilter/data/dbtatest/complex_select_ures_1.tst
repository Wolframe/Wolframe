**
**requires:TEXTWOLF
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<root><item><aa>1</aa></item><item></item><item><bb></bb></item></root>
**config
--module ../../src/modules/filter/textwolf/mod_filter_textwolf
--module ../wolfilter/modules/database/testtrace/mod_db_testtrace
--filter 'textwolf'
--database 'identifier=testdb,outfile=DBOUT,file=DBRES'
--program=DBIN.tdl
testcall
**file:DBIN.tdl
TRANSACTION testcall RESULT INTO result BEGIN
	FOREACH //aa DO run(.);
	FOREACH RESULT INTO person DO call( //aa , $1 ) ;
	FOREACH RESULT INTO age DO get ( //aa,$1 ,$2 ,$3);
END
**file: DBRES
#res#1#2
#vorname name beruf#karin fischer beamte
#vorname name beruf#peter mueller polizist
#alter#42
#alter#31
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<result><person><vorname>karin</vorname><name>fischer</name><beruf>beamte</beruf></person><person><vorname>peter</vorname><name>mueller</name><beruf>polizist</beruf></person><age><alter>42</alter></age><age><alter>31</alter></age></result>
run #1
call #1#[1]
get #1#[1]#[2]#[3]

start( 'run' );
bind( 1, '1' );
execute();
nofColumns(); returns 1
columnName( 1); returns res
get( 1 ); returns 1
next(); returns 1
get( 1 ); returns 2
next(); returns 0
start( 'call' );
bind( 1, '1' );
bind( 2, '1' );
execute();
nofColumns(); returns 3
columnName( 1); returns vorname
columnName( 2); returns name
columnName( 3); returns beruf
get( 1 ); returns karin
get( 2 ); returns fischer
get( 3 ); returns beamte
next(); returns 0
start( 'call' );
bind( 1, '1' );
bind( 2, '2' );
execute();
nofColumns(); returns 3
get( 1 ); returns peter
get( 2 ); returns mueller
get( 3 ); returns polizist
next(); returns 0
start( 'get' );
bind( 1, '1' );
bind( 2, 'karin' );
bind( 3, 'fischer' );
bind( 4, 'beamte' );
execute();
nofColumns(); returns 1
columnName( 1); returns alter
get( 1 ); returns 42
next(); returns 0
start( 'get' );
bind( 1, '1' );
bind( 2, 'peter' );
bind( 3, 'mueller' );
bind( 4, 'polizist' );
execute();
nofColumns(); returns 1
get( 1 ); returns 31
next(); returns 0
**end

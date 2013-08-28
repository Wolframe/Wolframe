**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><item></item><item><aa>1</aa><bb>2</bb></item><item><aa>11</aa><bb>22</bb></item><item><aa>111</aa><bb>222</bb></item><item><bb></bb></item></doc>
**config
--module ../../src/modules/filter/textwolf/mod_filter_textwolf
--module ../wolfilter/modules/database/testtrace/mod_db_testtrace
--filter 'xml:textwolf'
--database 'identifier=testdb,outfile=DBOUT,file=DBRES'
--program=DBIN.tdl
testcall
**file:DBIN.tdl
TRANSACTION testcall
RESULT INTO da
BEGIN
	FOREACH /doc/item/aa DO run(. ,../bb ) ;
	INTO da DO get($1);
	FOREACH //bb INTO do DO run( $1 );
END
**file: DBRES
#res#1
#res#2
#res#3
#res#a
#res#b
#res#c
#res#111
#res#222
#res#333
**outputfile:DBOUT
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<da><da><res>a</res></da><da><res>b</res></da><da><res>c</res></da><do><res>111</res></do><do><res>222</res></do><do><res>333</res></do></da>
run #1#2
run #11#22
run #111#222
get #[1]
run #[1]
run #[1]
run #[1]
run #[1]

start( 'run' );
bind( 1, '1' );
bind( 2, '2' );
execute();
nofColumns(); returns 1
columnName( 1); returns res
get( 1 ); returns 1
next(); returns 0
start( 'run' );
bind( 1, '11' );
bind( 2, '22' );
execute();
nofColumns(); returns 1
get( 1 ); returns 2
next(); returns 0
start( 'run' );
bind( 1, '111' );
bind( 2, '222' );
execute();
nofColumns(); returns 1
get( 1 ); returns 3
next(); returns 0
start( 'get' );
bind( 1, '1' );
execute();
nofColumns(); returns 1
columnName( 1); returns res
get( 1 ); returns a
next(); returns 0
start( 'get' );
bind( 1, '2' );
execute();
nofColumns(); returns 1
get( 1 ); returns b
next(); returns 0
start( 'get' );
bind( 1, '3' );
execute();
nofColumns(); returns 1
get( 1 ); returns c
next(); returns 0
start( 'run' );
bind( 1, 'a' );
execute();
nofColumns(); returns 1
columnName( 1); returns res
get( 1 ); returns 111
next(); returns 0
start( 'run' );
bind( 1, 'b' );
execute();
nofColumns(); returns 1
get( 1 ); returns 222
next(); returns 0
start( 'run' );
bind( 1, 'c' );
execute();
nofColumns(); returns 1
get( 1 ); returns 333
next(); returns 0
start( 'run' );
bind( 1, 'a' );
execute();
nofColumns(); returns 0
start( 'run' );
bind( 1, 'b' );
execute();
nofColumns(); returns 0
start( 'run' );
bind( 1, 'c' );
execute();
nofColumns(); returns 0
start( 'run' );
bind( 1, 'a' );
execute();
nofColumns(); returns 0
start( 'run' );
bind( 1, 'b' );
execute();
nofColumns(); returns 0
start( 'run' );
bind( 1, 'c' );
execute();
nofColumns(); returns 0
start( 'run' );
bind( 1, 'a' );
execute();
nofColumns(); returns 0
start( 'run' );
bind( 1, 'b' );
execute();
nofColumns(); returns 0
start( 'run' );
bind( 1, 'c' );
execute();
nofColumns(); returns 0
**end

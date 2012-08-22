**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<main><item><aa>1</aa><cc>3</cc><bb>2</bb><item><item><aa>11</aa><cc>33</cc><bb>22</bb><item><item><aa>111</aa><cc>333</cc><bb>222</bb><item></main>
**config
run(//aa: ., ./../bb, ../aa/../cc)
**output
begin();
start( 'run' );
bind( 1, '1' );
bind( 2, '2' );
bind( 3, '3' );
execute();
nofColumns();
next();
getLastError();
bind( 1, '11' );
bind( 2, '22' );
bind( 3, '33' );
execute();
nofColumns();
next();
getLastError();
bind( 1, '111' );
bind( 2, '222' );
bind( 3, '333' );
execute();
nofColumns();
next();
getLastError();
commit();
**end

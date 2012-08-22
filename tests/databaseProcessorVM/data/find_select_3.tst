**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<main><aa>1</aa><cc>3</cc><bb>2</bb></main>
**config
run(: //aa, ./main/bb, //cc)
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
commit();
**end

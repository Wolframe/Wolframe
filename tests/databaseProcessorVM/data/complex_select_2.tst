**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><item></item><item><aa>1</aa><bb>2</bb></item><item><aa>11</aa><bb>22</bb></item><item><aa>111</aa><bb>222</bb></item><item><bb></bb></item></doc>
**config
run(/doc/item/aa: ., ../bb)
**output
begin();
start( 'run' );
bind( 1, '1' );
bind( 2, '2' );
execute();
nofColumns();
next();
getLastError();
bind( 1, '11' );
bind( 2, '22' );
execute();
nofColumns();
next();
getLastError();
bind( 1, '111' );
bind( 2, '222' );
execute();
nofColumns();
next();
getLastError();
commit();
**end

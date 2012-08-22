**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><aa>1</aa><bb>2</bb></doc>
**config
run(: /doc/aa, //bb)
**output
begin();
start( 'run' );
bind( 1, '1' );
bind( 2, '2' );
execute();
nofColumns();
next();
getLastError();
commit();
**end

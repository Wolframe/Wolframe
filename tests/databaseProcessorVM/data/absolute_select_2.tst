**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<aa>1</aa><bb>2</bb>
**config
run(: /aa, /bb)
**output
begin();
start( 'run' );
bind( 1, '1' );
bind( 2, '2' );
execute();
commit();
**end

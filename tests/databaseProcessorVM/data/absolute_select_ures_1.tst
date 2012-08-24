**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<aa>1</aa>
**config
run(: /aa); exec(: $1)
**file: 1.result
id
2
**output
begin();
start( 'run' );
bind( 1, '1' );
execute();
nofColumns(); returns 1
get( 1 ); returns 2
next(); returns 0
getLastError(); returns 0
start( 'exec' );
bind( 1, '2' );
execute();
commit();
**end

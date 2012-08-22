**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<root><aa>1</aa></root>
**config
run(//aa: .)
**output
begin();
start( 'run' );
bind( 1, '1' );
execute();
nofColumns();
next();
getLastError();
commit();
**end

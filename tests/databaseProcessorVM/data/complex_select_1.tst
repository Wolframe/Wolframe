**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<root><item><aa>1</aa></item><item></item><item><bb></bb></item></root>
**config
run(//aa: ../aa)
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

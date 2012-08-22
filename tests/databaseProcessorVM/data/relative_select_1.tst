**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<guru>ein parameter text</guru>
**config
run(: guru)
**output
begin();
start( 'run' );
bind( 1, 'ein parameter text' );
execute();
nofColumns();
next();
getLastError();
commit();
**end

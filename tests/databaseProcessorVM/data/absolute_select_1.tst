**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<aa>1</aa>
**config
run(: /aa)
**output
begin();
start( 'run' );
bind( 1, '1' );
execute();
commit();
**end

**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><main><aa>1</aa><cc>3</cc><bb>2</bb><dd></dd></main></doc>
**config
run(: .//aa/., ./doc/main/bb/., //cc/., ./doc/main//dd)
**output
begin();
start( 'run' );
bind( 1, '1' );
bind( 2, '2' );
bind( 3, '3' );
bind( 4, '' );
execute();
nofColumns();
next();
getLastError();
commit();
**end

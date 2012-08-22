**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><item><aa>1</aa><cc>3</cc><bb>2</bb><dd></dd></item><item><aa>11</aa><cc>33</cc><bb></bb><dd>44</dd></item></doc>
**config
run(//item/aa: ., ../bb, .././cc, ..//dd/.)
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
bind( 1, '11' );
bind( 2, '' );
bind( 3, '33' );
bind( 4, '44' );
execute();
nofColumns();
next();
getLastError();
commit();
**end

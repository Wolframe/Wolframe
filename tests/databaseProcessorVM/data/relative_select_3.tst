**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<garten>1</garten><wald>3</wald><wiese>2</wiese>
**config
run(: wald, wiese, garten)
**output
begin();
start( 'run' );
bind( 1, '3' );
bind( 2, '2' );
bind( 3, '1' );
execute();
commit();
**end

**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<baum>hier ist ein baum text</baum><haus>hier ist ein haus text</haus>
**config
run(: haus, baum)
**output
begin();
start( 'run' );
bind( 1, 'hier ist ein haus text' );
bind( 2, 'hier ist ein baum text' );
execute();
commit();
**end

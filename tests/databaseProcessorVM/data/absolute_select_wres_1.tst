**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<aa>1</aa>
**config
result=run(: /aa)
**file: 1.result
id
1
**output
begin();
start( 'run' );
bind( 1, '1' );
execute();
nofColumns(); returns 1
get( 0 ); returns 1
next(); returns 0
getLastError(); returns 0
nofColumns(); returns 1
columnName( 0 ); returns id
commit();

result {
  id {
    '1'
  }
}
**end

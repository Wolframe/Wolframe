**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<aa>1</aa><bb>2</bb>
**config
result/item=run(: /aa, /bb)
**file: 1.result
id name
1 hugo
**output
begin();
start( 'run' );
bind( 1, '1' );
bind( 2, '2' );
execute();
nofColumns(); returns 2
get( 1 ); returns 1
get( 2 ); returns hugo
next(); returns 0
getLastError(); returns 0
nofColumns(); returns 2
columnName( 1 ); returns id
columnName( 2 ); returns name
commit();

result {
  item {
    id {
      '1'
    }
    name {
      'hugo'
    }
  }
}
**end
**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<aa>1</aa><bb>2</bb>
**config
result/item=run( : /aa , /bb ); exec ( : $1,$2)
**file: 1.result
id name
1 hugo
2 barbara
**output
begin();
start( 'run' );
bind( 1, '1' );
bind( 2, '2' );
execute();
nofColumns(); returns 2
get( 1 ); returns 1
get( 2 ); returns hugo
next(); returns 1
get( 1 ); returns 2
get( 2 ); returns barbara
next(); returns 0
getLastError(); returns 0
nofColumns(); returns 2
columnName( 1 ); returns id
columnName( 2 ); returns name
start( 'exec' );
bind( 1, '1' );
bind( 2, 'hugo' );
execute();
bind( 1, '2' );
bind( 2, 'barbara' );
execute();
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
  item {
    id {
      '2'
    }
    name {
      'barbara'
    }
  }
}
**end

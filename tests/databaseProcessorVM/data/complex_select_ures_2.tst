**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><item></item><item><aa>1</aa><bb>2</bb></item><item><aa>11</aa><bb>22</bb></item><item><aa>111</aa><bb>222</bb></item><item><bb></bb></item></doc>
**config
run(/doc/item/aa : . ,../bb ) ; da/da =get(:$1);  da/do=run(//bb:$1 )
**file: 1.result
res
1
**file: 2.result
res
2
**file: 3.result
res
3
**file: 4.result
res
a
**file: 5.result
res
b
**file: 6.result
res
c
**file: 7.result
res
111
**file: 8.result
res
222
**file: 9.result
res
333
**output
begin();
start( 'run' );
bind( 1, '1' );
bind( 2, '2' );
execute();
nofColumns(); returns 1
get( 1 ); returns 1
next(); returns 0
getLastError(); returns 0
bind( 1, '11' );
bind( 2, '22' );
execute();
nofColumns(); returns 1
get( 1 ); returns 2
next(); returns 0
getLastError(); returns 0
bind( 1, '111' );
bind( 2, '222' );
execute();
nofColumns(); returns 1
get( 1 ); returns 3
next(); returns 0
getLastError(); returns 0
start( 'get' );
bind( 1, '1' );
execute();
nofColumns(); returns 1
get( 1 ); returns a
next(); returns 0
getLastError(); returns 0
nofColumns(); returns 1
columnName( 1 ); returns res
bind( 1, '2' );
execute();
nofColumns(); returns 1
get( 1 ); returns b
next(); returns 0
getLastError(); returns 0
nofColumns(); returns 1
columnName( 1 ); returns res
bind( 1, '3' );
execute();
nofColumns(); returns 1
get( 1 ); returns c
next(); returns 0
getLastError(); returns 0
nofColumns(); returns 1
columnName( 1 ); returns res
start( 'run' );
bind( 1, 'a' );
execute();
nofColumns(); returns 1
get( 1 ); returns 111
next(); returns 0
getLastError(); returns 0
nofColumns(); returns 1
columnName( 1 ); returns res
bind( 1, 'b' );
execute();
nofColumns(); returns 1
get( 1 ); returns 222
next(); returns 0
getLastError(); returns 0
nofColumns(); returns 1
columnName( 1 ); returns res
bind( 1, 'c' );
execute();
nofColumns(); returns 1
get( 1 ); returns 333
next(); returns 0
getLastError(); returns 0
nofColumns(); returns 1
columnName( 1 ); returns res
bind( 1, 'a' );
execute();
nofColumns(); returns 0
getLastError(); returns 0
nofColumns(); returns 0
bind( 1, 'b' );
execute();
nofColumns(); returns 0
getLastError(); returns 0
nofColumns(); returns 0
bind( 1, 'c' );
execute();
nofColumns(); returns 0
getLastError(); returns 0
nofColumns(); returns 0
bind( 1, 'a' );
execute();
nofColumns(); returns 0
getLastError(); returns 0
nofColumns(); returns 0
bind( 1, 'b' );
execute();
nofColumns(); returns 0
getLastError(); returns 0
nofColumns(); returns 0
bind( 1, 'c' );
execute();
nofColumns(); returns 0
getLastError(); returns 0
nofColumns(); returns 0
bind( 1, 'a' );
execute();
nofColumns(); returns 0
getLastError(); returns 0
nofColumns(); returns 0
bind( 1, 'b' );
execute();
nofColumns(); returns 0
getLastError(); returns 0
nofColumns(); returns 0
bind( 1, 'c' );
execute();
nofColumns(); returns 0
getLastError(); returns 0
nofColumns(); returns 0
commit();

da {
  da {
    res {
      'a'
    }
  }
  da {
    res {
      'b'
    }
  }
  da {
    res {
      'c'
    }
  }
  do {
    res {
      '111'
    }
  }
  do {
    res {
      '222'
    }
  }
  do {
    res {
      '333'
    }
  }
}
**end

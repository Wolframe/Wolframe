**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<root><item><aa>1</aa></item><item></item><item><bb></bb></item></root>
**config
run(//aa: .); result/person=call(//aa: . , $1 ) ; result/age =get ( //aa : .,$1 ,$2 ,$3)
**file: 1.result
res
1
2
**file: 2.result
vorname name beruf
karin fischer beamte
**file: 3.result
vorname name beruf
peter mueller polizist
**file: 4.result
alter
42
**file: 5.result
alter
31
**output
begin();
start( 'run' );
bind( 1, '1' );
execute();
nofColumns(); returns 1
get( 1 ); returns 1
next(); returns 1
get( 1 ); returns 2
next(); returns 0
getLastError(); returns 0
start( 'call' );
bind( 1, '1' );
bind( 2, '1' );
execute();
nofColumns(); returns 3
get( 1 ); returns karin
get( 2 ); returns fischer
get( 3 ); returns beamte
next(); returns 0
getLastError(); returns 0
nofColumns(); returns 3
columnName( 1 ); returns vorname
columnName( 2 ); returns name
columnName( 3 ); returns beruf
bind( 2, '2' );
execute();
nofColumns(); returns 3
get( 1 ); returns peter
get( 2 ); returns mueller
get( 3 ); returns polizist
next(); returns 0
getLastError(); returns 0
nofColumns(); returns 3
columnName( 1 ); returns vorname
columnName( 2 ); returns name
columnName( 3 ); returns beruf
start( 'get' );
bind( 1, '1' );
bind( 2, 'karin' );
bind( 3, 'fischer' );
bind( 4, 'beamte' );
execute();
nofColumns(); returns 1
get( 1 ); returns 42
next(); returns 0
getLastError(); returns 0
nofColumns(); returns 1
columnName( 1 ); returns alter
bind( 2, 'peter' );
bind( 3, 'mueller' );
bind( 4, 'polizist' );
execute();
nofColumns(); returns 1
get( 1 ); returns 31
next(); returns 0
getLastError(); returns 0
nofColumns(); returns 1
columnName( 1 ); returns alter
commit();

result {
  person {
    vorname {
      'karin'
    }
    name {
      'fischer'
    }
    beruf {
      'beamte'
    }
  }
  person {
    vorname {
      'peter'
    }
    name {
      'mueller'
    }
    beruf {
      'polizist'
    }
  }
  age {
    alter {
      '42'
    }
  }
  age {
    alter {
      '31'
    }
  }
}
**end

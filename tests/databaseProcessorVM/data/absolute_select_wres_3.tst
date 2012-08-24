**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<aa>1</aa><cc>3</cc><bb>2</bb>
**config
doc/address=run(: /aa, /bb, /cc)
**file: 1.result
id name street
1 hugo "bahnhofstrasse 15"
2 miriam "zum gemsweg 3"
3 sara "tannensteig 12"
**output
begin();
start( 'run' );
bind( 1, '1' );
bind( 2, '2' );
bind( 3, '3' );
execute();
nofColumns(); returns 3
get( 1 ); returns 1
get( 2 ); returns hugo
get( 3 ); returns bahnhofstrasse 15
next(); returns 1
get( 1 ); returns 2
get( 2 ); returns miriam
get( 3 ); returns zum gemsweg 3
next(); returns 1
get( 1 ); returns 3
get( 2 ); returns sara
get( 3 ); returns tannensteig 12
next(); returns 0
getLastError(); returns 0
nofColumns(); returns 3
columnName( 1 ); returns id
columnName( 2 ); returns name
columnName( 3 ); returns street
commit();

doc {
  address {
    id {
      '1'
    }
    name {
      'hugo'
    }
    street {
      'bahnhofstrasse 15'
    }
  }
  address {
    id {
      '2'
    }
    name {
      'miriam'
    }
    street {
      'zum gemsweg 3'
    }
  }
  address {
    id {
      '3'
    }
    name {
      'sara'
    }
    street {
      'tannensteig 12'
    }
  }
}
**end

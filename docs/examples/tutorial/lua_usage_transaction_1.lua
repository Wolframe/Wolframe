
in = form( "employee", true )
out = form( "id", true )
res = out( transaction( "insertEmployee")( in( {surname='Hans', name='Muster', company='Wolframe'} )))
output:print( res[ "id" ] )

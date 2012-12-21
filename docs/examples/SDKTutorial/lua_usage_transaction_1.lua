
f = formfunction( "insertEmployee")
res = f ( {surname='Hans', name='Muster', company='Wolframe'} )
t = res:table()
output:print( t[ "id" ] )

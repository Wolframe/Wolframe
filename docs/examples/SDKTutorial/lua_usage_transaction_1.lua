
f = provider.formfunction( "insertEmployee")
res = f ( {surname='Hans', name='Muster', company='Wolframe'} )
t = res:value()
output:print( t[ "id" ] )

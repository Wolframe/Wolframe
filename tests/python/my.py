from time import time, ctime

def func( i, f ):
    print( 'func', i, f )
    return i * f

print( 'Module initialized on ' , ctime( time( ) ) )

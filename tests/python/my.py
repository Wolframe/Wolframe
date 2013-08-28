from time import time, ctime

def func( i, f ):
    print( 'func', i, f )
    return i * f

time = 1377677003.4642606
print( 'Module initialized on ' , ctime( time ) )

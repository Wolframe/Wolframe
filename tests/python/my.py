from time import time, ctime

def func( i, f ):
    print( 'func', i, f )
    return i * f

print( 'Module initialized on ' , ctime( time( ) ) )

class test:
	def __init__( self ):
		self.c = 5
		
	def method( self, i, f ):
		print( 'class.method', i, f )
		return slef.c * i * f
	

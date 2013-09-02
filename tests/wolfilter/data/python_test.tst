**
**requires:PYTHON
**input
**output
**file:test.py
from time import time, ctime

def func( ):
	print( 'Hello Python' )
    
def func2( i ):
	return i * i

print( 'Module initialized on ' , ctime( time( ) ) )


**config
--module ../../src/modules/cmdbind/python/mod_command_python
--program test.py run
**end

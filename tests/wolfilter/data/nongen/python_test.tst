**
**requires:PYTHON
**requires:DISABLED
**input
**output
**file:test.py
from time import time, ctime

def func( ):
	return None
    
def func2( i ):
	return i * i

def func3( f ):
	return f * f

print( 'Module initialized on ' , ctime( time( ) ) )

def run():
	print( 'Hello Python' )


**config
--config wolframe.conf run
**file:wolframe.conf
LoadModules
{
	module ../../src/modules/cmdbind/python/mod_command_python
}
Processor
{
	program test.py
}
**end

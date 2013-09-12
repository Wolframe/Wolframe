from time import time, ctime
import sys

def func( i, f ):
    print( 'func', i, f )
    return i * f

# this is 'ANSI_X3.4-1968' on Linux, 646 on NetBSD/Solaris
#print( sys.stdout.encoding )

# test UTF-8 identifiers (especially important for function names to work
répertoire = "/tmp/records.log"
print( 'repertoire: ', répertoire )

# output Unicode string
funny = 'α♭αʊмαᾔᾔ αт ⑂α♄øø ∂◎т ¢☺м'
print( funny.encode( 'ascii', 'backslashreplace') )

# this works only if PYTHONIOENCODING=utf-8 and LANG=en_US.utf8 is set
#print( funny )

time = 1377677003.4642606
x = ctime( time )

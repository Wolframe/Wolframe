from time import time, ctime

def func( i, f ):
    print( 'func', i, f )
    return i * f

# test UTF-8 identifiers (especially important for function names to work
répertoire = "/tmp/records.log"
print( 'repertoire: ', répertoire )

# output Unicode string
funny = 'Ѧηḓґℯα﹩ ℬ@üмαηη αт ℯʊґøṧ℘їḓ℮ґ▣ḉ◎м'
print( funny.encode('ascii', 'backslashreplace') )

time = 1377677003.4642606
print( 'Module initialized on ' , ctime( time ) )

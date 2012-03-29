#!/usr/bin/lua

gmp = require "gmp"

a = gmp.z( arg[1] )
b = gmp.z( arg[2] )
c = a:add( b )

print( a .. " + " .. b .. " => " .. c )

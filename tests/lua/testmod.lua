-- Lua part of the test module

local require = require

local _ENV = {}

local prv = {}
local aux = {}
require "c-testmod" (prv, aux)

function func( s, i, f )
	return prv.func( s, i, f )
end

return _ENV

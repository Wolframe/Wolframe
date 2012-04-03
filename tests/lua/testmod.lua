-- Lua part of the test module

local require = require

local _ENV = {}

local prv = {}
local aux = {}
prv = require "c-testmod"

function func( s, i, f )
	return prv.func( s, i, f )
end

return _ENV

-- called when a new LUA server is started
function init( )
	io.write( "LUA: init called.\n" )
end

-- called when the LUA server is stopped
function destroy( )
	io.write( "LUA: destroy called.\n" )
end

-- called when a client connection gets established
function new_connection( remote_host, remote_port, common_name )
	io.write( "LUA: new_connection called from " .. remote_host .. " (port: " .. remote_port .. ")\n" )
	if( common_name ) then
		io.write( "LUA: encrypted connection, CN is " .. common_name .. "\n" )
	end
end

-- called when receiving a line a data
function got_line( s )
	if s == "quit" then
		-- TODO: how to signal end?
	end
	io.write( "LUA got line: " .. s .. "\n" )
end

-- the main, initialize globals here
io.write( "LUA: main called.\n" )

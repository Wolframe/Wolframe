-- called when a new LUA server is started
function init( )
	io.write( "lua init called.\n" )
end

-- called when the LUA server is stopped
function destroy( )
	io.write( "lua destroy called.\n" )
end

-- the main, initialize globals here
io.write( "lua main called.\n" )

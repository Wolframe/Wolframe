-- global variable
state = "INIT"
buffer = ""

-- called when a new LUA server is started
function init( )
	io.write( "LUA: init called.\n" )
	state = "INIT"
	buffer = ""
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

-- handle a request and produce a reply
function next_operation( )
	io.write( "LUA next operation\n" )
	
	if state == "NEW" then
		state = "HELLO"
		return WRITE, "Welcome to SMERP.\n"
 	else if state == "HELLO" then
		state = "ANSWERING"
		if string.len( buffer ) == 0 then
			return WRITE, buffer
		else
			return WRITE, "BUFFER NOT EMPTY!\n"
		end
	else if state == "READING" then
		state = ANSWERING
		if string.len( buffer ) > 0 then
			return WRITE, buffer
		else
			return WRITE, "EMTPY BUFFER!\n"
		end
	else if state == "ANSWERING" then
		buffer = ""
		state = "READING"
		return READ, 30
	else if state == "FINISHING" then
		state = "TERMINATING"
		return WRITE, "Thanks for using SMERP.\n"
	else if state == "TIMEOUT" then
		state = TERMINATING
		return WRITE, "Timeout. :P\n"
	else if state == "SIGNALLED" then
		state = TERMINATING
		return WRITE, "Server is shutting down. :P\n";
	else if state == "TERMINATING"  then
		return "TERMINATE", 0
	else
		io.write( "Illegal state " . state . "!!" )
	end
end

-- a timeout occured
function timeout_occured( )
	state = "TIMEOUT"
end

-- a signal (Ctrl-C) occured
function aignal_occured( )
	state = "SIGNALLED"
end

-- called when receiving a line a data
function parse_input( data )
	io.write( "LUA got line: " .. s .. "\n" )
	if s == "quit" then
		-- signals end
		return false
	end
	return true
end

	void* echoConnection::parseInput( const void *begin, std::size_t bytesTransferred )
	{
		char *s = (char *)begin;
		for( std::size_t i = 0; i < bytesTransferred; i++ ) {
			if( *s != '\n' ) {
				buffer_ += *s;
			} else {
				// don't pass the end of line to Lua
				s++;
				lua_pushstring( l, "got_line" );
				lua_gettable( l, LUA_GLOBALSINDEX );
				lua_pushstring( l, buffer_.c_str( ) );
				int res = lua_pcall( l, 1, 1, 0 );
				if( res != 0 ) {
					LOG_FATAL << "Unable to call 'got_line' function: " << lua_tostring( l, -1 );
					lua_pop( l, 1 );
					throw new std::runtime_error( "Error in LUA processor" );
				}
				bool cont = lua_toboolean( l, lua_gettop( l ) );
				if( !cont ) {
					state_ = FINISHING;
					return s;
				} else {
					buffer_ += '\n';
					return s;
				}
			}
			s++;
		}
		return s;
	}

-- the main, initialize globals here
io.write( "LUA: main called.\n" )

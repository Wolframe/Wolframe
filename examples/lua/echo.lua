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
	state = "NEW"
end

-- handle a request and produce a reply
function next_operation( )
	io.write( "LUA next operation\n" )
	
	if state == "NEW" then
		state = "HELLO"
		return WRITE, "Welcome to SMERP.\n"
 	elseif state == "HELLO" then
		state = "ANSWERING"
		if string.len( buffer ) == 0 then
			return WRITE, buffer
		else
			return WRITE, "BUFFER NOT EMPTY!\n"
		end
	elseif state == "READING" then
		state = ANSWERING
		if string.len( buffer ) > 0 then
			return WRITE, buffer
		else
			return WRITE, "EMTPY BUFFER!\n"
		end
	elseif state == "ANSWERING" then
		buffer = ""
		state = "READING"
		return READ, 30
	elseif state == "FINISHING" then
		state = "TERMINATING"
		return WRITE, "Thanks for using SMERP.\n"
	elseif state == "TIMEOUT" then
		state = TERMINATING
		return WRITE, "Timeout. :P\n"
	elseif state == "SIGNALLED" then
		state = TERMINATING
		return WRITE, "Server is shutting down. :P\n";
	elseif state == "TERMINATING"  then
		return "TERMINATE", 0
	else
		io.write( "Illegal state " .. state .. "!!" )
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
	io.write( "LUA got data: " .. data .. "\n" )

	buffer = buffer + s
	pos = string.find( buffer, "\n" )
	if pos then
		s = string.sub( buffer, 0, pos )
		buffer = string.sub( buffer, pos+1 )

		if s == "quit" then
			state = "FINISHING"
		end
	else
		-- buffer doesn't contain a newline, wait a little longer
	end
end

-- the main, initialize globals here
io.write( "LUA: main called.\n" )

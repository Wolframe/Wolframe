-- includes
require "smerplogger"

-- global variables
state = "INIT"
buffer = ""

-- called when a new LUA server is started
function init( )
	smerplogger.write( "ERROR", "LUA: init called." )
	state = "INIT"
	buffer = ""
end

-- called when the LUA server is stopped
function destroy( )
	smerplogger.write( "ERROR", "LUA: destroy called." )
end

-- called when a client connection gets established
function new_connection( remote_host, remote_port, common_name )
	smerplogger.write( "ERROR",  "LUA: new_connection called from " .. remote_host .. " (port: " .. remote_port .. ")" )
	if( common_name ) then
		--io.write( "LUA: encrypted connection, CN is " .. common_name .. "\n" )
	end
	state = "NEW"
end

-- handle a request and produce a reply
function next_operation( )
	--smerplogger.write( "ERROR", "LUA next operation" )
	
	if state == "NEW" then
		state = "HELLO"
		return "WRITE", "Welcome to SMERP.\n"
 	elseif state == "HELLO" then
		if string.len( buffer ) == 0 then
			state = "READING"
			return "READ", 30
		else
			state = "ANSWERING"
			return "WRITE", "BUFFER NOT EMPTY!\n"
		end
	elseif state == "READING" then
		state = "ANSWERING"
		if string.len( buffer ) > 0 then
			return "WRITE", buffer
		else
			return "WRITE", "EMTPY BUFFER!\n"
		end
	elseif state == "ANSWERING" then
		buffer = ""
		state = "READING"
		return "READ", 30
	elseif state == "FINISHING" then
		state = "CLOSING"
		return "WRITE", "Thanks for using SMERP.\n"
	elseif state == "TIMEOUT" then
		state = "CLOSING"
		return "WRITE", "Timeout. :P\n"
	elseif state == "SIGNALLED" then
		state = "CLOSING"
		return "WRITE", "Server is shutting down. :P\n"
	elseif state == "CLOSING" then
		state = "TERMINATED"
		return "CLOSE"
	elseif state == "TERMINATING"  then
		state = "TERMINATED"
		return "TERMINATE"
	elseif state == "TERMINATED" then
		smerplogger.write( "ERROR", "LUA: got a nextOperation while in TERMINATED state" )
		return "TERMINATE"
	else
		smerplogger.write( "FATAL", "LUA: Illegal state " .. state .. "!!" )
	end
end

-- a timeout occured
function timeout_occured( )
	smerplogger.write( "ERROR", "LUA: got timeout" )
	state = "TIMEOUT"
end

-- a signal (Ctrl-C) occured
function signal_occured( )
	smerplogger.write( "ERROR", "LUA: got signalled" )
	state = "SIGNALLED"
end

-- an error occured
function error_occured( error )
	smerplogger.write( "ERROR", "LUA: got error ", error )
	state = "CLOSING"
end

-- called when receiving a line a data
function network_input( data )
	smerplogger.write( "ERROR", "LUA: Got ", string.len( data ), " bytes of data" )

	buffer = buffer .. data
	pos = string.find( buffer, "\n" )
	if pos then
		buffer = string.sub( buffer, 0, pos - 1 )

		if buffer == "quit" then
			state = "FINISHING"
		else
			buffer = buffer .. "\n"
		end
		return pos
	else
		-- buffer doesn't contain a newline, wait a little longer
		return 0
	end
end

-- the main, initialize globals here
smerplogger.write( "ERROR", "LUA: main called." )

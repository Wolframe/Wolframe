-- global variables
state = "INIT"
buffer = ""

-- called when a new LUA server is started
function init( )
	log( "TRACE", "LUA: init called." )
	state = "INIT"
	buffer = ""
end

-- called when the LUA server is stopped
function destroy( )
	log( "TRACE", "LUA: destroy called." )
end

-- called when a client connection gets established
function new_connection( remote_host, remote_port, common_name )
	log( "TRACE",  "LUA: new_connection called from " .. remote_host .. " (port: " .. remote_port .. ")" )
	if common_name then
		log( "TRACE", "LUA: encrypted connection, CN is ", common_name )
	end
	state = "NEW"
end

-- handle a request and produce a reply
function next_operation( )
	log( "TRACE", "LUA next operation, in state ", state )
	
	if state == "NEW" then
		state = "HELLO_SENT"
		return "WRITE", "Welcome to SMERP.\n"
 	elseif state == "HELLO_SENT" then
		if string.len( buffer ) == 0 then
			state = "READ_INPUT"
			return "READ", 30
		else
			state = "OUTPUT_MSG"
			return "WRITE", "BUFFER NOT EMPTY!\n"
		end
	elseif state == "READ_INPUT" or state == "OUTPUT_MSG" then
		if string.sub( buffer, 1, 4 ) == "quit" then
			state = "TERMINATE"
			return "WRITE", "Thanks for using SMERP.\n"
		else
			pos = string.find( buffer, "\n" )
			if pos then
				-- newline found, echo the line
				state = "OUTPUT_MSG"
				echo = string.sub( buffer, 1, pos )
				buffer = string.sub( buffer, pos + 1 )
				return "WRITE", echo;
			else
				-- we wait too long, the buffer in luaHandler.cpp can't
				-- overflow, but we could build a buffer of infinite size :-)
				if string.len( buffer ) > 8192 then
					state = "TERMINATE"
					return "WRITE", "Line too long. Bye.\n"
				end

				-- wait for more to come
				state = "READ_INPUT"
				return "READ", 30
			end
		end
	elseif state == "TERMINATE" then
		state = "FINISHED"
		return "CLOSE"
	elseif state == "TIMEOUT" then
		state = "TERMINATE"
		return "WRITE", "Timeout. :P\n"
	elseif state == "SIGNALLED" then
		state = "TERMINATE"
		return "WRITE", "Server is shutting down. :P\n"
	elseif state == "TERMINATE" then
		state = "FINISHED"
		return "CLOSE"
	elseif state == "FINISHED" then
		log( "DEBUG", "Processor in FINISHED state" )
		return "CLOSE"
	else
		state = "ILLEGAL"
		log( "FATAL", "LUA: Illegal state " .. state .. "!!" )
		return "CLOSE"
	end
end

-- a timeout occured
function timeout_occured( )
	log( "TRACE", "LUA: got timeout" )
	state = "TIMEOUT"
end

-- a signal (Ctrl-C) occured
function signal_occured( )
	log( "TRACE", "LUA: got signalled" )
	state = "SIGNALLED"
end

-- an error occured
function error_occured( error )
	log( "TRACE", "LUA: got error ", error )
	state = "TERMINATE"
end

-- called when receiving new data from the network, we have to consume all of it
function network_input( data )
	log( "DATA", "LUA: Got ", string.len( data ), " bytes of data" )

	buffer = buffer .. data
end

-- the main, initialize globals here
log( "TRACE", "LUA: main called." )

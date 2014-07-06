function run( input )
	provider.authorize("CONNECT")
	provider.authorize("PASSWD")
	provider.authorize("DBACCESS", "Customer.WRITE")
end

function checkValidConnect( struct )
	st = struct:value()
	logger.printc( "STRUCT [", st, "] FROM='", st.from, "' TO='", st.to, "' USER='", st.user, "'")
end

function checkCapabilityPasswordChange( struct )
	st = struct:value()
	logger.printc( "STRUCT [", st, "] FROM='", st.from, "' TYPE='", st.type, "'")
end

function checkTableAccess( struct )
	st = struct:value()
	logger.printc( "STRUCT [", st, "] TAB='", st['table'], "' OP='", st['op'], "' USER='", st['user'], "'")
end



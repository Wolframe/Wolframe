function checkValidConnect( struct )
	st = struct:value()
	if st[ 'from' ] ~= "123.123.123.123" then
		return false
	end
	if st[ 'to' ] ~= "fakeSocketIdentifier" then
		return false
	end
	if st[ 'user' ] ~= "wolfilter" then
		return false
	end
	return true
end

function checkCapabilityPasswordChange( struct )
	st = struct:value()
	if st[ 'from' ] ~= "123.123.123.123" then
		return false
	end
	if st[ 'type' ] ~= "TCP" then
		return false
	end
	return true
end

function checkTableAccess( struct )
	st = struct:value()
	if st[ 'table' ] ~= "Customer" then
		return false
	end
	if st[ 'op' ] ~= "WRITE" then
		return false
	end
	if st[ 'user' ] ~= "wolfilter" then
		return false
	end
	return true
end

function checkNoArg( struct )
	st = struct:value()
	return true
end

function checkAuthenticator( struct )
	st = struct:value()
	return (st[ 'auth' ] == "WolfilterAuth")
end



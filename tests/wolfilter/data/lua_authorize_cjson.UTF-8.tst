**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
{
    "-doctype": "Person",
    "name": "Hugi Turbo",
    "address": "Gurkenstrasse 7a 3145 Gumligen"
}**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson --module ../../src/modules/doctype/json/mod_doctype_json -c wolframe.conf run

**file:wolframe.conf
LoadModules
{
	module ./../../src/modules/cmdbind/aamap/mod_command_aamap
	module ./../../src/modules/cmdbind/lua/mod_command_lua
}
Processor
{
	program ../wolfilter/template/program/basic.aamap
	program ../wolfilter/scripts/authorize.lua
	cmdhandler
	{
		lua
		{
			program authorize_test.lua
			filter cjson
		}
	}
}
**file:basic.aamap

# [1] Implicit authorization check called immediately after connect:
#	-> Redirect to function checkValidConnect( from="156.217.32.32", to="PublicInterface", user="Koebi" );

AUTHORIZE CONNECT checkValidConnect( from=remotehost, to=socketid, user=username );

# [2] Implicit authorization check called from main protocol to 
#	switch ON/OFF capability to change own password:
#	-> Redirect to function checkCapabilityPasswordChange( from="156.217.32.32", type="SSL" );

AUTHORIZE PASSWD checkCapabilityPasswordChange( from=remotehost, type=connectiontype );

# [3] Explicit authorization check called in transaction
# example: AUTHORIZE (DBACCESS, Customer.WRITE)
#	-> Redirect to function checkTableAccess( table=Customer, op=WRITE, user=Fredi );

AUTHORIZE DBACCESS checkTableAccess( table=resource[.1], op=resource[.2], user=username );

# [4] 
AUTHORIZE NOARG checkNoArg();

# [5] 
AUTHORIZE AUTHENTICATOR checkAuthenticator( auth=authenticator );
**file:authorize_test.lua
function run( input )
	if not provider.authorize("NOARG") then
		error( "NOARG not authorized")
	end
	if not provider.authorize("CONNECT") then
		error( "CONNECT not authorized")
	end
	if not provider.authorize("PASSWD") then
		error( "PASSWD not authorized")
	end
	if not provider.authorize("DBACCESS", "Customer.WRITE") then
		error( "DBACCESS WRITE Customer on not authorized")
	end
	if not provider.authorize("AUTHENTICATOR") then
		error( "AUTHENTICATOR not authorized")
	end
end
**file:authorize.lua
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
**requires:DISABLED NETBSD

**output
{
	"-doctype":	"Person"
}
**end

**
**requires:LUA
**input
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE person SYSTEM 'Person'>
<person><name>Hugi Turbo</name><address>Gurkenstrasse 7a 3145 Gumligen</address></person>**config
--input-filter textwolf --output-filter textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf --module ../../src/modules/doctype/xml/mod_doctype_xml -c wolframe.conf run
**requires:TEXTWOLF
**file:wolframe.conf
LoadModules
{
	module ./../../src/modules/cmdbind/aamap/mod_command_aamap
	module ./../../src/modules/cmdbind/lua/mod_command_lua
}
Processor
{
	program ../wolfilter/scripts/authorize.lua
	program ../wolfilter/template/program/basic.aamap
	cmdhandler
	{
		lua
		{
			program authorize.lua
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
**file:authorize.lua
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
**output
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE person SYSTEM "Person"><person/>
**end

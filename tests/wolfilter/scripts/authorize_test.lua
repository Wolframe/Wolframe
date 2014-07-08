function run( input )
	if not provider.authorize("CONNECT") then
		error( "CONNECT not authorized")
	end
	if not provider.authorize("PASSWD") then
		error( "PASSWD not authorized")
	end
	if not provider.authorize("DBACCESS", "Customer.WRITE") then
		error( "DBACCESS WRITE Customer on not authorized")
	end
	if not provider.authorize("NOARG") then
		error( "NOARG not authorized")
	end
	if not provider.authorize("AUTHENTICATOR") then
		error( "AUTHENTICATOR not authorized")
	end
end



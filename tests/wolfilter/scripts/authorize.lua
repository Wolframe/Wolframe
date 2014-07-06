function run( input )
	provider.authorize("CONNECT")
	provider.authorize("PASSWD")
	provider.authorize("DBACCESS", "Customer.WRITE")
end


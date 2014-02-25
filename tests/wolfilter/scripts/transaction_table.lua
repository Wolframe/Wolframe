
function run()
	t = input:table()
	r = provider.formfunction("test_transaction")( t)
	output:print( r:table())
end


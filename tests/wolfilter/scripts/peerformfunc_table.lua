
function run()
	t = input:table()
	r = transaction("echo_transaction")( t)
	output:print( r:table())
end


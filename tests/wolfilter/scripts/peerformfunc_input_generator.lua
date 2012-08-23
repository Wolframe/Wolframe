
function run()
	r = transaction("echo_transaction")( input:get())
	output:print( r:get())
end


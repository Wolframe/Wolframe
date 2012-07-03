
function run()
	r = formfunction("transaction")( input:get())
	for v,t in r:get() do
		output:print( v,t)
	end
end


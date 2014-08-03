function run()
	provider.filter().empty = false
	local itr = input:get()
	for v,t in itr do
		if t == "customer" then
			provider.formfunction( "insertCustomer")( iterator.scope( itr))
		end
	end
end


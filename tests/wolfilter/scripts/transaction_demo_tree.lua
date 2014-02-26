function run()
	provider.filter().empty = false
	output:opentag( "result")			-- top level result tag
	local itr = input:get()
	for v,t in itr do
		if t and t ~= "transactions" then	-- top level tag names are the transaction names
			f = provider.formfunction( t )	-- call the transaction
			output:print( f( itr))		-- print the result
		end
	end
	output:closetag()				-- close result
end


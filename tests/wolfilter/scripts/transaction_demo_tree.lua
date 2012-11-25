function run()
	filter().empty = false
	output:opentag( "result")			-- top level result tag
	for v,t in input:get() do
		if t and t ~= "transactions" then	-- top level tag names are the transaction names
			q = input:table()		-- query is subtree
			f = formfunction( t )		-- call the transaction
			output:print( f( q ))		-- print the result
		end
	end
	output:closetag()				-- close result
end


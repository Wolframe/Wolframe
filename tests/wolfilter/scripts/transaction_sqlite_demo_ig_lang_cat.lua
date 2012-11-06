idcnt = 0

function insert_class( parentid, itr)
	local id = idcnt + 1
	for v,t in itr do
		if (t == "name") then
			local name = v
			if idcnt == 0 then
				local f = formfunction( "treeAddRoot")
				f( { node = { name=name } } )
			else
				local f = formfunction( "treeAddNode")
				f( { node = { name=name, parentid=parentid} } )
			end
			idcnt = idcnt + 1
		end
		if (t == "class") then
			insert_class( id, scope( itr))
		end
	end
end

function run()
	filter().empty = false
	output:opentag( "class")
	local itr = input:get()
	for v,t in itr do
		if t == "class" then
			insert_class( idcnt, scope( itr))
		end
	end
	output:closetag()
end


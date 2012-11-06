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

function get_tree( parentid)
	local f = formfunction( "treeSelectNodeAndChildren" )
	local r = f( { node = { id=parentid } } )
	local t = r:table()
	local a = {}
	for i,v in ipairs( t[ "node" ]) do
		table.insert( a, tonumber( v.ID), { name=v.name, parent=tonumber(v.parent), children = {} } )
	end
	for i,v in ipairs( a) do
		if v.parent ~= 0 then
			table.insert( a[ v.parent ].children, i )
		end
	end
	return a
end

function print_tree( tree, nodeid, indent)
	if (indent ~= "") then
		output:print( "\n" .. indent)
	end
	output:opentag( "class")
	output:print( tree[ nodeid].name, "name")
	local n = 0
	for i,v in ipairs( tree[ nodeid].children) do
		print_tree( tree, v, indent .. "\t")
		n = n + 1
	end
	if n > 0 then
		output:print( "\n" .. indent)
	end
	output:closetag()
end

function run()
	filter().empty = false
	local itr = input:get()
	for v,t in itr do
		if t == "class" then
			insert_class( idcnt, scope( itr))
		end
	end
	print_tree( get_tree( 1), 1, "")
end



local function insert_tree_itr( parentid, itr)
	local id = 1
	local name = ""
	for v,t in itr do
		if (t == "name") then
			id = formfunction( "treeAddNode")( {name=v, parentid=parentid} ):table().ID
		elseif (t == "class") then
			insert_tree_itr( id, scope( itr))
		end
	end
end

local function insert_topnode( name, parentid)
	if not parentid then
		formfunction( "treeAddRoot")( {name=name} )
	else
		formfunction( "treeAddNode")( {name=name, parentid=parentid} )
	end
end

local function insert_tree_topnode( itr)
	local parentid = nil
	local id = 1
	local name = nil
	for v,t in itr do
		if (t == "parent") then
			parentid = tonumber( v)
		elseif (t == "name") then
			name = v
		elseif (t == "class") then
			if name then
				insert_topnode( name, parentid)
				name = nil
			end
			insert_tree_itr( id, scope( itr))
		end
	end
	if name then
		insert_topnode( name, parentid)
	end
end

local function get_tree( parentid)
	local t = formfunction( "treeSelectNodeAndChildren")( { node = { id=parentid } } ):table()["node"] or {}
	local a = {}
	for i,v in pairs( t) do
		table.insert( a, tonumber( v.ID), { name=v.name, parent=tonumber(v.parent), children = {} } )
	end
	for i,v in pairs( a) do
		if v.parent ~= 0 then
			table.insert( a[ v.parent ].children, i )
		end
	end
	return a
end

local function print_tree_xml( tree, nodeid, indent)
	if (indent ~= "") then
		output:print( "\n" .. indent)
	end
	output:opentag( "class")
	output:print( tree[ nodeid].name, "name")
	local n = 0
	for i,v in pairs( tree[ nodeid].children) do
		print_tree_xml( tree, v, indent .. "\t")
		n = n + 1
	end
	if n > 0 then
		output:print( "\n" .. indent)
	end
	output:closetag()
end

function treeSelectHierarchy()
	filter().empty = false
	output:opentag( "result")
	for v,t in itr do
		if t == "id" then
			local id = tonumber( v)
			print_tree_xml( get_tree( id), id, "")
		end
	end
	output:closetag()
end

function treeAddHierarchy()
	filter().empty = false
	local itr = input:get()
	for v,t in itr do
		if t == "class" then
			insert_tree_topnode( scope( itr))
		end
	end
end


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

function delete_subtree( name)
	local id = formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table()["node"].ID
	logger.print( "ERROR", "PARAM treeSelectNodeByName ", name, " ", id)
	formfunction( "treeDeleteSubtree")( { node={ id=id } } )
end

function select_subtree( name)
	local id = formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table()["node"].ID
	local nodear = formfunction( "treeSelectNodeAndChildren")( { node={ id=id } } ):table()["node"] or {}
	output:opentag( "subtree")
	output:print( name, "name")
	output:print( "\n")
	for i,v in ipairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
		output:print( "\n")
	end
	output:closetag()
	output:print( "\n")
end

function select_subtree2( name)
	local nodear = formfunction( "treeSelectNodeAndChildrenByName")( { node={ name=name } } ):table()["node"] or {}
	output:opentag( "subtree")
	output:print( name, "name")
	output:print( "\n")
	for i,v in ipairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
		output:print( "\n")
	end
	output:closetag()
	output:print( "\n")
end

function select_children( name)
	local id = formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table()["node"].ID
	local nodear = formfunction( "treeSelectChildren")( { node={ id=id } } ):table()["node"] or {}
	output:opentag( "children")
	output:print( name, "name")
	output:print( "\n")
	for i,v in ipairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
		output:print( "\n")
	end
	output:closetag()
	output:print( "\n")
end

function select_children2( name)
	local nodear = formfunction( "treeSelectChildrenByName")( { node={ name=name } } ):table()["node"] or {}
	output:opentag( "children")
	output:print( name, "name")
	output:print( "\n")
	for i,v in ipairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
		output:print( "\n")
	end
	output:closetag()
	output:print( "\n")
end

function select_cover( name)
	local id = formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table()["node"].ID
	local nodear = formfunction( "treeSelectNodeAndParents")( { node={ id=id } } ):table()["node"] or {}
	output:opentag( "cover")
	output:print( name, "name")
	output:print( "\n")
	for i,v in ipairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
		output:print( "\n")
	end
	output:closetag()
	output:print( "\n")
end

function select_cover2( name)
	local nodear = formfunction( "treeSelectNodeAndParentsByName")( { node={ name=name } } ):table()["node"] or {}
	output:opentag( "cover")
	output:print( name, "name")
	output:print( "\n")
	for i,v in ipairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
		output:print( "\n")
	end
	output:closetag()
	output:print( "\n")
end

function select_parents( name)
	local id = formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table()["node"].ID
	local nodear = formfunction( "treeSelectParents")( { node={ id=id } } ):table()["node"] or {}
	output:opentag( "parents")
	output:print( name, "name")
	output:print( "\n")
	for i,v in ipairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
		output:print( "\n")
	end
	output:closetag()
	output:print( "\n")
end

function select_parents2( name)
	local nodear = formfunction( "treeSelectParentsByName")( { node={ name=name } } ):table()["node"] or {}
	output:opentag( "parents")
	output:print( name, "name")
	output:print( "\n")
	for i,v in ipairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
		output:print( "\n")
	end
	output:closetag()
	output:print( "\n")
end

function get_tree( parentid)
	local t = formfunction( "treeSelectNodeAndChildren")( { node = { id=parentid } } ):table()["node"] or {}
	local a = {}
	for i,v in ipairs( t) do
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
	output:opentag( "result")
	local itr = input:get()
	for v,t in itr do
		if t == "class" then
			insert_class( idcnt, scope( itr))
		end
	end
	print_tree( get_tree( 1), 1, "")
	select_subtree( "italic")
	select_subtree( "brythonic")
	select_subtree2( "germanic")
	select_subtree2( "anglo-frisian")
	select_children( "gaulisch")
	select_children( "slavonic")
	select_children2( "east germanic")
	select_children2( "indic")
	select_cover( "italic")
	select_cover( "brythonic")
	select_cover2( "germanic")
	select_cover2( "anglo-frisian")
	select_parents( "gaulisch")
	select_parents( "slavonic")
	select_parents2( "east germanic")
	select_parents2( "indic")
	-- delete_subtree( "hellenic")
	-- delete_subtree( "hittie")
	-- delete_subtree( "celtic")
	-- delete_subtree( "indo iranian")
	output:opentag( "sparsetree")
	print_tree( get_tree( 1), 1, "")
	output:closetag()
	output:closetag()
end


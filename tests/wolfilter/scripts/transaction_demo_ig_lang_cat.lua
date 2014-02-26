idcnt = 0

function insert_tree( parentid, itr)
	local id = idcnt + 1
	for v,t in itr do
		if (t == "name") then
			local name = v
			if idcnt == 0 then
				provider.formfunction( "treeAddRoot")( {name=name} )
			else
				provider.formfunction( "treeAddNode")( {name=name, parentid=parentid} )
			end
			idcnt = idcnt + 1
		end
		if (t == "class") then
			insert_tree( id, scope( itr))
		end
	end
end

function insert_node( parentname, name)
	local parentid = provider.formfunction( "treeSelectNodeByName")( { node={ name=parentname } } ):table().ID
	provider.formfunction( "treeAddNode")( {name=name, parentid=parentid} )
end

function copy_node( name, parentname)
	local parentid = provider.formfunction( "treeSelectNodeByName")( { node={ name=parentname } } ):table().ID
	local nodeid = provider.formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table().ID
	provider.formfunction( "treeCopyNode")( {nodeid=nodeid, newparentid=parentid} )
end


function delete_subtree( name)
	local id = provider.formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table().ID
	provider.formfunction( "treeDeleteSubtree")( {id=id} )
end

function select_subtree( name)
	local id = provider.formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table().ID
	local nodear = provider.formfunction( "treeSelectNodeAndChildren")( { node={ id=id } } ):table()["node"] or {}
	output:opentag( "subtree")
	output:print( name, "name")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
	end
	output:closetag()
end

function select_subtree2( name)
	local nodear = provider.formfunction( "treeSelectNodeAndChildrenByName")( { node={ name=name } } ):table()["node"] or {}
	output:opentag( "subtree")
	output:print( name, "name")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
	end
	output:closetag()
end

function select_children( name)
	local id = provider.formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table().ID
	local nodear = provider.formfunction( "treeSelectChildren")( { node={ id=id } } ):table()["node"] or {}
	output:opentag( "children")
	output:print( name, "name")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
	end
	output:closetag()
end

function select_children2( name)
	local nodear = provider.formfunction( "treeSelectChildrenByName")( { node={ name=name } } ):table()["node"] or {}
	output:opentag( "children")
	output:print( name, "name")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
	end
	output:closetag()
end

function select_cover( name)
	local id = provider.formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table().ID
	local nodear = provider.formfunction( "treeSelectNodeAndParents")( { node={ id=id } } ):table()["node"] or {}
	output:opentag( "cover")
	output:print( name, "name")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
	end
	output:closetag()
end

function select_cover2( name)
	local nodear = provider.formfunction( "treeSelectNodeAndParentsByName")( { node={ name=name } } ):table()["node"] or {}
	output:opentag( "cover")
	output:print( name, "name")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
	end
	output:closetag()
end

function select_parents( name)
	local id = provider.formfunction( "treeSelectNodeByName")( { node={ name=name } } ):table().ID
	local nodear = provider.formfunction( "treeSelectParents")( { node={ id=id } } ):table()["node"] or {}
	output:opentag( "parents")
	output:print( name, "name")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
	end
	output:closetag()
end

function select_parents2( name)
	local nodear = provider.formfunction( "treeSelectParentsByName")( { node={ name=name } } ):table()["node"] or {}
	output:opentag( "parents")
	output:print( name, "name")
	for i,v in pairs( nodear) do
		output:opentag( "node")
		output:print( v.ID, "id")
		output:print( v.name, "name")
		output:closetag()
	end
	output:closetag()
end

function get_tree( parentid)
	local t = provider.formfunction( "treeSelectNodeAndChildren")( { node = { id=parentid } } ):table()["node"] or {}
	local a = {}
	for i,v in pairs( t) do
		table.insert( a, tonumber( v.ID), { name=v.name, parent=tonumber(v.parent), children = {} } )
	end
	for i,v in pairs( a) do
		if v.parent and v.parent ~= 0 then
			table.insert( a[ v.parent ].children, i )
		end
	end
	return a
end

function print_tree( tree, nodeid)
	output:opentag( "class")
	output:print( tree[ nodeid].name, "name")
	local n = 0
	for i,v in pairs( tree[ nodeid].children) do
		print_tree( tree, v)
		n = n + 1
	end
	output:closetag()
end

function run()
	provider.filter().empty = false
	local itr = input:get()
	for v,t in itr do
		if t == "class" then
			insert_tree( idcnt, scope( itr))
		end
	end
	output:opentag( "result")
	print_tree( get_tree( 1), 1)
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
	delete_subtree( "hellenic")
	delete_subtree( "hittie")
	insert_node( "swiss", "bern german")
	insert_node( "swiss", "eastern swiss german")
	insert_node( "swiss", "grison german")
	insert_node( "indogermanic", "hittie")
	delete_subtree( "celtic")
	delete_subtree( "indo iranian")
	output:opentag( "sparsetree")
	print_tree( get_tree( 1), 1)
	output:closetag()
	output:closetag()
end


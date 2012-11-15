local function normalizeName( name)
	return name:gsub("[^%s]+", string.lower):gsub("[%-()]+", " "):gsub("^%s+", ""):gsub("%s+$", ""):gsub("%s+", " ")
end

local function insert_itr( tablename, parentid, itr)
	local id = 1
	local name = ""
	for v,t in itr do
		if (t == "name") then
			local nname = normalizeName( v)
			id = formfunction( "add" .. tablename)( {name=v, normalizedName=nname, parentid=parentid} ):table().ID
		elseif (t == "node") then
			insert_itr( tablename, id, scope( itr))
		end
	end
	return id
end

local function insert_topnode( tablename, name, parentid)
	local nname = normalizeName( name)
	if not parentid then
		formfunction( "add" .. tablename .. "Root")( {normalizedName=nname, name=name} )
		return 1
	else
		local id = formfunction( "add" .. tablename)( {normalizedName=nname, name=name, parentid=parentid} ):table().ID
		return id
	end
end

local function insert_tree_topnode( tablename, itr)
	local parentid = nil
	local id = 1
	local name = nil
	for v,t in itr do
		if (t == "parent") then
			parentid = tonumber( v)
		elseif (t == "name") then
			name = v
		elseif (t == "node") then
			if name then
				id = insert_topnode( tablename, name, parentid)
				name = nil
			end
			insert_itr( tablename, id, scope( itr))
		end
	end
	if name then
		insert_topnode( tablename, name, parentid)
	end
end

local function get_tree( tablename, parentid)
	local t = formfunction( "selectSub" .. tablename)( {id=parentid} ):table()["node"] or {}
	local a = {}
	for i,v in pairs( t) do
		table.insert( a, tonumber( v.ID), { name=v.name, parent=tonumber(v.parent), children = {} } )
	end
	for i,v in pairs( a) do
		if i ~= parentid and v.parent then
			table.insert( a[ v.parent ].children, i )
		end
	end
	return a
end

local function print_tree( tree, nodeid, indent)
	if (indent ~= "") then
		output:print( "\n" .. indent)
	end
	output:opentag( "node")
	output:print( tree[ nodeid].name, "name")
	output:print( nodeid, "id")
	local n = 0
	for i,v in pairs( tree[ nodeid].children) do
		print_tree( tree, v, indent .. "\t")
		n = n + 1
	end
	if n > 0 then
		output:print( "\n" .. indent)
	end
	output:closetag()
end

local function select_tree( tablename, itr)
	filter().empty = false
	for v,t in itr do
		if t == "id" then
			local id = tonumber( v)
			print_tree( get_tree( tablename, id), id, "")
		end
	end
end

local function edit_node( tablename, itr)
	local name = nil;
	local nname = nil;
	local id = nil;
	for v,t in itr do
		if t == "id" then
			id = v
		elseif t ==  "name" then
			name = v
			nname = normalizeName( name)
		end
	end
	formfunction( "update" .. tablename)( {normalizedName=nname, name=name, id=id} )
end

local function delete_node( tablename, itr)
	local id = nil;
	for v,t in itr do
		if t == "id" then
			id = v
		end
	end
	formfunction( "delete" .. tablename)( {id=id} )
end

local function create_node( tablename, itr)
	local name = nil;
	local parentid = nil;
	for v,t in itr do
		if t == "id" then
			parentid = v
		elseif t == "parent" then
			parentid = v
		elseif t ==  "name" then
			name = v
		end
	end
	insert_topnode( tablename, name, parentid)
end

local function add_tree( tablename, itr)
	filter().empty = false
	for v,t in itr do
		if t == "node" then
			insert_tree_topnode( tablename, scope( itr))
		end
	end
end

function CategoryHierarchyRequest()
	output:as( "node SYSTEM 'CategoryHierarchy.simpleform'")
	select_tree( "Category")
end

function FeatureHierarchyRequest()
	output:as( "node SYSTEM 'FeatureHierarchy.simpleform'")
	select_tree( "Feature")
end

function pushCategoryHierarchy()
	add_tree( "Category", input:get())
end

function pushFeatureHierarchy()
	add_tree( "Feature", input:get())
end

function CategoryHierarchyRequest()
	select_tree( "Category", input:get())
end

function FeatureHierarchyRequest()
	select_tree( "Feature", input:get())
end

function editCategory()
	edit_node( "Category", input:get())
	output:as( "node SYSTEM 'CategoryHierarchy.simpleform'")
	print_tree( get_tree( "Category", 1), 1, "")
end

function editFeature()
	edit_node( "Feature", input:get())
	output:as( "node SYSTEM 'FeatureHierarchy.simpleform'")
	print_tree( get_tree( "Feature", 1), 1, "")
end

function deleteCategory()
	delete_node( "Category", input:get())
	output:as( "node SYSTEM 'CategoryHierarchy.simpleform'")
	print_tree( get_tree( "Category", 1), 1, "")
end

function deleteFeature()
	delete_node( "Feature", input:get())
	output:as( "node SYSTEM 'FeatureHierarchy.simpleform'")
	print_tree( get_tree( "Feature", 1), 1, "")
end

function createCategory()
	create_node( "Category", input:get())
	output:as( "node SYSTEM 'CategoryHierarchy.simpleform'")
	print_tree( get_tree( "Category", 1), 1, "")
end

function createFeature()
	create_node( "Feature", input:get())
	output:as( "node SYSTEM 'FeatureHierarchy.simpleform'")
	print_tree( get_tree( "Feature", 1), 1, "")
end


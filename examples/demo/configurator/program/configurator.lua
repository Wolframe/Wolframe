local function normalizeName( name)
	return name:gsub("[^%s]+", string.lower):gsub("^%s+", ""):gsub("%s+$", ""):gsub("%s+", " ")
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
end

local function insert_topnode( tablename, name, parentid)
	local nname = normalizeName( name)
	if not parentid then
		formfunction( "add" .. tablename .. "Root")( {normalizedName=nname, name=name} )
		return 1
	else
		return formfunction( "add" .. tablename)( {normalizedName=nname, name=name, parentid=parentid} ):table().ID
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
	local t = formfunction( "selectSub" .. tablename)( { node = { id=parentid } } ):table()["node"] or {}
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
	output:opentag( "tree")
	for v,t in itr do
		if t == "id" then
			local id = tonumber( v)
			print_tree( get_tree( tablename, id), id, "")
		end
	end
	output:closetag()
end

function selectCategoryHierarchy()
	output:as( "tree 'hierarchyCategory")
	select_tree( "Category")
end

function selectFeatureHierarchy()
	output:as( "tree 'hierarchyFeature")
	select_tree( "Feature")
end

local function add_tree( tablename, itr)
	filter().empty = false
	for v,t in itr do
		if t == "node" then
			insert_tree_topnode( tablename, scope( itr))
		end
	end
end

function addCategoryHierarchy()
	add_tree( "Category", input:get())
end

function addFeatureHierarchy()
	add_tree( "Feature", input:get())
end

function selectCategoryHierarchy()
	select_tree( "Category", input:get())
end

function selectFeatureHierarchy()
	select_tree( "Feature", input:get())
end

local function content_value( v, itr)
	if v then
		return v
	end
	for v,t in itr do
		if t and v then
		else
			if t then
				return nil
			end
			if v then
				return v
			end
		end
	end
end

local function pictures_value( pictures, itr )
	if pictures == nil then
		pictures = { ["picture"] = { } }
	end
	for v,t in itr do
		--logger:print( "ERROR", "INPICTURE", ":", v, ":", t, ":", id )
		if( t == "id") then
			table.insert( pictures[ "picture" ], { ["id"] = v } )
		end
	end
	return pictures
end

local function insert_itr( tablename, parentID, itr)
	local id = 1
	local name = nil
	local nname = nil
	local description = nil
	local pictures = nil
	for v,t in itr do
		if (t == "name") then
			name = content_value( v, itr)
			nname = normalizer("name")( name)
		elseif (t == "description") then
			description = content_value( v, itr)
		elseif (t == "picture") then
			pictures = pictures_value( pictures, scope( itr))
		elseif (t == "node") then
			if name then
				id = formfunction( "add" .. tablename)( {name=name, normalizedName=nname, description=description, parentID=parentID, pictures=pictures} ):table().ID
				name = nil
				description = nil
			end
			insert_itr( tablename, id, scope( itr))
		end
	end
	if name then
		id = formfunction( "add" .. tablename)( {name=name, normalizedName=nname, description=description, parentID=parentID, pictures=pictures} ):table().ID
	end
	return id
end

local function insert_topnode( tablename, name, description, pictures, parentID)
	local nname = normalizer("name")( name)
	if not parentID then
		parentID = 1
	end
	local id = formfunction( "add" .. tablename)( {normalizedName=nname, name=name, description=description, parentID=parentID, pictures=pictures} ):table().ID
	return id
end

local function insert_tree_topnode( tablename, itr)
	local parentID = nil
	local id = 1
	local name = nil
	local description = nil
	local pictures = nil
	for v,t in itr do
		if (t == "parentID") then
			parentID = tonumber( v)
		elseif (t == "name") then
			name = content_value( v, itr)
		elseif (t == "description") then
			description = content_value( v, itr)
		elseif (t == "picture") then
			pictures = pictures_value( pictures, scope( itr))
		elseif (t == "node") then
			if name then
				id = insert_topnode( tablename, name, description, pictures, parentID)
				name = nil
				description = nil
			end
			insert_itr( tablename, id, scope( itr))
		end
	end
	if name then
		insert_topnode( tablename, name, description, pictures, parentID)
	end
end

local function get_tree( tablename, parentID)
	local t = formfunction( "selectSub" .. tablename)( {id=parentID} ):table()["node"] or {}
	local a = {}
	for i,v in pairs( t) do
		table.insert( a, tonumber( v.ID), { name=v.name, description=v.description, pictures=v.pictures, parentID=tonumber(v.parentID), children = {} } )
	end
	for i,v in pairs( a) do
		if i ~= parentID and v.parentID then
			table.insert( a[ v.parentID ].children, i )
		end
	end
	return a
end

local function print_tree( tree, tagname, nodeid, indent)
	if (indent ~= "") then
		output:print( "\n" .. indent)
	end
	output:opentag( "tree" )
	if tree[ nodeid ] then
		output:opentag( "item" )
		output:print( nodeid, "id")
		output:print( "\n" .. indent .. "\t")
		output:opentag( tagname)
		output:print( tree[ nodeid ].name )
		output:closetag( )
		if tree[ nodeid ].description then
			output:print( "\n" .. indent .. "\t")
			output:opentag( "description" )
			output:print( tree[ nodeid ].description )
			output:closetag( )
		end
		local n = 0
		for i,v in pairs( tree[ nodeid].children) do
			print_tree( tree, tagname, v, indent .. "\t")
			n = n + 1
		end
		if n > 0 then
			output:print( "\n" .. indent)
		end
		output:closetag( )
	end
	output:closetag()
end

local function select_tree( tablename, tagname, itr)
	filter().empty = false
	for v,t in itr do
		if t == "id" then
			local id = tonumber( v)
			print_tree( get_tree( tablename, id), tagname, id, "")
		end
	end
end

local function select_node( tablename, elementname, itr)
	filter().empty = false
	for v,t in itr do
		if t == "id" then
			output:opentag( elementname)
			local r = formfunction( "select" .. tablename)( {id=v} )
			local f = form( tablename);
			f:fill( r:get())
			output:print( f:get())
			output:closetag( )
		end
	end
end

local function edit_node( tablename, itr)
	local name = nil
	local nname = nil
	local description = nil
	local pictures = nil
	local id = nil
	for v,t in itr do
		--logger:print( "ERROR", v, ":", t, ":", id )
		if( t == "id" ) then
			id = v
		elseif t ==  "name" then
			name = content_value( v, itr)
			nname = normalizer("name")( name)
		elseif t == "description" then
			description = content_value( v, itr)
		elseif( t == "picture" ) then
			pictures = pictures_value( pictures, scope( itr))
		end
	end
	--logger:print( "ERROR", id, ":", name, ":", nname, ":", pictures )
	formfunction( "update" .. tablename)( {normalizedName=nname, name=name, description=description, id=id, pictures=pictures} )
end

local function delete_node( tablename, itr)
	local id = nil;
	for v,t in itr do
		if t == "id" then
			id = v
		end
	end
	-- don't allow deletion of the root element (fast hack)
	if id == "1" then
		return
	end
	formfunction( "delete" .. tablename)( {id=id} )
end

local function create_node( tablename, itr)
	local name = nil
	local parentID = nil
	local description = nil
	local pictures = nil
	for v,t in itr do
		if t == "parentID" then
			parentID = v
		elseif t ==  "name" then
			name = content_value( v, itr)
			nname = normalizer("name")( name)
		elseif t ==  "description" then
			description = content_value( v, itr)
		elseif t ==  "picture" then
			pictures = pictures_value( pictures, scope( itr))
		end
	end
	insert_topnode( tablename, name, description, pictures, parentID)
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
	output:as( "tree SYSTEM 'CategoryHierarchy.simpleform'")
	select_tree( "Category", "category", input:get())
end

function FeatureHierarchyRequest()
	output:as( "tree SYSTEM 'FeatureHierarchy.simpleform'")
	select_tree( "Feature", "feature", input:get())
end

function TagHierarchyRequest()
	output:as( "tree SYSTEM 'TagHierarchy.simpleform'")
	select_tree( "Tag", "tag", input:get())
end

function pushCategoryHierarchy()
	add_tree( "Category", input:get())
end

function pushFeatureHierarchy()
	add_tree( "Feature", input:get())
end

function pushTagHierarchy()
	add_tree( "Tag", input:get())
end

function CategoryRequest()
	output:as( "category SYSTEM 'Category.simpleform'")
	select_node( "Category", "category", input:get())
end

function FeatureRequest()
	output:as( "feature SYSTEM 'Feature.simpleform'")
	select_node( "Feature", "feature", input:get())
end

function TagRequest()
	output:as( "tag SYSTEM 'Tag.simpleform'")
	select_node( "Tag", "tag", input:get())
end

function editCategory()
	edit_node( "Category", input:get())
end

function editFeature()
	edit_node( "Feature", input:get())
end

function editTag()
	edit_node( "Tag", input:get())
end

function deleteCategory()
	delete_node( "Category", input:get())
end

function deleteFeature()
	delete_node( "Feature", input:get())
end

function deleteTag()
	delete_node( "Tag", input:get())
end

function createCategory()
	create_node( "Category", input:get())
end

function createFeature()
	create_node( "Feature", input:get())
end

function createTag()
	create_node( "Tag", input:get())
end

function PictureListRequest( )
	output:as( "list SYSTEM 'PictureList.simpleform'" )
	filter().empty = false
	local t = formfunction( "selectPictureList" )( {} )
	local f = form( "Picture" );
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

function PictureRequest( )
	output:as( "picture SYSTEM 'Picture.simpleform'")
	filter().empty = false
	local id = nil;
	for v,t in input:get( ) do
		if t == "id" then
			id = v
		end
	end
	local t = formfunction( "selectPicture" )( { id = id } )
	local f = form( "Picture" )
	f:fill( t:get( ) )
	-- hack again, can't get the right subtags in TDL..
	output:opentag( "dummy" )
	output:print( f:get( ) )
	output:closetag( )
end

local function transform_picture( itr )
	-- should be a form transformation, not funny lua code :-)
	local picture = {}
	picture["tags"] = {}
	local intag = false
	local intagwrap = false
	local inid = false;
	for v,t in itr do
		if( not v and t ) then
			-- begin tag
			if( t == "tagwrap" ) then
				intagwrap = true
			elseif( t == "tag" ) then
				intag = true
			elseif( t == "caption" or t == "info" or t == "image" ) then
				picture[ t] = content_value( v, itr)
			end
		elseif( v and t ) then
			-- attribute
			inid = true
			if ( ( t == "id" ) and not intagwrap and not intag ) then
				picture[ t] = content_value( v, itr )
			elseif( t == "id" and intag and intagwrap ) then
				table.insert( picture["tags"], { ["id"] = v } )
			end
		else
			-- end tag
			if( inid ) then
				inid = false
			elseif( intag and intagwrap ) then
				intag = false
			elseif( intagwrap ) then
				intagwrap = false
			end
		end
	end
	info = formfunction( "imageInfo" )( { [ "data"] = picture["image"] } ):table( )
	picture["width"] = info.width
	picture["height"] = info.height
	thumb = formfunction( "imageThumb" )( { [ "image" ] = { [ "data" ] = picture["image"] }, [ "size" ] = 50 } ):table( )
	picture["thumbnail"] = thumb.data
	return picture
end

function editPicture( )
	local picture = transform_picture( input:get( ) )
	--logger:print( "ERROR", picture )
	formfunction( "updatePicture" )( { picture = picture } )
end

function createPicture( )
	local picture = transform_picture( input:get( ) )
	formfunction( "addPicture" )( { picture = picture } )
end

function deletePicture( )
	local id = nil;
	for v,t in input:get( ) do
		if t == "id" then
			id = v
		end
	end
	formfunction( "deletePicture" )( { id = id } )
end

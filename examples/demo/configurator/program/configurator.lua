local function content_value( itr)
	local rt = nil
	for v,t in itr do
		if v and not t then
			rt = v
		end
	end
	return rt
end

local function pictures_value( pictures, itr )
	if pictures == nil then
		pictures = { ["picture"] = { } }
	end
	for v,t in itr do
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
			if v then
				name = v
			else
				name = content_value( scope(itr))
			end
			nname = normalizer("name")( name)
		elseif (t == "description") then
			if v then
				description = v
			else
				description = content_value( scope(itr))
			end
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
			if v then
				name = v
			else
				name = content_value( scope(itr))
			end
		elseif (t == "description") then
			if v then
				description = v
			else
				description = content_value( scope(itr))
			end
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
			local f = form( tablename)
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
		if( t == "id" ) then
			id = v
		elseif t ==  "name" then
			if v then
				name = v
			else
				name = content_value( scope(itr))
			end
			nname = normalizer("name")( name)
		elseif t == "description" then
			if v then
				description = v
			else
				description = content_value( scope(itr))
			end
		elseif( t == "picture" ) then
			pictures = pictures_value( pictures, scope( itr))
		end
	end
	formfunction( "update" .. tablename)( {normalizedName=nname, name=name, description=description, id=id, pictures=pictures} )
end

local function delete_node( tablename, itr)
	local id = nil
	for v,t in itr do
		if t == "id" then
			id = v
		end
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
			if v then
				name = v
			else
				name = content_value( scope(itr))
			end
			nname = normalizer("name")( name)
		elseif t ==  "description" then
			if v then
				description = v
			else
				description = content_value( scope(itr))
			end
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
	output:as( {root='tree', system='CategoryHierarchy.simpleform'})
	select_tree( "Category", "category", input:get())
end

function FeatureHierarchyRequest()
	output:as( {root='tree', system='FeatureHierarchy.simpleform'})
	select_tree( "Feature", "feature", input:get())
end

function TagHierarchyRequest()
	output:as( {root='tree', system='TagHierarchy.simpleform'})
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
	output:as( {root='category', system='Category.simpleform'})
	select_node( "Category", "category", input:get())
end

function FeatureRequest()
	output:as( {root='feature', system='Feature.simpleform'})
	select_node( "Feature", "feature", input:get())
end

function TagRequest()
	output:as( {root='tag', system='Tag.simpleform'})
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

-- manufacturers

function ManufacturerListRequest( )
	output:as( {root='list', system='manufacturerList.simpleform'})
	local t = formfunction( "selectManufacturerList" )( {} )
	local f = form( "ManufacturerList" )
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

function createManufacturer( )
	local manufacturer = input:table( )["manufacturer"]
	if manufacturer["picture"] then
		manufacturer["logo"] = manufacturer["picture"]["id"]
	end
	if manufacturer["name"] then
		manufacturer["normalizedName"] = normalizer( "name" )( manufacturer["name"] )
	end
	formfunction( "addManufacturer" )( manufacturer )
end

function editManufacturer( )
	local manufacturer = input:table( )["manufacturer"]
	if manufacturer["picture"] then
		manufacturer["logo"] = manufacturer["picture"]["id"]
	end
	if manufacturer["name"] then
		manufacturer["normalizedName"] = normalizer( "name" )( manufacturer["name"] )
	end
	formfunction( "updateManufacturer" )( manufacturer )
end

function deleteManufacturer( )
	formfunction( "deleteManufacturer" )( { id = input:table( )["manufacturer"]["id"] } )
end

function ManufacturerRequest( )
	local t = formfunction( "selectManufacturer" )( { id = input:table( )["manufacturer"]["id"] } )
	local f = form( "Manufacturer" )
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

-- components

function ComponentListRequest( )
	output:as( {root='list', system='componentList.simpleform'})
	local t = formfunction( "selectComponentList" )( {} )
	local f = form( "ComponentList" )
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

function createComponent( )
	local component = input:table( )["component"]
	if component["category"] then
		component["categoryID"] = component["category"]["id"]
	end
	if component["manufacturer"] then
		component["manufacturerID"] = component["manufacturer"]["id"]
	end
	if component["name"] then
		component["normalizedName"] = normalizer( "name" )( component["name"] )
	end
	formfunction( "addComponent" )( component )
end

function editComponent( )
	local component = input:table( )["component"]
	if component["category"] then
		component["categoryID"] = component["category"]["id"]
	end
	if component["manufacturer"] then
		component["manufacturerID"] = component["manufacturer"]["id"]
	end
	if component["name"] then
		component["normalizedName"] = normalizer( "name" )( component["name"] )
	end
	formfunction( "updateComponent" )( component )
end

function deleteComponent( )
	formfunction( "deleteComponent" )( { id = input:table( )["component"]["id"] } )
end

function ComponentRequest( )
	local t = formfunction( "selectComponent" )( { id = input:table( )["component"]["id"] } )
	local f = form( "Component" )
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

-- recipes

function RecipeListRequest( )
	output:as( {root='list', system='recipeList.simpleform'})
	local t = formfunction( "selectRecipeList" )( {} )
	local f = form( "RecipeList" )
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

function createRecipe( )
	local recipe = input:table( )["recipe"]
	if recipe["category"] then
		recipe["categoryID"] = recipe["category"]["id"]
	end
	if recipe["name"] then
		recipe["normalizedName"] = normalizer( "name" )( recipe["name"] )
	end
	formfunction( "addRecipe" )( recipe )
end

function editRecipe( )
	local recipe = input:table( )["recipe"]
	if recipe["category"] then
		recipe["categoryID"] = recipe["category"]["id"]
	end
	if recipe["name"] then
		recipe["normalizedName"] = normalizer( "name" )( recipe["name"] )
	end
	formfunction( "updateRecipe" )( recipe )
end

function deleteRecipe( )
	formfunction( "deleteRecipe" )( { id = input:table( )["recipe"]["id"] } )
end

function RecipeRequest( )
	local t = formfunction( "selectRecipe" )( { id = input:table( )["recipe"]["id"] } )
	local f = form( "Recipe" )
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

-- configurations

function ConfigurationListRequest( )
	output:as( {root='list', system='configurationList.simpleform'})
	local t = formfunction( "selectConfigurationList" )( {} )
	local f = form( "ConfigurationList" )
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

function createConfiguration( )
	local configuration = input:table( )["configuration"]
	if configuration["category"] then
		configuration["categoryID"] = configuration["category"]["id"]
	end
	formfunction( "addConfiguration" )( configuration )
end

function editConfiguration( )
	local configuration = input:table( )["configuration"]
	if configuration["category"] then
		configuration["categoryID"] = configuration["category"]["id"]
	end
	formfunction( "updateConfiguration" )( configuration )
end

function deleteConfiguration( )
	formfunction( "deleteConfiguration" )( { id = input:table( )["configuration"]["id"] } )
end

function ConfigurationRequest( )
	local t = formfunction( "selectConfiguration" )( { id = input:table( )["configuration"]["id"] } )
	local f = form( "Configuration" )
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

-- category/features associations

function createCategoryFeature( )
	local categoryFeature = input:table( )["CategoryFeature"]
	formfunction( "addCategoryFeature" )( categoryFeature )
end

function deleteCategoryFeature( )
	local categoryFeature = input:table( )["CategoryFeature"]
	formfunction( "deleteCategoryFeature" )( categoryFeature )
end

function CategoryFeatureRequest( )
	local categoryFeature = input:table( )["CategoryFeature"]
	local t = formfunction( "selectCategoryFeature" )( {
		category_id = categoryFeature["category_id"],
		feature_id = categoryFeature["feature_id"]
	} )
	local f = form( "CategoryFeature" )
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

function CategoryFeatureListRequest( )
	local categoryFeature = input:table( )["CategoryFeature"]
	local t = formfunction( "selectCategoryFeatureList" )( {
		category_id = categoryFeature["category_id"]
	} )
	local f = form( "CategoryFeatureList" )
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

-- components/features associations

function createComponentFeature( )
	local componentFeature = input:table( )["ComponentFeature"]
	formfunction( "addComponentFeature" )( componentFeature )
end

function deleteComponentFeature( )
	local componentFeature = input:table( )["ComponentFeature"]
	formfunction( "deleteComponentFeature" )( componentFeature )
end

function ComponentFeatureRequest( )
	local componentFeature = input:table( )["ComponentFeature"]
	local t = formfunction( "selectComponentFeature" )( {
		component_id = componentFeature["component_id"],
		feature_id = componentFeature["feature_id"]
	} )
	local f = form( "ComponentFeature" )
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

function ComponentFeatureListRequest( )
	local componentFeature = input:table( )["ComponentFeature"]
	local t = formfunction( "selectComponentFeatureList" )( {
		component_id = componentFeature["component_id"]
	} )
	local f = form( "ComponentFeatureList" )
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

-- recipe/category (content) associations

function createRecipeContent( )
	local recipeContent = input:table( )["RecipeContent"]
	formfunction( "addRecipeContent" )( recipeContent )
end

function deleteRecipeContent( )
	local recipeContent = input:table( )["RecipeContent"]
	formfunction( "deleteRecipeContent" )( recipeContent )
end

function RecipeContentRequest( )
	local recipeContent = input:table( )["RecipeContent"]
	local t = formfunction( "selectRecipeContent" )( {
		recipe_id = recipeContent["recipe_id"],
		category_id = recipeContent["category_id"]
	} )
	local f = form( "RecipeContent" )
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

function RecipeContentListRequest( )
	local recipeContent = input:table( )["RecipeContent"]
	local t = formfunction( "selectRecipeContentList" )( {
		recipe_id = recipeContent["recipe_id"]
	} )
	local f = form( "RecipeContentList" )
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

-- pictures

function PictureListRequest( )
	output:as( {root='list', system='PictureList.simpleform'})
	filter().empty = false
	local search = nil
	for v,t in input:get( ) do
		if t == "search" then
			search = "%" .. normalizer( "name" )( v ) .. "%"
		end
	end
	if search == nil then
		search = "%%"
	end
	local t = formfunction( "selectPictureList" )( { search = search } )
	local f = form( "Picture" )
	f:fill( t:get( ) )
	output:print( f:get( ) )
end

function PictureRequest( )
	output:as( {root='dummy', system='Picture.simpleform'})
	filter().empty = false
	local id = nil
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
	-- should be a form transformation, not lua code :-)
	local picture = {}
	picture["tags"] = {}
	local intag = false
	local intagwrap = false
	local inid = false
	for v,t in itr do
		if( not v and t ) then
			-- begin tag
			if( t == "tagwrap" ) then
				intagwrap = true
			elseif( t == "tag" ) then
				intag = true
			elseif( t == "caption" or t == "info" or t == "image" ) then
				picture[ t] = content_value( scope( itr))
			end
		elseif( v and t ) then
			-- attribute
			inid = true
			if ( ( t == "id" ) and not intagwrap and not intag ) then
				picture[ t] = v
			elseif( t == "id" and intag and intagwrap ) then
				table.insert( picture["tags"], { ["id"] = v } )
			end
		elseif( not v and not t ) then
			-- end tag
			if( inid ) then
				inid = false
			elseif( intag and intagwrap ) then
				intag = false
			elseif( intagwrap ) then
				intagwrap = false
			end
		else
			-- dummy content
		end
	end
	info = formfunction( "imageInfo" )( { [ "data"] = picture["image"] } ):table( )
	picture["width"] = info.width
	picture["height"] = info.height
	thumb = formfunction( "imageThumb" )( { [ "image" ] = { [ "data" ] = picture["image"] }, [ "size" ] = 50 } ):table( )
	picture["thumbnail"] = thumb.data
	return picture
end

local function delete_picture( itr)
	local id = nil
	for v,t in itr do
		if t == "id" then
			id = v
		end
	end
	formfunction( "deletePicture" )( { id = id } )
end

local function update_picture( itr)
	local picture = transform_picture( itr)
	formfunction( "updatePicture" )( { picture = picture } )
end

local function create_picture( itr)
	local picture = transform_picture( itr)
	formfunction( "addPicture" )( { picture = picture } )
end

function editPicture( )
	update_picture( input:get())
end

function createPicture( )
	create_picture( input:get())
end

function deletePicture( )
	delete_picture( input:get())
end


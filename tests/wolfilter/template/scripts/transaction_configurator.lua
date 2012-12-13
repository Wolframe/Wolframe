local function create_manufacturer(itr)
	local manufacturerform = form("Manufacturer");
	manufacturerform:fill(itr)
	local manufacturer = manufacturerform:table()["manufacturer"]
	manufacturer["normalizedName"] = normalizer( "name" )( manufacturer["name"] )
	manufacturer["logo"] = manufacturer["picture"]["id"]
	formfunction( "addManufacturer" )( manufacturer )
end

function run()
	filter().empty = false
	output:as( {root='result', system='test.simpleform'})
	output:opentag("result")
	local itr = input:get()
	for v,t in itr do
		if (t == "pushCategoryHierarchy") then
			add_tree( "Category", scope(itr))
		elseif (t == "pushFeatureHierarchy") then
			add_tree( "Feature", scope(itr))
		elseif (t == "pushTagHierarchy") then
			add_tree( "Tag", scope(itr))
		elseif (t == "CategoryHierarchyRequest") then
			select_tree( "Category", "category", scope(itr))
		elseif (t == "FeatureHierarchyRequest") then
			select_tree( "Feature", "feature", scope(itr))
		elseif (t == "TagHierarchyRequest") then
			select_tree( "Tag", "tag", scope(itr))
		elseif (t == "editCategory") then
			edit_node( "Category", scope(itr))
		elseif (t == "editFeature") then
			edit_node( "Feature", scope(itr))
		elseif (t == "editTag") then
			edit_node( "Tag", scope(itr))
		elseif (t == "deleteCategory") then
			delete_node( "Category", scope(itr))
		elseif (t == "deleteFeature") then
			delete_node( "Feature", scope(itr))
		elseif (t == "deleteTag") then
			delete_node( "Tag", scope(itr))
		elseif (t == "createCategory") then
			create_node( "Category", scope(itr))
		elseif (t == "createFeature") then
			create_node( "Feature", scope(itr))
		elseif (t == "createTag") then
			create_node( "Tag", scope(itr))
		elseif (t == "CategoryRequest") then
			select_node( "Category", "category", scope(itr))
		elseif (t == "FeatureRequest") then
			select_node( "Feature", "feature", scope(itr))
		elseif (t == "TagRequest") then
			select_node( "Tag", "tag", scope(itr))
		elseif (t == "createPicture") then
			create_picture(scope(itr))
		elseif (t == "editPicture") then
			update_picture(scope(itr))
		elseif (t == "deletePicture") then
			delete_picture( scope(itr))
		elseif (t == "createManufacturer") then
			create_manufacturer( scope(itr))
		end
	end
	output:closetag()
end

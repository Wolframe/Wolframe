
function run()
	filter().empty = false
	output:as( "result SYSTEM 'test.simpleform'")
	output:opentag("result")
	local itr = input:get()
	for v,t in itr do
		if (t == "addCategoryHierarchy") then
			add_tree( "Category", scope(itr))
		elseif (t == "addFeatureHierarchy") then
			add_tree( "Feature", scope(itr))
		elseif (t == "selectCategoryHierarchy") then
			select_tree( "Category", scope(itr))
		elseif (t == "selectFeatureHierarchy") then
			select_tree( "Feature", scope(itr))
		end
	end
	output:closetag()
end

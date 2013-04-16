local function insertLanguageTreeNode( parent, node)
	local nodeid = 1
	if node["name"] then
		local nodetab =
		{
			parentid = parent,
			name = node["name"],
			status = node["status"],
			description = node["description"]
		}
		nodeid = formfunction("InsertLanguage")( nodetab ):table()["id"]
	end
	for t,v in pairs(node) do
		if t == "language" then
			for i,c in ipairs(v) do
				insertLanguageTreeNode( nodeid, c)
			end
		end
	end
end

function InsertLanguageTree( inp)
	local node = inp:table();

	for t,v in pairs(node) do
		if t == "language" then
			for i,c in ipairs(v) do
				insertLanguageTreeNode( 1, c)
			end
		end
	end
	return {}
end

function GetLanguageTreeStructure( inp)
	local nodear = formfunction("GetLanguageTree")( inp:get() )
	local nodetab = { {} }
	local linktab = {}

	-- build the array nodetab of all nodes
	for t,node in pairs( nodear:table()[ "language"]) do
		local nodeid = tonumber( node[ 'id'])
		table.insert( nodetab, nodeid,
		{
			id = nodeid,
			name = node[ 'name']
		})
		table.insert( linktab, nodeid, tonumber( node[ 'parentid']))
	end
	-- insert all nodes into their parent node structure
	for index,node in ipairs (nodetab) do
		local parentid = linktab[ index]
		if (parentid) then
			if nodetab[ parentid].language then
				table.insert( nodetab[ parentid].language, nodetab[ index])
			else
				nodetab[ parentid].language = { nodetab[ index] }
			end
		end
	end
	-- return the root
	logger.printc( "RESULT TREE ", nodetab[1])
	return nodetab[1]
end


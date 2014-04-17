function run()
	t = input:table()

	output:as( input:doctype(), provider.filter())

	output:opentag( "issuelist")		-- issuelist
	output:opentag( "issue")		-- issue
	output:opentag( "")			-- array element
		output:print( t[ "issuelist"][ "issue"][1]["date"], "date");
		output:opentag( "subtask")	-- subtask
			output:opentag( "")	-- array element
			output:print( t[ "issuelist"][ "issue"][1]["subtask"][1]["key"], "key");
			output:print( t[ "issuelist"][ "issue"][1]["subtask"][1]["title"], "title");
			output:closetag()	-- array element
			output:opentag( "")	-- array element
			output:print( t[ "issuelist"][ "issue"][1]["subtask"][2]["key"], "key");
			output:print( t[ "issuelist"][ "issue"][1]["subtask"][2]["title"], "title");
			output:closetag()	-- array element
		output:closetag()		-- end subtask
		output:opentag( "system")	-- system
			output:print( t[ "issuelist"][ "issue"][1]["system"]["machine"], "machine");
			output:print( t[ "issuelist"][ "issue"][1]["system"]["OS"], "OS");
			output:print( t[ "issuelist"][ "issue"][1]["system"]["version"], "version");
		output:closetag()		-- end system
	output:closetag()			-- end array element

	output:opentag( "")			-- array element
	output:print( t[ "issuelist"][ "issue"][2]["date"], "date");
	output:opentag( "subtask")		-- subtask
		output:opentag( "")		-- array element
		output:print( t[ "issuelist"][ "issue"][2]["subtask"][1]["key"], "key");
		output:print( t[ "issuelist"][ "issue"][2]["subtask"][1]["title"], "title");
		output:closetag()		-- end array element
	output:closetag()			-- end subtask
	output:opentag( "system")
		output:print( t[ "issuelist"][ "issue"][2]["system"]["machine"], "machine");
		output:print( t[ "issuelist"][ "issue"][2]["system"]["OS"], "OS");
		output:print( t[ "issuelist"][ "issue"][2]["system"]["version"], "version");
	output:closetag()			-- end system
	output:closetag()			-- end array element

	output:closetag()			-- end issue
	output:closetag()			-- end issuelist
end


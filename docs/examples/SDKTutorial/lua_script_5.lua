
function run()
	f = filter( "XML:textwolf")
	-- filter out tokens containing only spaces:
	f.empty = false
	input:as( f)
	output:as( f)
	t = input:value()

	-- print the document from the table
	output:opentag( 'doc')
	output:opentag( 'name')
	output:print( t['doc']['name'])
	output:closetag()
	output:opentag( 'country')
	output:print( t['doc']['country'])
	output:closetag()
	output:closetag()
end


function process_content( itr)
	for c in itr do
		output:print( c)
	end
end

function run( )
	filter f = filter( "XML:libxml2")
	input:as(f)
	output:as(f)
	for c,t in input:get() do
		output:print( c, t)
		if t = "content" then
			process_content( input:get())
			output:print( false, false)
		end
	end
end

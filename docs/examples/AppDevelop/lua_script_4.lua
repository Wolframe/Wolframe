
function process_content( itr)
	for c,t in itr do
		output:print( c, t)
	end
end

function run( )
	local f = provider.filter( "libxml2")
	input:as(f)
	output:as(f)
	i = input:get()
	for c,t in i do
		output:print( c, t)
		if t == "content" then
			process_content( iterator.scope( i))
			output:print( false, false)
		end
	end
end

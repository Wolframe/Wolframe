function run( )
	input:as( filter( "char:UTF-16"))
	output:as( filter( "char:UTF-16"))
	for c in input:get() do
		output:print( c)
	end
end

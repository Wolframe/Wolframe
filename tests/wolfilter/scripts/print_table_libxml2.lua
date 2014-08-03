function run()
	f = provider.filter( "libxml2")
	f.empty = false
	input:as( f)
	output:as( provider.filter( "blob"))
	local t = input:get()
	local f = provider.formfunction( "print_invoice")
	for val,tag in f(t):get() do
		output:print( val)
	end
end


function run()
	local f = provider.filter( "textwolf")
	f.empty = false
	input:as( f)
	output:as( provider.filter( "blob"))
	local t = input:get()
	local f = provider.formfunction( "print_invoice")
	for val,tag in f(t):get() do
		output:print( val)
	end
end


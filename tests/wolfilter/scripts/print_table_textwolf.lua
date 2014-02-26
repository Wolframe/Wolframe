
function run()
	f = provider.filter( "textwolf")
	f.empty = false
	input:as( f)
	output:as( provider.filter( "blob"))
	t = input:table()
	f = provider.formfunction( "print_invoice")
	output:print( f( t):table())
end


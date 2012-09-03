
function run()
	f = filter( "xml:textwolf")
	f.empty = false
	input:as( f)
	output:as( f)
	t = input:table()
	f = formfunction( "print_invoice")
	output:print( f( t))
end


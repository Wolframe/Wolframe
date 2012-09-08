
function run( )
        input:as( filter( "char", "UTF-16BE"))
        output:as( filter( "char", "UTF-16BE"))
        for c in input:get() do
                output:print( c)
        end
end

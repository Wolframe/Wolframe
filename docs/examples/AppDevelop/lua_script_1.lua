
function run( )
        local f = provider.filter( "char", "UTF-16BE")
        input:as( f)
        output:as( f)
        for c in input:get() do
                output:print( c)
        end
end

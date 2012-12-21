
function run( )
        local f = filter( "char", "UTF-16BE")
        input:as( f)
        output:as( f)
        for c in input:get() do
                output:print( c)
        end
end

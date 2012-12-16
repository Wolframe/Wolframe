
function run( )
        local f = filter( "line", "UTF-8")
        input:as( f)
        output:as( f)
        for c in input:get() do
                output:print( c)
        end
end

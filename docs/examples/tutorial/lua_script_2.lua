
function run( )
        input:as( filter( "line:UTF-8"))
        output:as( filter( "line:UTF-8"))
        for c in input:get() do
                output:print( c)
        end
end

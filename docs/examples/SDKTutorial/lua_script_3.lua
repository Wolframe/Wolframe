
function run( )
        f = filter( "XML:textwolf")
        input:as(f)
        output:as(f)
        for c,t in input:get() do
                output:print( c, t)
        end
end

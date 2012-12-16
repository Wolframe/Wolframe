
function run()
        f = filter( "XML:textwolf")
        -- A W3C conform XML parser returns by default also tokens containing only spaces.
        -- The following line tells the input filter to ignore elements consisting only of
        -- spaces and control characters:
        f.empty = false
        input:as( f)
        output:as( f)
        t = input:table()

        -- print the document from the table
        output:opentag( 'doc')
        output:opentag( 'name')
        output:print( t['doc']['name'])
        output:closetag()
        output:opentag( 'country')
        output:print( t['doc']['country'])
        output:closetag()
        output:closetag()
end


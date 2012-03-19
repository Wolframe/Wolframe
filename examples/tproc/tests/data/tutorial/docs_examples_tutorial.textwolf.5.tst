**
**requires:LUA
**input
HELLO
RUN
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 B E "   s t a n d a l o n e = " y e s " ? > 
 < d o c > < n a m e > m u m b a y < / n a m e > < c o u n t r y > i n d i a < / c o u n t r y > < c o n t i n e n t > a s i a < / c o n t i n e n t > < / d o c >
.
QUIT
**file:example_5.lua

function readTable( itr)
        local tab = {}
        -- function result maps a vector with one element to this element
        function result( t)
                if #t == 1 and t[1] then
                        return t[1]
                else
                        return t
                end
        end

        for v,t in itr do
                if t then
                        local val = v or readTable( itr)
                        local e = tab[t]
                        if e then
                                if type(e) == "table" and e[#e] then
                                        table.insert( tab[t], val)
                                else
                                        tab[t] = { tab[t], val }
                                end
                        else
                                tab[t] = val
                        end
                elseif v then
                        table.insert( tab, v)
                else
                        return result( tab)
                end
        end
        return result( tab)
end

function run()
        f = filter( "xml:textwolf")

        -- a W3C conform XML parser returns by default also tokens containing only spaces.
        -- these we eliminate to be able to create a table:
        f.empty = false

        input:as(f)
        output:as(f)

        t = readTable( input:get())

        -- print the document from the table
        output:print( false, 'doc')
        output:print( false, 'name')
        if t['doc']['name'] then
                output:print( t['doc']['name'])
        end
        output:print( false, false)
        output:print( false, 'country')
        if t['doc']['country'] then
                output:print( t['doc']['country'])
        end
        output:print( false, false)
        output:print( false, false)
end

**config
script {
	name RUN
	path example_5.lua
	main run
}
**output
OK enter cmd
 < ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 B E "   s t a n d a l o n e = " y e s " ? > 
 < d o c > < n a m e > m u m b a y < / n a m e > < c o u n t r y > i n d i a < / c o u n t r y > < / d o c >
.
OK
BYE
**end


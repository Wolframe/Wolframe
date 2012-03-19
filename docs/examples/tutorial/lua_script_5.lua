
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
        output:opentag( 'doc')
        output:opentag( 'name')
        if t['doc']['name'] then
                output:print( t['doc']['name'])
        end
        output:closetag()
        output:opentag( 'country')
        if t['doc']['country'] then
                output:print( t['doc']['country'])
        end
        output:closetag()
        output:closetag()
end


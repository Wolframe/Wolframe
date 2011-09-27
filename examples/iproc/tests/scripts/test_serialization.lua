
function readTable( itr)
	local tab = {}
	local i = -1
	function index()
		i=i+1
		return i
	end
	for t,v in itr() do
		local val = v or readTable( itr);
		if t then
			if tab[t]
				local e = tab[t]
				if type(e) == "table" and tab[ #e] then
					tab.insert( val)
				else
					tab[t] = { tab[t], val }
				end
			else
				tab[t] = val;
			end
		else
			tab[index()] = val
		end
	end
	return tab
end

function printTable( tab)
	for t,v in pairs(tab) do
		if type(v) == "table" then
			output:print( false, t)
			printTable( tab)
		else
			output:print( v, t)
		end
	end
end

function run()
	iof = filter( "xml:textwolf")
	input:as( iof)
	output:as( iof)
	t = readTable( input.get())
	t = tranform( t)
	printTable( t)
end


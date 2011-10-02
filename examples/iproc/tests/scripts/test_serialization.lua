
function readTable( itr)
	local tab = {}
	local i = -1
	function index()
		i=i+1
		return i
	end
	for v,t in itr do
		local val = v or readTable( itr);
		if t then
			if tab[t] then
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
	t = readTable( input:get())
--	t = transform( t)
	printTable( t)
end


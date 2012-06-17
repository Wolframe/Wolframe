**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><issuedate>13.5.2006</issuedate><employee><firstname>Julia</firstname><phone>098 765 43 21</phone><surname>Tegel-Sacher</surname></employee><task><customernumber>324</customernumber><title>job 1</title><key>A123</key></task><task><customernumber>567</customernumber><title>job 2</title><key>V456</key></task></assignment><assignment><issuedate>13.5.2006</issuedate><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><task><customernumber>567</customernumber><title>job 3</title><key>A456</key></task><task><customernumber>890</customernumber><title>job 4</title><key>V789</key></task></assignment></assignmentlist>
**file: formfunc_luatable_luaparse.lua

function readTable( itr)
	local tab = {}
	-- function result maps a vector with one element to this element
	function result( t)
		if #t == 1 and t[1] then
			return t[1]
		else
			return t;
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
					v = { tab[t], val }
					tab[t] = v
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

function printArray( t, a)
	for i,v in ipairs(a) do
		if type(v) == "table" then
			output:print( false, t)
			printTable(v)
			output:print()
		else
			output:print( v, t)
		end
	end
end

function printTable( tab)
	local iscontent = true
	--... we do not print attributes here
	--... iscontent=false -> print atomic values as attributes until the first non atomic value
	--... iscontent=false -> print all values as tag content
	for t,v in pairs(tab) do
		local tagname = t
		if type(v) == "table" then
			iscontent = true
			if v[#v] then
				printArray( t, v)
			else
				output:print( false, t)
				printTable( v)
				output:print()
			end
		elseif type(t) == "number" then
			output:print( v)
		elseif iscontent then
			output:print( false, t)
			output:print( v)
			output:print()
		else
			output:print( v, t)
		end
	end
end

function run()
	f = filter( "xml:textwolf")

	-- a W3C conform XML parser returns by default also tokens containing only spaces.
	-- these we eliminate to be able to create a table:
	f.empty = false

	input:as(f)
	output:as(f)

	t = readTable( input:get())
	r = formfunction("employee_assignment_convert")( t)
	printTable( r:table())
end

**config
--script formfunc_luatable_luaparse.lua run xml:textwolf
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><issuedate>8647993</issuedate><employee><firstname>juulliiaa</firstname><phone>901 234 56 78</phone><surname>teeggeell-saacchheerr</surname></employee><task><customernumber>325</customernumber><title>jjoobb 8</title><key>a876</key></task><task><customernumber>568</customernumber><title>jjoobb 7</title><key>v543</key></task></assignment><assignment><issuedate>8647993</issuedate><employee><firstname>jaakkoobb</firstname><phone>987 654 32 10</phone><surname>stteeggeelliinn</surname></employee><task><customernumber>568</customernumber><title>jjoobb 6</title><key>a543</key></task><task><customernumber>891</customernumber><title>jjoobb 5</title><key>v210</key></task></assignment></assignmentlist>
**end

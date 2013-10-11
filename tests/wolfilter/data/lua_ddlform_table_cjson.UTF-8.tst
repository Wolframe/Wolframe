**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
{
	"assignmentlist":	{
		"assignment":	[{
				"task":	[{
						"title":	"job 1",
						"key":	"A123",
						"customernumber":	"324"
					}, {
						"title":	"job 2",
						"key":	"V456",
						"customernumber":	"567"
					}],
				"employee":	{
					"firstname":	"Julia",
					"surname":	"Tegel-Sacher",
					"phone":	"098 765 43 21"
				},
				"issuedate":	"13.5.2006"
			}, {
				"task":	[{
						"title":	"job 3",
						"key":	"A456",
						"customernumber":	"567"
					}, {
						"title":	"job 4",
						"key":	"V789",
						"customernumber":	"890"
					}],
				"employee":	{
					"firstname":	"Jakob",
					"surname":	"Stegelin",
					"phone":	"012 345 67 89"
				},
				"issuedate":	"13.5.2006"
			}]
	}
}**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson  --module ../../src/modules/cmdbind/lua/mod_command_lua --module ../../src/modules/ddlcompiler//simpleform/mod_ddlcompiler_simpleform --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/normalize//string/mod_normalize_string --program simpleform_range.wnmp --program employee_assignment_print.sfrm --cmdprogram ddlform_table.lua run

**file:simpleform_range.wnmp
iNt=number:integer(10);
uint=numbeR:unsigned(10);
float=number:fLoat(10,10);
currency=number:fixedpoint(13,2);
percent_1=number:fixedpoint(5,1);
**file: ddlform_table.lua

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
	t = readTable( input:get())
	r = form("employee_assignment_print")
	r:fill( t)
	printTable( r:table())
end

**file: employee_assignment_print.sfrm
FORM Employee
{
	firstname string
	surname string
	phone string
}

FORM employee_assignment_print
{
	assignmentlist
	{
		assignment []
		{
			task []
			{
				title string
				key string
				customernumber int
			}
			employee Employee
			issuedate string
		}
	}
}

**output
{
	"assignmentlist":	{
		"assignment":	[{
				"issuedate":	"13.5.2006",
				"employee":	{
					"firstname":	"Julia",
					"phone":	"098 765 43 21",
					"surname":	"Tegel-Sacher"
				},
				"task":	[{
						"customernumber":	"324",
						"key":	"A123",
						"title":	"job 1"
					}, {
						"customernumber":	"567",
						"key":	"V456",
						"title":	"job 2"
					}]
			}, {
				"issuedate":	"13.5.2006",
				"employee":	{
					"firstname":	"Jakob",
					"phone":	"012 345 67 89",
					"surname":	"Stegelin"
				},
				"task":	[{
						"customernumber":	"567",
						"key":	"A456",
						"title":	"job 3"
					}, {
						"customernumber":	"890",
						"key":	"V789",
						"title":	"job 4"
					}]
			}]
	}
}
**end

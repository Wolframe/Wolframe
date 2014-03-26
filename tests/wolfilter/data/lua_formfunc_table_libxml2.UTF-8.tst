**
**requires:LUA
**requires:LIBXML2
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><task><title>job 1</title><key>A123</key><customernumber>324</customernumber></task><task><title>job 2</title><key>V456</key><customernumber>567</customernumber></task><employee><firstname>Julia</firstname><surname>Tegel-Sacher</surname><phone>098 765 43 21</phone></employee><issuedate>13.5.2006</issuedate></assignment><assignment><task><title>job 3</title><key>A456</key><customernumber>567</customernumber></task><task><title>job 4</title><key>V789</key><customernumber>890</customernumber></task><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><issuedate>13.5.2006</issuedate></assignment></assignmentlist>**config
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2 -c wolframe.conf run

**file:wolframe.conf
LoadModules
{
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../wolfilter/modules/employee_assignment_convert/mod_employee_assignment_convert
}
Processor
{
	cmdhandler
	{
		lua
		{
			program script.lua
		}
	}
}
**file:script.lua
function printTable( tab)
	-- deterministic print of a table (since lua 5.2.1 table keys order is non deterministic)

	keys = {}
	for key,val in pairs( tab) do
		table.insert( keys, key)
	end
	table.sort( keys)

	for i,t in ipairs( keys) do
		local v = tab[ t]

		if type(v) == "table" then
			if v[ #v] then
				-- print array (keys are indices)
				for eidx,elem in ipairs( v) do
					output:opentag( t)
					if type(elem) == "table" then
						printTable( elem)
					else
						output:print( elem)
					end
					output:closetag()
				end
			else
				-- print table (keys are values)
				output:opentag( t)
				printTable( v)
				output:closetag()
			end
		else
			output:opentag( t)
			output:print( v)
			output:closetag()
		end
	end
end

function run()
	t = input:table()
	r = provider.formfunction("employee_assignment_convert")( t)
	printTable( r:table())
end
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><employee><firstname>juulliiaa</firstname><phone>901 234 56 78</phone><surname>teeggeell-saacchheerr</surname></employee><issuedate>8647993</issuedate><task><customernumber>325</customernumber><key>a876</key><title>jjoobb 8</title></task><task><customernumber>568</customernumber><key>v543</key><title>jjoobb 7</title></task></assignment><assignment><employee><firstname>jaakkoobb</firstname><phone>987 654 32 10</phone><surname>stteeggeelliinn</surname></employee><issuedate>8647993</issuedate><task><customernumber>568</customernumber><key>a543</key><title>jjoobb 6</title></task><task><customernumber>891</customernumber><key>v210</key><title>jjoobb 5</title></task></assignment></assignmentlist>
**end

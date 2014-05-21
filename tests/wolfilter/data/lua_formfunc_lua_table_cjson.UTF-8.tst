**
**requires:LUA
**requires:CJSON
**requires:TEXTWOLF
**input
{
    "assignment":  [{
        "task":  [{
            "title":  "job 1",
            "key":  "A123",
            "customernumber":  "324"
          }, {
            "title":  "job 2",
            "key":  "V456",
            "customernumber":  "567"
          }],
        "employee":  {
          "firstname":  "Julia",
          "surname":  "Tegel-Sacher",
          "phone":  "098 765 43 21"
        },
        "issuedate":  "13.5.2006"
      }, {
        "task":  [{
            "title":  "job 3",
            "key":  "A456",
            "customernumber":  "567"
          }, {
            "title":  "job 4",
            "key":  "V789",
            "customernumber":  "890"
          }],
        "employee":  {
          "firstname":  "Jakob",
          "surname":  "Stegelin",
          "phone":  "012 345 67 89"
        },
        "issuedate":  "13.5.2006"
      }]
}**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson -c wolframe.conf run

**file:wolframe.conf
LoadModules
{
	module ../../src/modules/cmdbind/lua/mod_command_lua
	module ../wolfilter/modules/employee_assignment_convert/mod_employee_assignment_convert
}
Processor
{
	program program_formfunc.lua
	cmdhandler
	{
		lua
		{
			program script.lua
		}
	}
}
**file:program_formfunc.lua
function toupper_table( tb)
	for i,v in pairs(tb)
	do
		if type(v) == "table" then
			tb[ i] = toupper_table(v)
		else
			tb[ i] = string.upper(v)
		end
	end
	return tb
end

function toupper_formfunc( inp)
	return toupper_table( inp:table())
end
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
	res = provider.formfunction( "toupper_formfunc")( input:table())
	printTable( res:table())
end
**requires:DISABLED NETBSD

**output
{
  "assignment":  [{
      "employee":  {
        "firstname":  "JULIA",
        "phone":  "098 765 43 21",
        "surname":  "TEGEL-SACHER"
      },
      "issuedate":  "13.5.2006",
      "task":  [{
          "customernumber":  "324",
          "key":  "A123",
          "title":  "JOB 1"
        }, {
          "customernumber":  "567",
          "key":  "V456",
          "title":  "JOB 2"
        }]
    }, {
      "employee":  {
        "firstname":  "JAKOB",
        "phone":  "012 345 67 89",
        "surname":  "STEGELIN"
      },
      "issuedate":  "13.5.2006",
      "task":  [{
          "customernumber":  "567",
          "key":  "A456",
          "title":  "JOB 3"
        }, {
          "customernumber":  "890",
          "key":  "V789",
          "title":  "JOB 4"
        }]
    }]
}
**end

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
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson  --module ../../src/modules/cmdbind/lua/mod_command_lua --module ../wolfilter/modules/database/testtrace/mod_db_testtrace --database 'identifier=testdb,outfile=DBOUT,file=DBRES' --program=DBIN.tdl --cmdprogram transaction_iterator.lua run

**file: DBRES
#name#job1#job2#job3#job4
**file:DBIN.tdl
TRANSACTION test_transaction
RESULT INTO doc
BEGIN
	INTO title FOREACH //task DO run( title);
END
**file: transaction_iterator.lua

function run()
	r = formfunction("test_transaction")( input:get())
	for v,t in r:get() do
		output:print( v,t)
	end
end

**output
{
	"doc":	{
		"title":	[{
				"name":	"job1"
			}, {
				"name":	"job2"
			}, {
				"name":	"job3"
			}, {
				"name":	"job4"
			}]
	}
}
**end

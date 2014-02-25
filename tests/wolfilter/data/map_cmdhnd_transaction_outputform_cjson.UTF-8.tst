**
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
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson -c wolframe.conf employee_assignment_print

**file:wolframe.conf
LoadModules
{
	module NOTFOUND:mod_db_testtrace
	module NOTFOUND:mod_normalize_number
	module NOTFOUND:mod_normalize_string
	module NOTFOUND:mod_command_directmap
	module NOTFOUND:mod_ddlcompiler_simpleform
}
Database
{
	test
	{
		identifier testdb
		outfile DBOUT
		file DBRES
	}
}
Processor
{
	database testdb
	program DBIN.tdl
	program ../wolfilter/scripts/map_cmdhnd_transaction_outputform.sfrm
	program ../wolfilter/scripts/employee_assignment_print.sfrm
	program ../wolfilter/template/program/simpleform.wnmp

	cmdhandler
	{
		directmap
		{
			program test.dmap
			filter cjson
		}
	}
}
**file: map_cmdhnd_transaction_outputform.sfrm
FORM map_cmdhnd_transaction_outputform
{
	doc
	{
		task []
		{
			task string
			start string
			end string
			id int
		}
	}
}
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
**file: test.dmap
COMMAND(employee_assignment_print) CALL(test_transaction) RETURN (map_cmdhnd_transaction_outputform);
**file: DBRES
#id Task start end#11 'bla bla' '12:04:19 1/3/2012' '12:41:34 1/3/2012'#12 'bli blu' '07:14:23 1/3/2012' '08:01:51 1/3/2012'
#id task Start end#21 'gardening' '09:24:28 1/3/2012' '11:11:07 1/3/2012'#22 'helo' '11:31:01 1/3/2012' '12:07:55 1/3/2012'
#ID task start End#31 'hula hop' '19:14:38 1/4/2012' '20:01:12 1/4/2012'#32 'hula hip' '11:31:01 1/3/2012' '12:07:55 1/3/2012'#33 'hula hup' '11:31:01 1/3/2012' '12:07:55 1/3/2012'
**file:DBIN.tdl
TRANSACTION test_transaction
BEGIN
	INTO task FOREACH //task DO run( title);
END
**output
{
	"doctype":	"map_cmdhnd_transaction_outputform.simpleform",
	"doc":	{
		"task":	[{
				"task":	"bla bla",
				"start":	"12:04:19 1/3/2012",
				"end":	"12:41:34 1/3/2012",
				"id":	"11"
			}, {
				"task":	"bli blu",
				"start":	"07:14:23 1/3/2012",
				"end":	"08:01:51 1/3/2012",
				"id":	"12"
			}, {
				"task":	"gardening",
				"start":	"09:24:28 1/3/2012",
				"end":	"11:11:07 1/3/2012",
				"id":	"21"
			}, {
				"task":	"helo",
				"start":	"11:31:01 1/3/2012",
				"end":	"12:07:55 1/3/2012",
				"id":	"22"
			}, {
				"task":	"hula hop",
				"start":	"19:14:38 1/4/2012",
				"end":	"20:01:12 1/4/2012",
				"id":	"31"
			}, {
				"task":	"hula hip",
				"start":	"11:31:01 1/3/2012",
				"end":	"12:07:55 1/3/2012",
				"id":	"32"
			}, {
				"task":	"hula hup",
				"start":	"11:31:01 1/3/2012",
				"end":	"12:07:55 1/3/2012",
				"id":	"33"
			}]
	}
}
**end

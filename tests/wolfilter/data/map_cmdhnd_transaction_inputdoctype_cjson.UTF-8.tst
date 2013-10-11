**
**requires:CJSON
**requires:TEXTWOLF
**input
{
	"doctype":	"employee_assignment_print",
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
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson  --program employee_assignment_print.sfrm --module ../../src/modules/ddlcompiler//simpleform/mod_ddlcompiler_simpleform --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/normalize//string/mod_normalize_string --program simpleform_range_trim.wnmp --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../wolfilter/modules/database/testtrace/mod_db_testtrace --database 'identifier=testdb,outfile=DBOUT,file=DBRES' --program=DBIN.tdl --cmdprogram=test.dmap employee_assignment_print

**file: test.dmap
COMMAND(employee_assignment_print) CALL(test_transaction);
**file:simpleform_range_trim.wnmp
int=string:trim,number:integer(10);
uint=string:trim,number:unsigned(10);
float=string:trim,number:float(10,10);
currency=number:fixedpoint(13,2);
percent_1=number:fixedpoint(5,1);
**file: DBRES
#id task start end#11 'bla bla' '12:04:19 1/3/2012' '12:41:34 1/3/2012'#12 'bli blu' '07:14:23 1/3/2012' '08:01:51 1/3/2012'
#id task start end#21 'gardening' '09:24:28 1/3/2012' '11:11:07 1/3/2012'#22 'helo' '11:31:01 1/3/2012' '12:07:55 1/3/2012'
#id task start end#31 'hula hop' '19:14:38 1/4/2012' '20:01:12 1/4/2012'#32 'hula hip' '11:31:01 1/3/2012' '12:07:55 1/3/2012'#33 'hula hup' '11:31:01 1/3/2012' '12:07:55 1/3/2012'
**file:DBIN.tdl
TRANSACTION test_transaction
RESULT INTO doc
BEGIN
	INTO task FOREACH //task DO run( title);
END
**requires:DISABLED NETBSD

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
	"doc":	{
		"task":	[{
				"id":	"11",
				"task":	"bla bla",
				"start":	"12:04:19 1/3/2012",
				"end":	"12:41:34 1/3/2012"
			}, {
				"id":	"12",
				"task":	"bli blu",
				"start":	"07:14:23 1/3/2012",
				"end":	"08:01:51 1/3/2012"
			}, {
				"id":	"21",
				"task":	"gardening",
				"start":	"09:24:28 1/3/2012",
				"end":	"11:11:07 1/3/2012"
			}, {
				"id":	"22",
				"task":	"helo",
				"start":	"11:31:01 1/3/2012",
				"end":	"12:07:55 1/3/2012"
			}, {
				"id":	"31",
				"task":	"hula hop",
				"start":	"19:14:38 1/4/2012",
				"end":	"20:01:12 1/4/2012"
			}, {
				"id":	"32",
				"task":	"hula hip",
				"start":	"11:31:01 1/3/2012",
				"end":	"12:07:55 1/3/2012"
			}, {
				"id":	"33",
				"task":	"hula hup",
				"start":	"11:31:01 1/3/2012",
				"end":	"12:07:55 1/3/2012"
			}]
	}
}
**end

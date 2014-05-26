**
**requires:LIBXML2
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><task><title>job 1</title><key>A123</key><customernumber>324</customernumber></task><task><title>job 2</title><key>V456</key><customernumber>567</customernumber></task><employee><firstname>Julia</firstname><surname>Tegel-Sacher</surname><phone>098 765 43 21</phone></employee><issuedate>13.5.2006</issuedate></assignment><assignment><task><title>job 3</title><key>A456</key><customernumber>567</customernumber></task><task><title>job 4</title><key>V789</key><customernumber>890</customernumber></task><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><issuedate>13.5.2006</issuedate></assignment></assignmentlist>**config
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2 -c wolframe.conf employee_assignment_print

**file:wolframe.conf
LoadModules
{
	module ./../wolfilter/modules/database/testtrace/mod_db_testtrace
	module ./../../src/modules/normalize/number/mod_normalize_number
	module ./../../src/modules/normalize/string/mod_normalize_string
	module ./../../src/modules/cmdbind/directmap/mod_command_directmap
	module ./../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform
	module ./../../src/modules/datatype/bcdnumber/mod_datatype_bcdnumber
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
			filter libxml2
		}
	}
}
**file: map_cmdhnd_transaction_outputform.sfrm
FORM map_cmdhnd_transaction_outputform
	-root doc
{
	task []
	{
		task string
		start string
		end string
		id int
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
	-root assignmentlist
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
**file: test.dmap
COMMAND(employee_assignment_print) CALL(test_transaction) RETURN (map_cmdhnd_transaction_outputform);
**file: DBRES
#id Task start end#11 'bla bla' '12:04:19 1/3/2012' '12:41:34 1/3/2012'#12 'bli blu' '07:14:23 1/3/2012' '08:01:51 1/3/2012'
#id task Start end#21 'gardening' '09:24:28 1/3/2012' '11:11:07 1/3/2012'#22 'helo' '11:31:01 1/3/2012' '12:07:55 1/3/2012'
#ID task start End#31 'hula hop' '19:14:38 1/4/2012' '20:01:12 1/4/2012'#32 'hula hip' '11:31:01 1/3/2012' '12:07:55 1/3/2012'#33 'hula hup' '11:31:01 1/3/2012' '12:07:55 1/3/2012'
**file:DBIN.tdl
TRANSACTION test_transaction
BEGIN
	INTO task FOREACH //task DO SELECT run( title);
END
**output
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE doc SYSTEM "map_cmdhnd_transaction_outputform.simpleform"><doc><task><task>bla bla</task><start>12:04:19 1/3/2012</start><end>12:41:34 1/3/2012</end><id>11</id></task><task><task>bli blu</task><start>07:14:23 1/3/2012</start><end>08:01:51 1/3/2012</end><id>12</id></task><task><task>gardening</task><start>09:24:28 1/3/2012</start><end>11:11:07 1/3/2012</end><id>21</id></task><task><task>helo</task><start>11:31:01 1/3/2012</start><end>12:07:55 1/3/2012</end><id>22</id></task><task><task>hula hop</task><start>19:14:38 1/4/2012</start><end>20:01:12 1/4/2012</end><id>31</id></task><task><task>hula hip</task><start>11:31:01 1/3/2012</start><end>12:07:55 1/3/2012</end><id>32</id></task><task><task>hula hup</task><start>11:31:01 1/3/2012</start><end>12:07:55 1/3/2012</end><id>33</id></task></doc>
**end

**
**requires:LIBXML2
**requires:SQLITE3
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><task><title>job 1</title><id>1</id><customernumber>324</customernumber></task><task><title>job 2</title><id>2</id><customernumber>567</customernumber></task><employee><firstname>Julia</firstname><surname>Tegel-Sacher</surname><phone>098 765 43 21</phone></employee><issuedate>13.5.2006</issuedate></assignment><assignment><task><title>job 3</title><id>3</id><customernumber>567</customernumber></task><task><title>job 4</title><id>4</id><customernumber>890</customernumber></task><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><issuedate>13.5.2006</issuedate></assignment></assignmentlist>**config
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2  --program schema_select_task_by_id.sfrm --program simpleform.wnmp --module ../../src/modules/ddlcompiler//simpleform/mod_ddlcompiler_simpleform --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/normalize//string/mod_normalize_string --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../wolfilter/modules/database/sqlite3/mod_db_sqlite3test --database 'identifier=testdb,file=test.db,dumpfile=DBDUMP,inputfile=DBDATA' --program=DBPRG.tdl --cmdprogram=test.dmap schema_select_task_by_id

**file: test.dmap
COMMAND schema_select_task_by_id CALL test_transaction RETURN STANDALONE doc;
**file:simpleform.wnmp
int=number:integer;
uint=number:unsigned;
float=number:float;
currency=number:fixedpoint(13,2);
percent_1=number:fixedpoint(5,1);
**file: DBDATA
CREATE TABLE task
(
	title STRING,
	id INTEGER,
	start DATE,
	end DATE
);

INSERT INTO task (title,id,start,end) VALUES ('bla bla', '1', '1/4/2012 12:04:19', '1/4/2012 12:41:14');
INSERT INTO task (title,id,start,end) VALUES ('bli blu', '2', '2/4/2012 11:14:29', '2/4/2012 12:11:34');
INSERT INTO task (title,id,start,end) VALUES ('blu bli', '3', '3/4/2012 17:11:13', '3/4/2012 18:19:31');
INSERT INTO task (title,id,start,end) VALUES ('ble ble', '4', '4/4/2012 19:14:29', '4/4/2012 19:58:44');
**file:DBPRG.tdl
TRANSACTION test_transaction
BEGIN
	INTO task FOREACH //task DO SELECT * FROM task WHERE id=$(id) ORDER BY id ASC;
END
**outputfile:DBDUMP
**file: schema_select_task_by_id.sfrm
FORM Employee
{
	firstname string
	surname string
	phone string
}

FORM schema_select_task_by_id
{
	assignmentlist
	{
		assignment []
		{
			task []
			{
				id int
				title string
				customernumber int
			}
			employee Employee
			issuedate string
		}
	}
}

**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><task><title>bla bla</title><id>1</id><start>1/4/2012 12:04:19</start><end>1/4/2012 12:41:14</end></task><task><title>bli blu</title><id>2</id><start>2/4/2012 11:14:29</start><end>2/4/2012 12:11:34</end></task><task><title>blu bli</title><id>3</id><start>3/4/2012 17:11:13</start><end>3/4/2012 18:19:31</end></task><task><title>ble ble</title><id>4</id><start>4/4/2012 19:14:29</start><end>4/4/2012 19:58:44</end></task></doc>
task:
'bla bla', '1', '1/4/2012 12:04:19', '1/4/2012 12:41:14'
'bli blu', '2', '2/4/2012 11:14:29', '2/4/2012 12:11:34'
'blu bli', '3', '3/4/2012 17:11:13', '3/4/2012 18:19:31'
'ble ble', '4', '4/4/2012 19:14:29', '4/4/2012 19:58:44'

**end

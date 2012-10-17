**
**requires:LIBXML2
**requires:SQLITE3
**requires:DISABLED
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><task><title>job 1</title><key>A123</key><customernumber>324</customernumber></task><task><title>job 2</title><key>V456</key><customernumber>567</customernumber></task><employee><firstname>Julia</firstname><surname>Tegel-Sacher</surname><phone>098 765 43 21</phone></employee><issuedate>13.5.2006</issuedate></assignment><assignment><task><title>job 3</title><key>A456</key><customernumber>567</customernumber></task><task><title>job 4</title><key>V789</key><customernumber>890</customernumber></task><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><issuedate>13.5.2006</issuedate></assignment></assignmentlist>**config
--input-filter xml:libxml2 --output-filter xml:libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2  --program simpleform.normalize --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../wolfilter/modules/database/sqlite3/mod_db_sqlite3test --database 'identifier=testdb,file=test.db,dumpfile=DBDUMP,inputfile=DBDATA' --program=DBPRG.tdl --directmap 'f=xml,c=test_transaction,n=run' run

**file:simpleform.normalize
int=number:integer;
uint=number:unsigned;
float=number:float;
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
STATEMENT get_task SELECT * FROM task WHERE id=$1;

TRANSACTION test_transaction
BEGIN
	INTO doc/task WITH //task DO get_task( id);
END
**outputfile:DBDUMP
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><task><id>11</id><task>bla bla</task><start>12:04:19 1/3/2012</start><end>12:41:34 1/3/2012</end></task><task><id>12</id><task>bli blu</task><start>07:14:23 1/3/2012</start><end>08:01:51 1/3/2012</end></task><task><id>21</id><task>gardening</task><start>09:24:28 1/3/2012</start><end>11:11:07 1/3/2012</end></task><task><id>22</id><task>helo</task><start>11:31:01 1/3/2012</start><end>12:07:55 1/3/2012</end></task><task><id>31</id><task>hula hop</task><start>19:14:38 1/4/2012</start><end>20:01:12 1/4/2012</end></task><task><id>32</id><task>hula hip</task><start>11:31:01 1/3/2012</start><end>12:07:55 1/3/2012</end></task><task><id>33</id><task>hula hup</task><start>11:31:01 1/3/2012</start><end>12:07:55 1/3/2012</end></task></doc>
**end

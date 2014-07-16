**
**requires:SQLITE3
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><task><title>job 1</title><id>1</id><customernumber>324</customernumber></task><task><title>job 2</title><id>2</id><customernumber>567</customernumber></task><employee><firstname>Julia</firstname><surname>Tegel-Sacher</surname><phone>098 765 43 21</phone></employee><issuedate>13.5.2006</issuedate></assignment><assignment><task><title>job 3</title><id>3</id><customernumber>567</customernumber></task><task><title>job 4</title><id>4</id><customernumber>890</customernumber></task><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><issuedate>13.5.2006</issuedate></assignment></assignmentlist>**config
--config wolframe.conf schema_select_task_by_id

**requires:TEXTWOLF
**requires:DISABLED WIN32
**file: schema_select_task_by_id.sfrm
STRUCT Employee
{
	firstname string
	surname string
	phone string
}

FORM schema_select_task_by_id
	-root assignmentlist
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
**file: test.dmap
COMMAND schema_select_task_by_id  CALL test_transaction  RETURN SKIP doc {standalone='yes', root = 'doc'};
**file:wolframe.conf
<?xml version="1.0"?>

<!-- Wolframe configuration file -->
<configuration>

	<!-- modules to be loaded by the server -->
	<LoadModules>
		<module>../wolfilter/modules/database/sqlite3/mod_db_sqlite3test</module>
		<module>../../src/modules/normalize/number/mod_normalize_number</module>
		<module>../../src/modules/cmdbind/tdl/mod_command_tdl</module>
		<module>../../src/modules/cmdbind/directmap/mod_command_directmap</module>
		<module>../../src/modules/audit/database/mod_audit_database</module>
		<module>../../src/modules/audit/textfile/mod_audit_textfile</module>
		<module>../../src/modules/doctype/xml/mod_doctype_xml</module>
		<module>../../src/modules/doctype/json/mod_doctype_json</module>
		<module>../../src/modules/filter/char/mod_filter_char</module>
		<module>../../src/modules/filter/line/mod_filter_line</module>
		<module>../../src/modules/filter/textwolf/mod_filter_textwolf</module>
		<module>../../src/modules/filter/token/mod_filter_token</module>
		<module>../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform</module>
		<module>../../src/modules/datatype/bcdnumber/mod_datatype_bcdnumber</module>
	</LoadModules>
	
	<ServerTokens>OS</ServerTokens>
	<ServerSignature>On</ServerSignature>

	<daemon>
		<user>nobody</user>
		<group>nobody</group>
		<pidFile>/tmp/wolframed.pid</pidFile>
	</daemon>

	<server>
		<maxConnections>3</maxConnections>
		<threads>7</threads>
		<listen>
			<address>*</address>
			<port>7661</port>
			<maxConnections>2</maxConnections>
		</listen>
		<listen>
			<address>localhost</address>
			<port>7662</port>
			<name>Interface 1</name>
		</listen>
	</server>
	<database>
		<SQLiteTest>
			<identifier>testdb</identifier>
			<file>test.db</file>
			<connections>3</connections>
			<dumpfile>DBDUMP</dumpfile>
			<inputfile>DBDATA</inputfile>
		</SQLiteTest>
	</database>
	<logging>
		<stderr>
			<level>INFO</level>
		</stderr>
		<logFile>
			<filename>/tmp/wolframed.log</filename>
			<level>NOTICE</level>
		</logFile>
		<syslog>
			<ident>wolframed</ident>
			<facility>LOCAL2</facility>
			<level>INFO</level>
		</syslog>
		<eventlog>
			<name>Application</name>
			<source>Wolframe</source>
			<level>INFO</level>
		</eventlog>
	</logging>
	<Processor>
	<program>DBPRG.tdl</program>
	<program>simpleform.wnmp</program>
	<program>schema_select_task_by_id.sfrm</program>
	<database>testdb</database>
	<cmdhandler>
		<directmap>
			<program>test.dmap</program>
			<filter>textwolf</filter>
		</directmap>
	</cmdhandler>
	</Processor>
</configuration>
**file:simpleform.wnmp
int=integer;
uint=unsigned;
float=floatingpoint;
currency=bigfxp(2);
percent_1=bigfxp(2);
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
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><task><title>bla bla</title><id>1</id><start>1/4/2012 12:04:19</start><end>1/4/2012 12:41:14</end></task><task><title>bli blu</title><id>2</id><start>2/4/2012 11:14:29</start><end>2/4/2012 12:11:34</end></task><task><title>blu bli</title><id>3</id><start>3/4/2012 17:11:13</start><end>3/4/2012 18:19:31</end></task><task><title>ble ble</title><id>4</id><start>4/4/2012 19:14:29</start><end>4/4/2012 19:58:44</end></task></doc>
task:
'bla bla', '1', '1/4/2012 12:04:19', '1/4/2012 12:41:14'
'bli blu', '2', '2/4/2012 11:14:29', '2/4/2012 12:11:34'
'blu bli', '3', '3/4/2012 17:11:13', '3/4/2012 18:19:31'
'ble ble', '4', '4/4/2012 19:14:29', '4/4/2012 19:58:44'

**end

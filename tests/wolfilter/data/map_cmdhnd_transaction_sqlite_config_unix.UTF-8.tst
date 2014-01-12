**
**requires:SQLITE3
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><task><title>job 1</title><id>1</id><customernumber>324</customernumber></task><task><title>job 2</title><id>2</id><customernumber>567</customernumber></task><employee><firstname>Julia</firstname><surname>Tegel-Sacher</surname><phone>098 765 43 21</phone></employee><issuedate>13.5.2006</issuedate></assignment><assignment><task><title>job 3</title><id>3</id><customernumber>567</customernumber></task><task><title>job 4</title><id>4</id><customernumber>890</customernumber></task><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><issuedate>13.5.2006</issuedate></assignment></assignmentlist>**config
--config wolframe.conf schema_select_task_by_id

**requires:TEXTWOLF
**requires:DISABLED WIN32
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
**file: test.dmap
COMMAND(schema_select_task_by_id) CALL(test_transaction) RETURN STANDALONE doc;
**file:wolframe.conf
;Wolframe configuration file

LoadModules	{						; modules to be loaded by the server
	module ../wolfilter/modules/database/sqlite3/mod_db_sqlite3test
	module ../../src/modules/normalize/number/mod_normalize_number
	module ../../src/modules/cmdbind/directmap/mod_command_directmap
	module ../../src/modules/audit/database/mod_audit_database
	module ../../src/modules/audit/textfile/mod_audit_textfile
	module ../../src/modules/authentication/textfile/mod_auth_textfile
	module ../../src/modules/filter/char/mod_filter_char
	module ../../src/modules/filter/line/mod_filter_line
	module ../../src/modules/filter/textwolf/mod_filter_textwolf
	module ../../src/modules/filter/token/mod_filter_token
	module ../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform
}

ServerTokens		OS					; print the application name and
								; version in the initial greeting
								; allowed values are:
								;   ProductOnly, Major, Minor,
								;   Revision, OS, None
ServerSignature		On					; print the name of the server as
								; first element of the greeting

; Unix daemon parameters. This section is not valid for windows
daemon {
	user		nobody
	group		nobody
	pidFile		/tmp/wolframed.pid
}

;; Windows service parameters (for service registration). This section is not valid on non-Windows OSes
;service {
;	serviceName	wolframe
;	displayName	"Wolframe Daemon"
;	description	"Wolframe Daemon"
;}


; Ports 7649-7671 and 7934-7966 are unassigned according to IANA (last updated 2010-03-11).
; Let's use 7660 for unencrypted connections and 7960 for SSL connections.
; Note that 7654 seems to be used by Winamp.
listen	{
	maxConnections	3					; Maximum number of simultaneous connections (clients).
								; Default is the OS limit. This is the global (total) value.
								; Set it to 0 to disable (to use the system limit)
	threads		7					; Number of threads for serving client connections.
								; Default 4
	socket	{
		address		*
		port		7661
		maxConnections	2				; Maximum number of simultaneous connections (clients)
								; for this socket. Default is the OS limit.
	}
	socket	{
		address		localhost
;		address		::1
		port		7662
		identifier	"Interface 1"			; Interfaces can be named for AAA purposes
	}

	SSLsocket	{
		address		0.0.0.0
		port		7961
		identifier	"Interface 1"			; Many interfaces can have the same identifier and
								; they can be referred as one group in AAAA
		; SSL parameters: required if SSLsocket is defined
		certificate	../SSL/wolframed.crt		; Server certificate file (PEM)
		key		../SSL/wolframed.key		; Server key file (PEM)
		CAdirectory	../SSL/CAdir			; Directory holding CA certificate files
		CAchainFile	../SSL/CAchain.pem		; SSL CA chain file
		verify		ON				; Require and verify client certificate
	}
	SSLsocket	{
		address		127.0.0.1
		port		7962
		identifier	"Interface 2"
		maxConnections	2
		certificate	../SSL/wolframed.crt
		key		../SSL/wolframed.key
		; CAdirectory	../SSL/CAdir			; It's not necessary to define both
		CAchainFile	../SSL/CAchain.pem		; CAdirectory and CAchainFile.
		verify		OFF
	}
}


; Database configuration
database	{
	SQLiteTest	{					; SQLite test database.
		identifier	testdb
		file		test.db
		connections	3				; number of parallel connections to the database
								; Default is 3
		dumpfile	DBDUMP
		inputfile	DBDATA
	}
}

; Logging configuration
logging	{
	; log level for console (stderr) logging
	stderr	{
		level	INFO
	}

	; log level for file logging
	logFile	{
		filename	/tmp/wolframed.log
		level		NOTICE
	}

	; syslog facility, identifier and log level
	syslog	{
		ident		wolframed
		facility	LOCAL2
		level		INFO
	}

	; eventlog source and log level. Valid only on Windows
	eventlog	{
		name		Wolframe
		source		wolframed
		level		INFO
	}
}

; Authentication, authorization, auditing and accounting configuration
AAAA	{
	authentication	{
		randomDevice	/dev/urandom
		allowAnonymous	yes
		database	{
			identifier	"PostgreSQL database authentication"
			database	pgdb
		}
		TextFile	{
			identifier	"Text file authentication"
			file		../var/tmp/wolframe.auth
		}
	}

	Authorization	{
		default		allow				; what to do when there is no
								; authorization information
	}

	Audit	{
		TextFile	{
			required	yes
			file		../var/tmp/wolframe.audit
		}
	}
}

; Data Processing Configuration
Processor
{
	; Programs to load
	program DBPRG.tdl
	program simpleform.wnmp
	program schema_select_task_by_id.sfrm
	database testdb
	cmdhandler
	{
		directmap
		{
			program test.dmap
			filter textwolf
		}
	}
}
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
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<doc><task><title>bla bla</title><id>1</id><start>1/4/2012 12:04:19</start><end>1/4/2012 12:41:14</end></task><task><title>bli blu</title><id>2</id><start>2/4/2012 11:14:29</start><end>2/4/2012 12:11:34</end></task><task><title>blu bli</title><id>3</id><start>3/4/2012 17:11:13</start><end>3/4/2012 18:19:31</end></task><task><title>ble ble</title><id>4</id><start>4/4/2012 19:14:29</start><end>4/4/2012 19:58:44</end></task></doc>
task:
'bla bla', '1', '1/4/2012 12:04:19', '1/4/2012 12:41:14'
'bli blu', '2', '2/4/2012 11:14:29', '2/4/2012 12:11:34'
'blu bli', '3', '3/4/2012 17:11:13', '3/4/2012 18:19:31'
'ble ble', '4', '4/4/2012 19:14:29', '4/4/2012 19:58:44'

**end

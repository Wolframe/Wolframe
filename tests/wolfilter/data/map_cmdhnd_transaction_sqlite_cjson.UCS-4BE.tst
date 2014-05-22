**
**requires:CJSON
**requires:TEXTWOLF
**requires:SQLITE3
**input
   {   
   	   "   a   s   s   i   g   n   m   e   n   t   "   :       [   
   	   	   {   
   	   	   	   "   t   a   s   k   "   :       [   
   	   	   	   	   {   
   	   	   	   	   	   "   t   i   t   l   e   "   :       "   j   o   b       1   "   ,   
   	   	   	   	   	   "   i   d   "   :       "   1   "   ,   
   	   	   	   	   	   "   c   u   s   t   o   m   e   r   n   u   m   b   e   r   "   :       "   3   2   4   "   
   	   	   	   	   }   ,   
   	   	   	   	   {   
   	   	   	   	   	   "   t   i   t   l   e   "   :       "   j   o   b       2   "   ,   
   	   	   	   	   	   "   i   d   "   :       "   2   "   ,   
   	   	   	   	   	   "   c   u   s   t   o   m   e   r   n   u   m   b   e   r   "   :       "   5   6   7   "   
   	   	   	   	   }   
   	   	   	   ]   ,   
   	   	   	   "   e   m   p   l   o   y   e   e   "   :       {   
   	   	   	   	   "   f   i   r   s   t   n   a   m   e   "   :       "   J   u   l   i   a   "   ,   
   	   	   	   	   "   s   u   r   n   a   m   e   "   :       "   T   e   g   e   l   -   S   a   c   h   e   r   "   ,   
   	   	   	   	   "   p   h   o   n   e   "   :       "   0   9   8       7   6   5       4   3       2   1   "   
   	   	   	   }   ,   
   	   	   	   "   i   s   s   u   e   d   a   t   e   "   :       "   1   3   .   5   .   2   0   0   6   "   
   	   	   }   ,   
   	   	   {   
   	   	   	   "   t   a   s   k   "   :       [   
   	   	   	   	   {   
   	   	   	   	   	   "   t   i   t   l   e   "   :       "   j   o   b       3   "   ,   
   	   	   	   	   	   "   i   d   "   :       "   3   "   ,   
   	   	   	   	   	   "   c   u   s   t   o   m   e   r   n   u   m   b   e   r   "   :       "   5   6   7   "   
   	   	   	   	   }   ,   
   	   	   	   	   {   
   	   	   	   	   	   "   t   i   t   l   e   "   :       "   j   o   b       4   "   ,   
   	   	   	   	   	   "   i   d   "   :       "   4   "   ,   
   	   	   	   	   	   "   c   u   s   t   o   m   e   r   n   u   m   b   e   r   "   :       "   8   9   0   "   
   	   	   	   	   }   
   	   	   	   ]   ,   
   	   	   	   "   e   m   p   l   o   y   e   e   "   :       {   
   	   	   	   	   "   f   i   r   s   t   n   a   m   e   "   :       "   J   a   k   o   b   "   ,   
   	   	   	   	   "   s   u   r   n   a   m   e   "   :       "   S   t   e   g   e   l   i   n   "   ,   
   	   	   	   	   "   p   h   o   n   e   "   :       "   0   1   2       3   4   5       6   7       8   9   "   
   	   	   	   }   ,   
   	   	   	   "   i   s   s   u   e   d   a   t   e   "   :       "   1   3   .   5   .   2   0   0   6   "   
   	   	   }   
   	   ]   
   }**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson -c wolframe.conf schema_select_task_by_id

**file:wolframe.conf
LoadModules
{
	module ./../wolfilter/modules/database/sqlite3/mod_db_sqlite3test
	module ./../../src/modules/normalize/number/mod_normalize_number
	module ./../../src/modules/normalize/string/mod_normalize_string
	module ./../../src/modules/cmdbind/directmap/mod_command_directmap
	module ./../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform
	module ./../../src/modules/datatype/bcdnumber/mod_datatype_bcdnumber
}
Database
{
	SQliteTest
	{
		identifier testdb
		file test.db
		dumpfile DBDUMP
		inputfile DBDATA
	}
}
Processor
{
	database testdb
	program ../wolfilter/template/program/schema_select_task_by_id_UTF16BE.tdl
	program ../wolfilter/scripts/schema_select_task_by_id.sfrm
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
**file: test.dmap
COMMAND schema_select_task_by_id CALL test_transaction RETURN STANDALONE doc;
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
   {   
   	   "   t   a   s   k   "   :   	   [   {   
   	   	   	   "   t   i   t   l   e   "   :   	   "   b   l   a       b   l   a   "   ,   
   	   	   	   "   i   d   "   :   	   "   1   "   ,   
   	   	   	   "   s   t   a   r   t   "   :   	   "   1   /   4   /   2   0   1   2       1   2   :   0   4   :   1   9   "   ,   
   	   	   	   "   e   n   d   "   :   	   "   1   /   4   /   2   0   1   2       1   2   :   4   1   :   1   4   "   
   	   	   }   ,       {   
   	   	   	   "   t   i   t   l   e   "   :   	   "   b   l   i       b   l   u   "   ,   
   	   	   	   "   i   d   "   :   	   "   2   "   ,   
   	   	   	   "   s   t   a   r   t   "   :   	   "   2   /   4   /   2   0   1   2       1   1   :   1   4   :   2   9   "   ,   
   	   	   	   "   e   n   d   "   :   	   "   2   /   4   /   2   0   1   2       1   2   :   1   1   :   3   4   "   
   	   	   }   ,       {   
   	   	   	   "   t   i   t   l   e   "   :   	   "   b   l   u       b   l   i   "   ,   
   	   	   	   "   i   d   "   :   	   "   3   "   ,   
   	   	   	   "   s   t   a   r   t   "   :   	   "   3   /   4   /   2   0   1   2       1   7   :   1   1   :   1   3   "   ,   
   	   	   	   "   e   n   d   "   :   	   "   3   /   4   /   2   0   1   2       1   8   :   1   9   :   3   1   "   
   	   	   }   ,       {   
   	   	   	   "   t   i   t   l   e   "   :   	   "   b   l   e       b   l   e   "   ,   
   	   	   	   "   i   d   "   :   	   "   4   "   ,   
   	   	   	   "   s   t   a   r   t   "   :   	   "   4   /   4   /   2   0   1   2       1   9   :   1   4   :   2   9   "   ,   
   	   	   	   "   e   n   d   "   :   	   "   4   /   4   /   2   0   1   2       1   9   :   5   8   :   4   4   "   
   	   	   }   ]   
   }   
task:
'bla bla', '1', '1/4/2012 12:04:19', '1/4/2012 12:41:14'
'bli blu', '2', '2/4/2012 11:14:29', '2/4/2012 12:11:34'
'blu bli', '3', '3/4/2012 17:11:13', '3/4/2012 18:19:31'
'ble ble', '4', '4/4/2012 19:14:29', '4/4/2012 19:58:44'

**end

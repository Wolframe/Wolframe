**
**requires:CJSON
**requires:TEXTWOLF
**requires:SQLITE3
**input
{
  "assignmentlist": {
    "assignment": [
      {
        "task": [
          {
            "title": "job 1",
            "id": "1",
            "customernumber": "324"
          },
          {
            "title": "job 2",
            "id": "2",
            "customernumber": "567"
          }
        ],
        "employee": {
          "firstname": "Julia",
          "surname": "Tegel-Sacher",
          "phone": "098 765 43 21"
        },
        "issuedate": "13.5.2006"
      },
      {
        "task": [
          {
            "title": "job 3",
            "id": "3",
            "customernumber": "567"
          },
          {
            "title": "job 4",
            "id": "4",
            "customernumber": "890"
          }
        ],
        "employee": {
          "firstname": "Jakob",
          "surname": "Stegelin",
          "phone": "012 345 67 89"
        },
        "issuedate": "13.5.2006"
      }
    ]
  }
}**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson  --program schema_select_task_by_id.sfrm --program simpleform.wnmp --module ../../src/modules/ddlcompiler//simpleform/mod_ddlcompiler_simpleform --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/normalize//string/mod_normalize_string --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../wolfilter/modules/database/sqlite3/mod_db_sqlite3test --database 'identifier=testdb,file=test.db,dumpfile=DBDUMP,inputfile=DBDATA' --program=DBPRG.tdl --cmdprogram=test.dmap schema_select_task_by_id

**file: test.dmap
COMMAND schema_select_task_by_id CALL test_transaction RETURN STANDALONE doc;
**file:simpleform.wnmp
int=integer;
uint=unsigned;
float=floatingpoint;
currency=fixedpoint(13,2);
percent_1=fixedpoint(5,1);
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
{
	"doc":	{
		"task":	[{
				"title":	"bla bla",
				"id":	"1",
				"start":	"1/4/2012 12:04:19",
				"end":	"1/4/2012 12:41:14"
			}, {
				"title":	"bli blu",
				"id":	"2",
				"start":	"2/4/2012 11:14:29",
				"end":	"2/4/2012 12:11:34"
			}, {
				"title":	"blu bli",
				"id":	"3",
				"start":	"3/4/2012 17:11:13",
				"end":	"3/4/2012 18:19:31"
			}, {
				"title":	"ble ble",
				"id":	"4",
				"start":	"4/4/2012 19:14:29",
				"end":	"4/4/2012 19:58:44"
			}]
	}
}
task:
'bla bla', '1', '1/4/2012 12:04:19', '1/4/2012 12:41:14'
'bli blu', '2', '2/4/2012 11:14:29', '2/4/2012 12:11:34'
'blu bli', '3', '3/4/2012 17:11:13', '3/4/2012 18:19:31'
'ble ble', '4', '4/4/2012 19:14:29', '4/4/2012 19:58:44'

**end

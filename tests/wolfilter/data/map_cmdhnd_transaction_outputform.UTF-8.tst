**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assignmentlist><assignment><task><title>job 1</title><key>A123</key><customernumber>324</customernumber></task><task><title>job 2</title><key>V456</key><customernumber>567</customernumber></task><employee><firstname>Julia</firstname><surname>Tegel-Sacher</surname><phone>098 765 43 21</phone></employee><issuedate>13.5.2006</issuedate></assignment><assignment><task><title>job 3</title><key>A456</key><customernumber>567</customernumber></task><task><title>job 4</title><key>V789</key><customernumber>890</customernumber></task><employee><firstname>Jakob</firstname><surname>Stegelin</surname><phone>012 345 67 89</phone></employee><issuedate>13.5.2006</issuedate></assignment></assignmentlist>**config
--input-filter xml:textwolf --output-filter xml:textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf  --program map_cmdhnd_transaction_outputform.sfrm --program employee_assignment_print.sfrm --module ../../src/modules/ddlcompiler//simpleform/mod_ddlcompiler_simpleform --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/normalize//string/mod_normalize_string --program simpleform.wnmp --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../wolfilter/modules/database/testtrace/mod_db_testtrace --database 'identifier=testdb,outfile=DBOUT,file=DBRES' --program=DBIN.tdl --cmdprogram=test.dmap run

**file: test.dmap
run = test_transaction( xml :employee_assignment_print) :map_cmdhnd_transaction_outputform;
**file:simpleform.wnmp
int=number:integer;
uint=number:unsigned;
float=number:float;
**file: DBRES
#id Task start end#11 'bla bla' '12:04:19 1/3/2012' '12:41:34 1/3/2012'#12 'bli blu' '07:14:23 1/3/2012' '08:01:51 1/3/2012'
#id task Start end#21 'gardening' '09:24:28 1/3/2012' '11:11:07 1/3/2012'#22 'helo' '11:31:01 1/3/2012' '12:07:55 1/3/2012'
#ID task start End#31 'hula hop' '19:14:38 1/4/2012' '20:01:12 1/4/2012'#32 'hula hip' '11:31:01 1/3/2012' '12:07:55 1/3/2012'#33 'hula hup' '11:31:01 1/3/2012' '12:07:55 1/3/2012'
**file:DBIN.tdl
TRANSACTION test_transaction
BEGIN
	INTO task FOREACH //task DO run( title);
END
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
**output
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE data SYSTEM "Data.simpleform"><data><person><company><name>Baluba Inc.</name></company><company><name>Carimba Inc.</name></company><company><name>Dereno Inc.</name></company><company><name>Huratz Inc.</name></company><id>1</id><child><prename>Beno</prename><surname>Beret</surname></child><child><prename>Carla</prename><surname>Carlson</surname></child><child><prename>Dorothe</prename><surname>Dubi</surname></child><child><prename>Hubert</prename><surname>Hauer</surname></child><prename>Aufru</prename><surname>Alano</surname><location><street>Butterweg 23</street><town>Bendorf</town></location><location><street>Camelstreet 34</street><town>Carassa</town></location><location><street>Demotastrasse 45</street><town>Durnfo</town></location><location><street>Hurtika 89</street><town>Hof</town></location><tag>1001</tag></person><person><company><name>Carimba Inc.</name></company><company><name>Dereno Inc.</name></company><company><name>Etungo Inc.</name></company><company><name>Huratz Inc.</name></company><id>2</id><child><prename>Carla</prename><surname>Carlson</surname></child><child><prename>Dorothe</prename><surname>Dubi</surname></child><child><prename>Erik</prename><surname>Ertki</surname></child><child><prename>Hubert</prename><surname>Hauer</surname></child><prename>Beno</prename><surname>Beret</surname><location><street>Camelstreet 34</street><town>Carassa</town></location><location><street>Demotastrasse 45</street><town>Durnfo</town></location><location><street>Erakimolstrasse 56</street><town>Enden</town></location><location><street>Hurtika 89</street><town>Hof</town></location><tag>1001</tag></person><person><company><name>Dereno Inc.</name></company><company><name>Etungo Inc.</name></company><company><name>Figaji Inc.</name></company><company><name>Huratz Inc.</name></company><id>3</id><child><prename>Dorothe</prename><surname>Dubi</surname></child><child><prename>Erik</prename><surname>Ertki</surname></child><child><prename>Fran</prename><surname>Fuioko</surname></child><child><prename>Hubert</prename><surname>Hauer</surname></child><prename>Carla</prename><surname>Carlson</surname><location><street>Demotastrasse 45</street><town>Durnfo</town></location><location><street>Erakimolstrasse 56</street><town>Enden</town></location><location><street>Fabelweg 67</street><town>Formkon</town></location><location><street>Hurtika 89</street><town>Hof</town></location><tag>1001</tag></person><person><company><name>Etungo Inc.</name></company><id>4</id><child><prename>Erik</prename><surname>Ertki</surname></child><prename>Dorothe</prename><surname>Dubi</surname><location><street>Erakimolstrasse 56</street><town>Enden</town></location><tag>1001</tag></person><person><company><name>Figaji Inc.</name></company><id>5</id><child><prename>Fran</prename><surname>Fuioko</surname></child><prename>Erik</prename><surname>Ertki</surname><location><street>Fabelweg 67</street><town>Formkon</town></location><tag>1001</tag></person><person><company/><id>6</id><child/><prename>Fran</prename><surname>Fuioko</surname><location/><tag>1001</tag></person><person><company><name>Huratz Inc.</name></company><id>7</id><child><prename>Hubert</prename><surname>Hauer</surname></child><prename>Gerd</prename><surname>Golto</surname><location><street>Hurtika 89</street><town>Hof</town></location><tag>1001</tag></person><person><company/><id>8</id><child/><prename>Hubert</prename><surname>Hauer</surname><location/><tag>1001</tag></person></data>
**end

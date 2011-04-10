///
/// Author: Matt Giger
/// http://www.superdeadly.com
/// Copyright (c) 2009 Super Deadly Software, LLC.
///
/// License:
///  Permission is hereby granted, free of charge, to any person obtaining a
///  copy of this software and associated documentation files (the
///  "Software"), to deal in the Software without restriction, including
///  without limitation the rights to use, copy, modify, merge, publish, dis-
///  tribute, sublicense, and/or sell copies of the Software, and to permit
///  persons to whom the Software is furnished to do so, subject to the fol-
///  lowing conditions:
///
///  The above copyright notice and this permission notice shall be included
///  in all copies or substantial portions of the Software.
///
///  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
///  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABIL-
///  ITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
///  SHALL THE AUTHOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
///  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
///  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
///  IN THE SOFTWARE.
///

Version 1.0 (05/14/2009)

The "sdsqlite" wrapper library encapsulates the most common uses of the SQLite
database API using C++ templates to greatly simplify syntax.

Adding sdsqlite to your project:
You must be able to compile C++ and link with the stdc++ library. Then just add
the sdsqlite.cpp file to the list of files to compile and link in and #include
the sdsqlite.h header file in any source file you need to use it in. If you are
using Objective-C, you must have the .mm extension on any file using sdsqlite.
You must also link in the SQLite 3 library of course.


Example 1: Simple database/table creation
=========================================
#include "sdsqlite.h"

void create_db(void)
{
	sd::sqlite database("mydb.db");
	database << "create table if not exists work (first_name text, last_name text, hours real)";
}



Example 2: Database insertion
=============================
#include "sdsqlite.h"

struct work_data { char* first; char* last; float hours; };

work_data wdata[] = {
	{"Joe", "Smith", 2.5},
	...
};

void insert_rows(void)
{
	try
	{
		sd::sqlite database("mydb.db");	// open the db with the table already created
		
		sd::sql insert_query(database);	// build an sql query
		insert_query << "insert into work (first_name, last_name, hours) VALUES(?, ?, ?)";
		
		database << "begin transaction";// create a transaction for speed
		
		// insert data (sdsqlite will auto-detect data type and execure query)
		for(int i=0;i<sizeof(wdata)/sizeof(work_data);++i)
			insert_query << wdata[i].first << wdata[i].last << wdata[i].hours;
			
		database << "commit transaction";// complete transaction
	}
	catch(sd::db_error& err)
	{
		// do something with error
	}
}


Example 3: Database extraction
==============================
#include "sdsqlite.h"

void extract_name(const std::string& name)
{
	try
	{
		sd::sqlite database("mydb.db");	// open the db with the table already created
		
		// select all names that begin with the contents of the "name" variable
		sd::sql select_query(database);
		select_query << "select first_name, last_name, hours from work where first_name like ?" << name + "%";
		
		// extract the matching rows
		float hours;
		std::string first, last;
		while(select_query.step())
		{
			select_query >>	first >> last >> hours;
			
			// do something with the data
		}	
	}
	catch(sd::db_error& err)
	{
		// do something with error
	}
}


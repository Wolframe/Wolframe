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

#include "sdsqlite.h"

#include <cassert>
#include <cstring>
#include <cstdlib>
#include <iostream>

namespace sd
{

//////////////////////////////////////////////////////////////////////
///
/// @class sqlite
///
/// Sqlite database instance.
///
//////////////////////////////////////////////////////////////////////

///
/// Empty constructor.
sqlite::sqlite(void) :
	db_(0), trans_count_(0)
{
}

///
/// @param path		Path to sqlite database file
///
/// Construct a database instance to the specified file.
sqlite::sqlite(const std::string& path) :
	db_(0), trans_count_(0)
{
	open(path);
}

///
/// Destructor closes current database connection.
sqlite::~sqlite(void)
{
	close();
	sqlite3_thread_cleanup();
}

///
/// @param path		Path to sqlite database file
///
/// Close any previous connection and open a new connection to the
/// sqlite database in the specified file.
void	sqlite::open(const std::string& path)
{
	const int	c_busy_timeout		= 5000;
	
	if(sqlite3_open(path.c_str(), &db_) != SQLITE_OK)
		throw db_error(sqlite3_errmsg(db_));
	if(!db_)
		throw db_error(std::string("can't open database: ") + path);
	
	sqlite3_busy_timeout(db_, c_busy_timeout);
}

///
/// Close the connection to the current database.
void	sqlite::close(void)
{
	if(db_)
		sqlite3_close(db_);
	db_ = 0;
}

///
/// Begin a database transaction.
void	sqlite::begin(void)
{
	if(++trans_count_ == 1)
		*this << "begin";
}

///
/// Commit the current database transaction.
void	sqlite::commit(void)
{
	if(--trans_count_ == 0)
		*this << "commit";
	else if(trans_count_ < 0)
		trans_count_ = 0;
}

///
/// Rollback the current transction on the database.
void	sqlite::rollback(void)
{
	trans_count_ = 0;
	*this << "rollback";
}

///
/// @param name		Table name to check for
/// @return			True if table name exists
///
/// Return whether a table with the specified name exists in the current database.
bool	sqlite::table_exists(const std::string& name)
{
	assert(db_);
	
	// check the master table first
	std::string sql = "select name from sqlite_master where type IN ('table','view') AND name=?";
	sqlite3_stmt* stmt;
	if(sqlite3_prepare(db_, sql.c_str(), sql.length(), &stmt, 0) != SQLITE_OK)
		throw db_error(sqlite3_errmsg(db_));
	
	sqlite3_bind_text(stmt, 1, name.c_str(), name.length(), SQLITE_TRANSIENT);
	
	int result = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	if(result == SQLITE_ROW)
		return true;
	
	// check the temp_master table last
	sql = "select name from sqlite_temp_master where type IN ('table','view') AND name=?";
	if(sqlite3_prepare(db_, sql.c_str(), sql.length(), &stmt, 0) != SQLITE_OK)
		throw db_error(sqlite3_errmsg(db_));
	
	sqlite3_bind_text(stmt, 1, name.c_str(), name.length(), SQLITE_TRANSIENT);
	result = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	return result == SQLITE_ROW;
}

///
/// @param stmt		Sql statement
/// @return			Reference to the current database
///
/// Operator shift function to simplify the execution of a sql statement.
sqlite&	sqlite::operator<<(const std::string& stmt)
{
	assert(db_);
	
	char* err_msg;
	if(sqlite3_exec(db_, stmt.c_str(), 0, 0, &err_msg) != SQLITE_OK)
		throw db_error(err_msg);
	return *this;
}



//////////////////////////////////////////////////////////////////////
///
/// @class sql
///
/// Sqlite prepared statement.
///
//////////////////////////////////////////////////////////////////////

///
/// @param db		Sqlite database
///
/// Construct a statement to be executed on the specified database.
sql::sql(sqlite& db) :
	db_(db), stmt_(0),
	ipos_(0), opos_(0), icol_(0), ocol_(0)
{
}

///
/// Destructor. Completes any pending uncompleted statement.
sql::~sql(void)
{
	if(icol_ > 0 && ipos_ == icol_)
		step();
	
	reset();
}

///
/// Reset the statement to be empty.
void	sql::reset(void)
{
	if(stmt_)
		sqlite3_finalize(stmt_);
	stmt_ = 0;
	ipos_ = opos_ = icol_ = ocol_ = 0;
}

///
/// @param stmt		An sql statement
///
/// Prepare the specified sql statement to be executed on the database.
void	sql::prepare(const std::string& stmt)
{
	reset();
	
	// prepare the statement
#if SQLITE_VERSION_NUMBER >= 3005000
	if(sqlite3_prepare_v2(db_.handle(), stmt.c_str(), static_cast<int>(stmt.length()), &stmt_, 0) != SQLITE_OK)
#else
	if(sqlite3_prepare(db_.handle(), stmt.c_str(), static_cast<int>(stmt.length()), &stmt_, 0) != SQLITE_OK)
#endif
		throw db_error(stmt + " " + sqlite3_errmsg(db_.handle()));
	
	// find out the number of columns to extract or bind
	ocol_ = sqlite3_column_count(stmt_);
	icol_ = sqlite3_bind_parameter_count(stmt_);
}

///
/// @return			True if there are more results pending from a select.
///
/// Step the prepared statement. This will make the values of a select available
/// or execute an insert or create or other general database statement.
bool	sql::step(void)
{
	ipos_ = opos_ = 0;
	int result = sqlite3_step(stmt_);
	if(result == SQLITE_ROW)
		return true;
	else if(result == SQLITE_DONE)
		reset();
	else
		throw db_error(sqlite3_errmsg(db_.handle()));
	return false;
}

///
/// @param	val		Statement to prepare or value to bind
/// @return			Reference to the statement
///
/// If the statement is not prepared, prepare val as the sql statement.
/// Otherwise bind the string value to the current input.
sql&	sql::operator<<(const char* val)
{
	if(!stmt_)			// first string added is the sql statement
		prepare(val);
	else
		sqlite3_bind_text(stmt_, ++ipos_, val, std::strlen(val), SQLITE_TRANSIENT);
	return *this;
}

///
/// @param	val		Statement to prepare or value to bind
/// @return			Reference to the statement
///
/// If the statement is not prepared, prepare val as the sql statement.
/// Otherwise bind the string value to the current input.
sql&	sql::operator<<(const std::string& val)
{
	if(!stmt_)			// first string added is the sql statement
		prepare(val);
	else
		sqlite3_bind_text(stmt_, ++ipos_, val.c_str(), static_cast<int>(val.length()), SQLITE_TRANSIENT);
	return *this;
}

///
/// @param	val		Value to bind
/// @return			Reference to the statement
///
/// Bind an input stream value to the current input.
sql&	sql::operator<<(std::istream& val)
{
	std::streamsize bytes = val.seekg(0, std::ios::end).tellg();
	val.seekg(0);
	void* buf = (bytes > 0) ? std::malloc(bytes) : 0;
	if(bytes > 0)
		val.read(static_cast<char*>(buf), bytes);
	sqlite3_bind_blob(stmt_, ++ipos_, buf, bytes, (bytes > 0) ? std::free : 0);
	return *this;
}

///
/// @param	val		Reference to variable to have value returned in
/// @return			Reference to the statement
///
/// Extract a value from the current result row.
sql&	sql::operator>>(std::string& val)
{
	const char* v = reinterpret_cast<const char*>(sqlite3_column_text(stmt_, opos_++));
	if(v)
		val.assign(v);
	return *this;
}

///
/// @param	val		Reference to variable to have value returned in
/// @return			Reference to the statement
///
/// Extract a stream from the current result row.
sql&	sql::operator>>(std::ostream& val)
{
	int bytes = sqlite3_column_bytes(stmt_, opos_);
	if(bytes > 0)
		val.write(static_cast<const char*>(sqlite3_column_blob(stmt_, opos_)), bytes);
	++opos_;
	return *this;
}



}

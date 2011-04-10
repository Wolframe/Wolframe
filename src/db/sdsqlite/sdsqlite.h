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

#ifndef _sdsqlite_
#define _sdsqlite_

#include <string>
#include <istream>
#include <ostream>
#include <sqlite3.h>

namespace sd
{

//////////////////////////////////////////////////////////////////////
///
/// @class sqlite
///
/// Sqlite database instance.
///
//////////////////////////////////////////////////////////////////////
struct sqlite
{
	///
	/// Empty constructor.
	sqlite(void);
	
	///
	/// @param path		Path to sqlite database file
	///
	/// Construct a database instance to the specified file.
	sqlite(const std::string& path);
	
	///
	/// Destructor closes current database connection.
	~sqlite(void);
	
	///
	/// @return			Sqlite handle to database
	///
	/// Return the internal sqlite handle to the database.
	sqlite3*	handle(void)				{ return db_; }
	
	///
	/// @return			True if the database is open on a valid instance
	///
	/// Is this database available for querying.
	bool		is_open(void)				{ return db_ != 0; }
	
	///
	/// @return			Integer value of the last "ROWID" for the previous insert statement
	///
	/// Sometimes it is convenient to have the internal sqlite ROWID for the previous insert
	/// statement so you can perform other operations on the just inserted row.
	int			last_rowid(void)			{ return static_cast<int>(sqlite3_last_insert_rowid(db_)); }
	
	///
	/// @param path		Path to sqlite database file
	///
	/// Close any previous connection and open a new connection to the
	/// sqlite database in the specified file.
	void		open(const std::string& path);
	
	///
	/// Close the connection to the current database.
	void		close(void);
	
	///
	/// Begin a database transaction.
	void		begin(void);
	
	///
	/// Commit the current database transaction.
	void		commit(void);
	
	///
	/// Rollback the current transction on the database.
	void		rollback(void);
	
	///
	/// @param name		Table name to check for
	/// @return			True if table name exists
	///
	/// Return whether a table with the specified name exists in the current database.
	bool		table_exists(const std::string& name);
	
	///
	/// @param stmt		Sql statement
	/// @return			Reference to the current database
	///
	/// Operator shift function to simplify the execution of a sql statement.
	sqlite&		operator<<(const std::string& stmt);
	
	
	std::string	path_;				///< Filesystem path to database
private:
	sqlite3*	db_;				///< Sqlite database handle
	int			trans_count_;		///< Reference count to handle nested transactions
};



//////////////////////////////////////////////////////////////////////
///
/// @class sql
///
/// Sqlite prepared statement.
///
//////////////////////////////////////////////////////////////////////
struct sql
{
	struct end {};			///< Utility class to mark the completion of a statement.
	struct null {};			///< Utility class to mark a null value.
	
	///
	/// @param db		Sqlite database
	///
	/// Construct a statement to be executed on the specified database.
	sql(sqlite& db);
	
	///
	/// Destructor. Completes any pending uncompleted statement.
	~sql(void);
	
	///
	/// @return			Number of input parameters required by statement.
	inline int	in_params(void) const			{ return icol_; }
	
	///
	/// @return			Number of output parameters supplied by statement.
	inline int	out_params(void) const			{ return ocol_; }
	
	///
	/// @param idx		Zero based index into select statement results.
	/// @return			Type of result expected.
	///
	/// Return the type of result expected for the specified column.
	inline int	col_type(int idx)				{ return sqlite3_column_type(stmt_, idx); }
	
	///
	/// Reset the statement to be empty.
	void		reset(void);
	
	///
	/// @param stmt		An sql statement
	///
	/// Prepare the specified sql statement to be executed on the database.
	void		prepare(const std::string& stmt);
	
	///
	/// @return			True if there are more results pending from a select.
	///
	/// Step the prepared statement. This will make the values of a select available
	/// or execute an insert or create or other general database statement.
	bool		step(void);
	
	///
	/// @return			Reference to the statement
	///
	/// This terminates the statement by calling step(), which is most useful for
	/// insert statements as a useful shorthand.
	inline sql&	operator<<(const end&)				{ step(); return *this; }
	
	///
	/// @return			Reference to the statement
	///
	/// Bind a null value to the current input.
	inline sql&	operator<<(const null&)				{ sqlite3_bind_null(stmt_, ++ipos_); return *this; }
	
	///
	/// @param	val		Value to bind
	/// @return			Reference to the statement
	///
	/// Bind a value to the current input.
	inline sql&	operator<<(bool val)				{ sqlite3_bind_int(stmt_, ++ipos_, val); return *this; }
	
	///
	/// @param	val		Value to bind
	/// @return			Reference to the statement
	///
	/// Bind a value to the current input.
	inline sql&	operator<<(char val)				{ sqlite3_bind_int(stmt_, ++ipos_, val); return *this; }
	
	///
	/// @param	val		Value to bind
	/// @return			Reference to the statement
	///
	/// Bind a value to the current input.
	inline sql&	operator<<(unsigned char val)		{ sqlite3_bind_int(stmt_, ++ipos_, val); return *this; }
	
	///
	/// @param	val		Value to bind
	/// @return			Reference to the statement
	///
	/// Bind a value to the current input.
	inline sql&	operator<<(short val)				{ sqlite3_bind_int(stmt_, ++ipos_, val); return *this; }
	
	///
	/// @param	val		Value to bind
	/// @return			Reference to the statement
	///
	/// Bind a value to the current input.
	inline sql&	operator<<(unsigned short val)		{ sqlite3_bind_int(stmt_, ++ipos_, val); return *this; }
	
	///
	/// @param	val		Value to bind
	/// @return			Reference to the statement
	///
	/// Bind a value to the current input.
	inline sql&	operator<<(int val)					{ sqlite3_bind_int(stmt_, ++ipos_, val); return *this; }
	
	///
	/// @param	val		Value to bind
	/// @return			Reference to the statement
	///
	/// Bind a value to the current input.
	inline sql&	operator<<(unsigned int val)		{ sqlite3_bind_int(stmt_, ++ipos_, val); return *this; }
	
	///
	/// @param	val		Value to bind
	/// @return			Reference to the statement
	///
	/// Bind a value to the current input.
	inline sql&	operator<<(long val)				{ sqlite3_bind_int(stmt_, ++ipos_, val); return *this; }
	
	///
	/// @param	val		Value to bind
	/// @return			Reference to the statement
	///
	/// Bind a value to the current input.
	inline sql&	operator<<(unsigned long val)		{ sqlite3_bind_int(stmt_, ++ipos_, val); return *this; }
	
	///
	/// @param	val		Value to bind
	/// @return			Reference to the statement
	///
	/// Bind a value to the current input.
	inline sql&	operator<<(long long val)			{ sqlite3_bind_int64(stmt_, ++ipos_, val); return *this; }
	
	///
	/// @param	val		Value to bind
	/// @return			Reference to the statement
	///
	/// Bind a value to the current input.
	inline sql&	operator<<(unsigned long long val)	{ sqlite3_bind_int64(stmt_, ++ipos_, val); return *this; }
	
	///
	/// @param	val		Value to bind
	/// @return			Reference to the statement
	///
	/// Bind a value to the current input.
	inline sql&	operator<<(float val)				{ sqlite3_bind_double(stmt_, ++ipos_, val); return *this; }
	
	///
	/// @param	val		Value to bind
	/// @return			Reference to the statement
	///
	/// Bind a value to the current input.
	inline sql&	operator<<(double val)				{ sqlite3_bind_double(stmt_, ++ipos_, val); return *this; }
	
	///
	/// @param	val		Statement to prepare or value to bind
	/// @return			Reference to the statement
	///
	/// If the statement is not prepared, prepare val as the sql statement.
	/// Otherwise bind the string value to the current input.
	sql&		operator<<(const char* val);
	
	///
	/// @param	val		Statement to prepare or value to bind
	/// @return			Reference to the statement
	///
	/// If the statement is not prepared, prepare val as the sql statement.
	/// Otherwise bind the string value to the current input.
	sql&		operator<<(const std::string& val);
	
	///
	/// @param	val		Value to bind
	/// @return			Reference to the statement
	///
	/// Bind an input stream value to the current input.
	sql&		operator<<(std::istream& val);
	
	///
	/// @param	val		Reference to variable to have value returned in
	/// @return			Reference to the statement
	///
	/// Extract a value from the current result row.
	inline sql&	operator>>(bool& val)				{ val = sqlite3_column_int(stmt_, opos_++) != 0; return *this; }
	
	///
	/// @param	val		Reference to variable to have value returned in
	/// @return			Reference to the statement
	///
	/// Extract a value from the current result row.
	inline sql&	operator>>(char& val)				{ val = sqlite3_column_int(stmt_, opos_++); return *this; }
	
	///
	/// @param	val		Reference to variable to have value returned in
	/// @return			Reference to the statement
	///
	/// Extract a value from the current result row.
	inline sql&	operator>>(unsigned char& val)		{ val = sqlite3_column_int(stmt_, opos_++); return *this; }
	
	///
	/// @param	val		Reference to variable to have value returned in
	/// @return			Reference to the statement
	///
	/// Extract a value from the current result row.
	inline sql&	operator>>(short& val)				{ val = sqlite3_column_int(stmt_, opos_++); return *this; }
	
	///
	/// @param	val		Reference to variable to have value returned in
	/// @return			Reference to the statement
	///
	/// Extract a value from the current result row.
	inline sql&	operator>>(unsigned short& val)		{ val = sqlite3_column_int(stmt_, opos_++); return *this; }
	
	///
	/// @param	val		Reference to variable to have value returned in
	/// @return			Reference to the statement
	///
	/// Extract a value from the current result row.
	inline sql&	operator>>(int& val)				{ val = sqlite3_column_int(stmt_, opos_++); return *this; }
	
	///
	/// @param	val		Reference to variable to have value returned in
	/// @return			Reference to the statement
	///
	/// Extract a value from the current result row.
	inline sql&	operator>>(unsigned int& val)		{ val = sqlite3_column_int(stmt_, opos_++); return *this; }
	
	///
	/// @param	val		Reference to variable to have value returned in
	/// @return			Reference to the statement
	///
	/// Extract a value from the current result row.
	inline sql&	operator>>(long& val)				{ val = sqlite3_column_int(stmt_, opos_++); return *this; }
	
	///
	/// @param	val		Reference to variable to have value returned in
	/// @return			Reference to the statement
	///
	/// Extract a value from the current result row.
	inline sql&	operator>>(unsigned long& val)		{ val = sqlite3_column_int(stmt_, opos_++); return *this; }
	
	///
	/// @param	val		Reference to variable to have value returned in
	/// @return			Reference to the statement
	///
	/// Extract a value from the current result row.
	inline sql&	operator>>(long long& val)			{ val = sqlite3_column_int64(stmt_, opos_++); return *this; }
	
	///
	/// @param	val		Reference to variable to have value returned in
	/// @return			Reference to the statement
	///
	/// Extract a value from the current result row.
	inline sql&	operator>>(unsigned long long& val)	{ val = sqlite3_column_int64(stmt_, opos_++); return *this; }
	
	///
	/// @param	val		Reference to variable to have value returned in
	/// @return			Reference to the statement
	///
	/// Extract a value from the current result row.
	inline sql&	operator>>(float& val)				{ val = sqlite3_column_double(stmt_, opos_++); return *this; }
	
	///
	/// @param	val		Reference to variable to have value returned in
	/// @return			Reference to the statement
	///
	/// Extract a value from the current result row.
	inline sql&	operator>>(double& val)				{ val = sqlite3_column_double(stmt_, opos_++); return *this; }
	
	///
	/// @param	val		Reference to variable to have value returned in
	/// @return			Reference to the statement
	///
	/// Extract a value from the current result row.
	sql&		operator>>(std::string& val);
	
	///
	/// @param	val		Reference to variable to have value returned in
	/// @return			Reference to the statement
	///
	/// Extract a stream from the current result row.
	sql&		operator>>(std::ostream& val);
	
private:
	///
	/// Private copy constructor to protect from statement duplication.
	sql(const sql&);
	
	///
	/// Private copy operator to protect from statement duplication.
	sql& operator=(const sql&);
	
	sqlite&			db_;		///< Reference to the database connection
	sqlite3_stmt*	stmt_;		///< Sqlite statement handle
	int				ipos_;		///< Current insert input position
	int				opos_;		///< Current select output position
	int				icol_;		///< Number of insert columns expected
	int				ocol_;		///< Number of select columns expected
};

//////////////////////////////////////////////////////////////////////
///
/// @class db_error
///
/// Database exception class.
///
//////////////////////////////////////////////////////////////////////
struct db_error : std::exception
{
	///
	/// @param what		Error string
	///
	/// Database exception constructor.
	db_error(const std::string& _what) :
		what_(_what) {}
	
	///
	/// Virtual destructor.
	~db_error(void) throw() {}
	
	///
	/// @return		Exception description string.
	const char* what() const throw()	{ return what_.c_str(); }
	
	std::string	what_;		///< Exception description string
};


}

#endif

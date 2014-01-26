/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\brief Interface to substitute parameters in embedded SQL statements
///\file database/PostgreSQLstatement.hpp
#ifndef _POSTGRESQL_STATEMENT_HPP_INCLUDED
#define _POSTGRESQL_STATEMENT_HPP_INCLUDED
#include "database/baseStatement.hpp"
#include <string>
#include "types/variant.hpp"
#include <libpq-fe.h>

namespace _Wolframe {
namespace db {

class PostgreSQLstatement : public BaseStatement
{
public:
	enum {MaxNofParam=99};

	PostgreSQLstatement();
	PostgreSQLstatement( const PostgreSQLstatement& o);

	//\brief Executes the statement with the bound parameters on connection 'conn'
	PGresult* execute( PGconn *conn) const;

	virtual void clear( );

	virtual void bind( const unsigned int idx, const types::Variant &value );

private:
	//\remark See implementation of pq_sendint64
	void bindUInt64( boost::uint64_t value, const char* type="uint8");
	void bindInt64( boost::int64_t value);
	void bindUInt32( boost::uint32_t value, const char* type="uint4");
	void bindInt32( boost::int32_t value);
	void bindUInt16( boost::uint16_t value, const char* type="uint2");
	void bindInt16( boost::int16_t value);
	void bindByte( boost::uint8_t value, const char* type="uint1");
	void bindByte( boost::int8_t value);
	void bindBool( bool value);
	void bindDouble( double value);
	void bindString( const char* value, std::size_t size);
	void bindNull();

	void setNextParam( const void* ptr, unsigned int size, const char* type);

	struct Params
	{
		const char* paramar[MaxNofParam];
		int paramarsize;
	};
	void getParams( Params& params) const;

private:
	int m_paramofs[ MaxNofParam];
	const char* m_paramtype[ MaxNofParam];
	int m_paramlen[ MaxNofParam];
	int m_parambinary[ MaxNofParam];
	int m_paramarsize;
	std::string m_buf;
};


}}//namespace
#endif


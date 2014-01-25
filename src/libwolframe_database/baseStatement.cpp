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
///\brief Implements substitution of parameters in embedded SQL statements
///\file baseStatement.cpp

#include "database/baseStatement.hpp"
#include <boost/lexical_cast.hpp>
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::db;

BaseStatement::BaseStatement( )
	: m_maxParam( 0 )
{
}

BaseStatement::BaseStatement( const std::string &stmtStr )
	: m_stmtStr( stmtStr ),
	m_maxParam( 0 )
{
	parse( );
}

void BaseStatement::init( const std::string &stmtStr )
{
	clear( );
	m_stmtStr = stmtStr;
	parse( );
}

void BaseStatement::parse( )
{
	// TODO
}

void BaseStatement::clear( )
{
	m_stmtStr.clear( );
	m_maxParam = 0;
}

void BaseStatement::bind( unsigned int idx, const types::Variant & /*value*/ )
{
	if( idx < 1 || idx > m_maxParam ) {
		throw new std::runtime_error(
			"index of bind parameter is out of range (" +
			boost::lexical_cast<std::string>( idx ) +
			" required to be in range 1.." +
			boost::lexical_cast<std::string>( m_maxParam ) +
			" in statement '" + /* or nativeSQL? */ originalSQL( ) + "'"
		);
	}
}

const std::string BaseStatement::originalSQL( ) const
{
	return m_stmtStr;
}

//~ void Statement::clear()
//~ {
	//~ m_string.clear();
	//~ m_data.clear();
	//~ m_bind.clear();
//~ }
//~ 
//~ void BaseStatement::init( const std::string& stmstr)
//~ {
	//~ clear();
	//~ m_string = stmstr;
	//~ std::string::const_iterator si = stmstr.begin(), se = stmstr.end();
	//~ std::string::const_iterator chunkstart = si;
	//~ for (; si != se; ++si)
	//~ {
		//~ if (*si == '$')
		//~ {
			//~ if (si > chunkstart)
			//~ {
				//~ m_data.push_back( Element( 0, std::string( chunkstart, si)));
			//~ }
			//~ std::string idxstr;
			//~ for (++si; si != se && *si >= '0' && *si <= '9'; ++si)
			//~ {
				//~ idxstr.push_back( *si);
			//~ }
			//~ chunkstart = si;
			//~ if (idxstr.empty()) throw std::runtime_error( "only parameters referenced by index supported until now in database statements");
			//~ unsigned int idx = boost::lexical_cast<unsigned int>( idxstr);
			//~ if (idx == 0) throw std::runtime_error( "parameter index out of range");
			//~ if (idx > m_maxparam) m_maxparam = idx;
			//~ m_data.push_back( Element( idx, ""));
			//~ if (si == se) break;
		//~ }
	//~ }
	//~ if (si > chunkstart)
	//~ {
		//~ m_data.push_back( Element( 0, std::string( chunkstart, si)));
	//~ }

// from PostgresqlStatement:

	//~ std::string rt;
	//~ std::string::const_iterator si = m_stmstr.begin(), se = m_stmstr.end();
	//~ std::string::const_iterator chunkstart = si;
//~ 
	//~ for (; si != se; ++si)
	//~ {
		//~ if (*si == '\'' || *si == '\"')
		//~ {
			//~ // ignore contents in string:
			//~ char eb = *si;
			//~ for (++si; si != se && *si != eb; ++si)
			//~ {
				//~ if (*si == '\\')
				//~ {
					//~ ++si;
					//~ if (si == se) break;
				//~ }
			//~ }
			//~ if (si == se) throw std::runtime_error( "string not terminated in statement");
		//~ }
		//~ if (*si == '$')
		//~ {
			//~ if (si > chunkstart)
			//~ {
				//~ rt.append( chunkstart, si);
				//~ chunkstart = si;
			//~ }
			//~ int idx = 0;
			//~ for (++si; si != se && *si >= '0' && *si <= '9'; ++si)
			//~ {
				//~ idx *= 10;
				//~ idx += (*si - '0');
				//~ if (idx > MaxNofParam) throw std::runtime_error( "parameter index out of range");
			//~ }
			//~ if (si != se)
			//~ {
				//~ if ((*si|32) >= 'a' && (*si|32) <= 'z') throw std::runtime_error( "illegal parameter index (immediately followed by identifier)");
				//~ if (*si == '_') throw std::runtime_error( "illegal parameter index (immediately followed by underscore)");
			//~ }
			//~ if (idx == 0 || idx > m_paramarsize) throw std::runtime_error( "parameter index out of range");
			//~ if (m_paramtype[ idx-1])
			//~ {
				//~ rt.append( "$");
				//~ rt.append( chunkstart, si);
				//~ if (m_paramtype[ idx-1][0])
				//~ {
					//~ rt.append( "::");
					//~ rt.append( m_paramtype[ idx-1]);
				//~ }
			//~ }
			//~ else
			//~ {
				//~ rt.append( "NULL");
			//~ }
			//~ chunkstart = si;
			//~ if (si == se) break;
		//~ }
	//~ }
	//~ if (si > chunkstart)
	//~ {
		//~ rt.append( chunkstart, si);
	//~ }
	//~ return rt;

//~ }
//~ 
//~ Statement::Statement( const std::string& stmstr)
	//~ :m_maxparam(0)
//~ {
	//~ init( stmstr);
//~ }
//~ 
//~ void Statement::bind( unsigned int idx, const std::string& arg)
//~ {
	//~ m_bind[ idx] = arg;
//~ }
//~ 
//~ // will be nativeSQL
//~ std::string Statement::expanded() const
//~ {
	//~ std::string rt;
	//~ std::vector<Element>::const_iterator di = m_data.begin(), de = m_data.end();
	//~ for (; di != de; di++)
	//~ {
		//~ if (di->first)
		//~ {
			//~ std::map<unsigned int, std::string>::const_iterator bi = m_bind.find( di->first);
			//~ if (bi == m_bind.end()) throw std::runtime_error( std::string( "parameter $") + boost::lexical_cast<std::string>(di->first) + " undefined");
			//~ rt.append( bi->second);
		//~ }
		//~ else
		//~ {
			//~ rt.append( di->second);
		//~ }
	//~ }
	//~ return rt;
//~ }
//~ 
//~ 
//~ 

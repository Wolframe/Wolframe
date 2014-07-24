/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
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
/// \file database/substitutingStatement.hpp
/// \brief Interface to an abstract database statement class

#ifndef _DATABASE_SUBSTITUTING_STATEMENT_HPP_INCLUDED
#define _DATABASE_SUBSTITUTING_STATEMENT_HPP_INCLUDED

#include "database/baseStatement.hpp"

#include <map>

namespace _Wolframe {
namespace db {

/// \class SubstitutingStatement
/// \brief An abstract statement class which allows derived classes to
///       provide a simple escaping function which encodes the data
///       which is stored directly in the SQL statement
/// \file database/substitutingStatement.hpp
/// \remark A generic statement class which takes the string and replaces
///        the placeholders direcly with the data (more as prove of concept).
///        To be useful a derived class has at least to implement the 'convert'
///        method
class SubstitutingStatement : public BaseStatement
{
	public:
		SubstitutingStatement( );
		SubstitutingStatement( const SubstitutingStatement &o );
		
		virtual void init( const std::string &stmtStr );

		virtual void clear( );

		virtual void bind( const unsigned int idx, const types::Variant &value );
	
	protected:
		/// \brief Function to convert and escape a variant, if single
		///       quotes are need, the function should also return those,
		///       care must be taken to use the database escape functions
		///       to escape the values here (beware of SQL code injection!)
		virtual const std::string convert( const types::Variant &value ) const = 0;

		virtual const std::string replace( const unsigned int idx ) const;
	
	private:
		/// \brief Stores the placeholders in calls of bind till they are used
		///       to create the substituted SQL statement with the embedded data.
		std::map< unsigned int, std::string > m_bind;
};

}}//namespace
#endif


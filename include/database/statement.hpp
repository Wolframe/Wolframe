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

///\file database/statement.hpp

#ifndef _DATABASE_STATEMENET_HPP_INCLUDED
#define _DATABASE_STATEMENET_HPP_INCLUDED

#include <string>
#include "types/variant.hpp"

namespace _Wolframe {
namespace db {

///\class Statement
///\brief The interface of a statement SQL parameter substutution class
class Statement
{
	public:
		virtual ~Statement( ) { }
		
		///\brief Binds parameter at position idx with variant data
		///       in the format with Wolframe placeholders
		virtual void bind( const unsigned int idx, const types::Variant &arg ) = 0;

		///\brief Returns the SQL statement as passed down to the
		///       database layer
		virtual const std::string originalSQL( ) const = 0;
		
		///\brief Returns the SQL statement with the native database
		///       placeholders or the data filled in (this is up
		///       to the derived classes).
		virtual const std::string nativeSQL( ) const = 0;

		///\brief Set new SQL statement
		virtual void init( const std::string &stmtStr ) = 0;
		
		///\brief Clear current statement
		virtual void clear( ) = 0;

		///\brief Trigger substitution (nativeSQL is valid after
		///       this call and not before!)
		///\param[in] checkForMissingPlaceholders whether the function should
		///           already check the validity and cardinality of the placeholders
		///           in the SQL statement
		virtual void substitute( bool checkForMissingPlaceholders = true ) = 0;

		///\brief Funtion called when the placeholder should be put
		///       into the final string (this is either for subsituting
		///       data or rewrite placeholders to native syntax)
		virtual const std::string replace( const unsigned int idx ) const = 0;
};

}}//namespace
#endif


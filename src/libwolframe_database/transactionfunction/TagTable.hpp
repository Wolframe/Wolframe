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
///\brief Internal interface for input data structure of transaction function
///\file transactionfunction/TagTable.hpp
#ifndef _DATABASE_TRANSACTION_FUNCTION_TAG_TABLE_HPP_INCLUDED
#define _DATABASE_TRANSACTION_FUNCTION_TAG_TABLE_HPP_INCLUDED
#include <map>
#include <string>

namespace _Wolframe {
namespace db {

class TagTable
{
public:
	explicit TagTable( bool case_sensitive_);
	TagTable( const TagTable& o);

	///\brief Get the index of a tag
	///\return index of the tag or 0, if not found
	int find( const char* tag, std::size_t tagsize) const;
	///\brief Get the index of a tag
	///\return index of the tag or 0, if not found
	int find( const std::string& tagstr) const;
	///\brief Get the index of a tag. Create it if not found
	///\return index of the tag
	int get( const std::string& tagstr);
	///\brief Get the index of a tag. Create it if not found
	///\return index of the tag
	int get( const char* tag, std::size_t tagsize);

	///\brief Get the name of the tag as inserted as first
	const char* getstr( int tg) const	{return (tg<=0)?0:m_strings.c_str()+tg;}
	///\brief Find out if the map is case sensitive in search
	///\return true if yes
	bool case_sensitive() const		{return m_case_sensitive;}

	///\brief Insert a tag table into another and return the map for rewriting references to tag indices from the old to the new tag map
	std::map<int,int> insert( const TagTable& o);
	///\brief Get the special tag index used for an unused value
	int unused() const			{return 1;}

private:
	bool m_case_sensitive;
	int m_size;
	std::map< std::string, int> m_map;
	std::string m_strings;
};

}}//namespace
#endif


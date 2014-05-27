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
/// \file utils/sourceLineInfo.hpp
/// \brief Defines a data structure to hold positional info in a source string for error messsages etc.

#ifndef _WOLFRAME_SOURCE_LINE_INFO_HPP_INCLUDED
#define _WOLFRAME_SOURCE_LINE_INFO_HPP_INCLUDED
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace utils {

/// \class SourceLineInfo
/// \brief Line information for error messages for parser using STL string iterators as source scanners
class SourceLineInfo
{
public:
	/// \brief Default constructor
	SourceLineInfo()
		:m_line(1),m_column(1){}
	/// \brief Constructor
	SourceLineInfo( unsigned int line_, unsigned int column_)
		:m_line(line_),m_column(column_){}
	/// \brief Copy constructor
	SourceLineInfo( const SourceLineInfo& o)
		:m_line(o.m_line),m_column(o.m_column){}

	/// \brief Get the current line
	unsigned int line() const	{return m_line;}
	/// \brief Get the current column
	unsigned int column() const	{return m_column;}

	/// \brief Increment the current line
	void incrementLine()	{m_column=1; ++m_line;}
	/// \brief Increment the current column
	void incrementColumn()	{++m_column;}

	/// \brief Update this line info by scanning the source parsed from 'lastpos' to 'pos', counting lines and characters inbetween
	void update( const std::string::const_iterator& lastpos, const std::string::const_iterator& pos);

private:
	unsigned int m_line;		///< the current line
	unsigned int m_column;		///< the current column
};

/// \brief Get the source line info by scanning the source parsed from 'start' to 'pos', counting lines and characters inbetween
SourceLineInfo getSourceLineInfo( const std::string::const_iterator& start, const std::string::const_iterator& pos);

}}
#endif



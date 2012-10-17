/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
///\brief Templates for parsing with positional error messages
///\file config/programBase.hpp
//
#ifndef _CONFIG_PROGRAM_HPP_INCLUDED
#define _CONFIG_PROGRAM_HPP_INCLUDED
#include "utils/miscUtils.hpp"
#include <string>
#include <cstdlib>
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
#include <boost/detail/select_type.hpp>

namespace _Wolframe {
namespace config {

struct LineInfo
{
	LineInfo( const LineInfo& o)
		:line(o.line),col(o.col){}
	LineInfo( const std::string::const_iterator& start, const std::string::const_iterator& pos)
	{
		std::pair<unsigned int,unsigned int> info = utils::getLineInfo( start, pos);
		line = info.first;
		col = info.second;
	}

	unsigned int line;
	unsigned int col;
};


struct PositionalErrorException :public std::runtime_error
{
	PositionalErrorException( const LineInfo& pos, const std::string& msg)
		:std::runtime_error( std::string( "error on line ") + boost::lexical_cast<std::string>(pos.line) + " column " + boost::lexical_cast<std::string>(pos.col) + ":" + msg){}
	PositionalErrorException( const std::string& filename, const PositionalErrorException& e)
		:std::runtime_error( std::string( "error in file '") + filename + "' " + (e.what() + std::strlen("error "))){}
};

class PositionalErrorMessageBase
{
public:
	PositionalErrorMessageBase( const std::string& source)
		:m_start( source.begin()){}

	template <typename MessageObject>
	typename boost::enable_if_c<
		boost::is_convertible<MessageObject,std::string>::value
		,PositionalErrorException>::type
	operator()( const std::string::const_iterator& pos_, const MessageObject& msgobj)
	{
		return PositionalErrorException( LineInfo( m_start, pos_), std::string(msgobj));
	}

	template <typename MessageObject>
	typename boost::enable_if_c<
		!boost::is_convertible<MessageObject,std::string>::value
		,PositionalErrorException>::type
	operator()( const std::string::const_iterator& pos_, const MessageObject& msgobj)
	{
		return PositionalErrorException( LineInfo( m_start, pos_), boost::lexical_cast<std::string>(msgobj));
	}

	struct Message :public std::ostringstream
	{
		Message(){}
		operator std::string()
		{
			return std::ostringstream::str();
		}
	};
private:
	std::string::const_iterator m_start;
};


}} //namespace
#endif


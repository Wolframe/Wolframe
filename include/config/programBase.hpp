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
///\brief Templates for parsing with positional error messages
///\file config/programBase.hpp
//
#ifndef _CONFIG_PROGRAM_BASE_HPP_INCLUDED
#define _CONFIG_PROGRAM_BASE_HPP_INCLUDED
#include "utils/parseUtils.hpp"
#include "logger/logObject.hpp"
#include <string>
#include <cstdlib>
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
#include <boost/detail/select_type.hpp>

namespace _Wolframe {
namespace config {

//\class LineInfo
//\brief Info for line/coulumn position
struct LineInfo
	:public utils::LineInfo
{
	LineInfo( const LineInfo& o)
		:utils::LineInfo(o){}
	LineInfo( const std::string::const_iterator& start, const std::string::const_iterator& pos)
		:utils::LineInfo( utils::getLineInfo( start, pos))
	{}

	static const log::LogObjectDescriptionBase* getLogObjectDescription()
	{
		struct Description :public log::LogObjectDescription<utils::LineInfo>
		{
			Description():log::LogObjectDescription<utils::LineInfo>( "on line $1 column $2"){(*this)(&utils::LineInfo::line)(&utils::LineInfo::column);}
		};
		static Description rt;
		return &rt;
	}
};

//\class PositionalFileError
//\brief Error class with message and source position info
struct PositionalError
{
	PositionalError( const PositionalError& o)
		:line(o.line),column(o.column),msg(o.msg){}
	PositionalError( const LineInfo& o, const std::string& msg_)
		:line(o.line),column(o.column),msg(msg_){}

	unsigned int line;
	unsigned int column;
	std::string msg;

	static const log::LogObjectDescriptionBase* getLogObjectDescription()
	{
		struct Description :public log::LogObjectDescription<PositionalError>
		{
			Description():log::LogObjectDescription<PositionalError>( "error on line $1 column $2: $3"){(*this)(&PositionalError::line)(&PositionalError::column)(&PositionalError::msg);}
		};
		static Description rt;
		return &rt;
	}
};

//\class PositionalFileError
//\brief Error class with message and source file and position info
struct PositionalFileError
{
	PositionalFileError( const PositionalFileError& o)
		:line(o.line),column(o.column),msg(o.msg),filename(o.filename){}
	PositionalFileError( const PositionalError& o, const std::string& filename_)
		:line(o.line),column(o.column),msg(o.msg),filename(filename_){}
	PositionalFileError( const LineInfo& o, const std::string& msg_, const std::string& filename_)
		:line(o.line),column(o.column),msg(msg_),filename(filename_){}

	unsigned int line;
	unsigned int column;
	std::string msg;
	std::string filename;

	static const log::LogObjectDescriptionBase* getLogObjectDescription()
	{
		struct Description :public log::LogObjectDescription<PositionalFileError>
		{
			Description():log::LogObjectDescription<PositionalFileError>( "error in file $4 on line $1 column $2: $3"){(*this)(&PositionalFileError::line)(&PositionalFileError::column)(&PositionalFileError::msg)(&PositionalFileError::filename);}
		};
		static Description rt;
		return &rt;
	}
};

//\class PositionalErrorException
//\brief Base class for an error exception including source position info
struct PositionalErrorException
	:public log::Exception<std::runtime_error,PositionalError>
{
	PositionalErrorException( const PositionalError& err_)
		:log::Exception<std::runtime_error,PositionalError>(err_){}
};

//\class PositionalFileErrorException
//\brief Base class for an error exception including source file and position info
struct PositionalFileErrorException
	:public log::Exception<std::runtime_error,PositionalFileError>
{
	PositionalFileErrorException( const PositionalFileError& err_)
		:log::Exception<std::runtime_error,PositionalFileError>(err_){}
};

//\class PositionalErrorMessageBase
//\brief Base class to build up a positional error exception with its error message
class PositionalErrorMessageBase
{
public:
	PositionalErrorMessageBase( const std::string& source)
		:m_start( source.begin()){}

	struct Message :public std::string
	{
		Message(){}
		Message& operator << (const std::string& arg)
		{
			append( arg);
			return *this;
		}
		template <typename T>
		typename boost::enable_if_c<
			boost::is_arithmetic<T>::value
			,Message&>::type operator << (const T& arg)
		{
			append( boost::lexical_cast<std::string>( arg));
			return *this;
		}
	};

	PositionalErrorException operator()( const std::string::const_iterator& pos_, const std::string& msgobj) const
	{
		PositionalError err( LineInfo( m_start, pos_), msgobj);
		return PositionalErrorException( err);
	}

	PositionalError getError( const std::string::const_iterator& pos_, const std::string& msgobj) const
	{
		return PositionalError( LineInfo( m_start, pos_), msgobj);
	}
private:
	std::string::const_iterator m_start;
};


}} //namespace
#endif


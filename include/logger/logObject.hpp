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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file logger/logObject.hpp
///\brief Provides uniform handling of logger and exception objects
#ifndef _Wolframe_LOGGER_OBJECT_HPP_INCLUDED
#define _Wolframe_LOGGER_OBJECT_HPP_INCLUDED
#include <stdexcept>
#include <string>
#include <vector>
#include <boost/concept_check.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
#include <boost/detail/select_type.hpp>
#include <boost/type_traits/function_traits.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/function.hpp>

namespace _Wolframe {
namespace log {

class LogObjectDescriptionBase
{
public:
	LogObjectDescriptionBase( const std::string& formatstring);

	LogObjectDescriptionBase( const LogObjectDescriptionBase& o)
		:m_format(o.m_format),m_strings(o.m_strings){}

	typedef std::string (*ToStringMethod)( const void* elem);

	void substitute( std::size_t idx, ToStringMethod tostring, std::size_t ofs);
	void checkUnresolved();

	std::string objToString( const void* obj) const;

private:
	struct FormatElement
	{
		enum Type {String,Reference,Index};
		Type type;
		std::size_t index;
		ToStringMethod tostring;

		FormatElement() :type(String),index(0),tostring(0){}
		FormatElement( std::size_t index_) :type(Index),index(index_),tostring(0){}
		FormatElement( const FormatElement& o) :type(o.type),index(o.index),tostring(o.tostring){}
	};
	std::vector<FormatElement> m_format;
	std::string m_strings;
};


template <class LogObject>
class LogObjectDescription
	:public LogObjectDescriptionBase
{
public:
	LogObjectDescription( const std::string& formatstring)
		:LogObjectDescriptionBase( formatstring),m_idx(0){}

	LogObjectDescription( const LogObjectDescription& o)
		:LogObjectDescriptionBase(o),m_idx(o.m_idx){}

	template <typename Element>
	LogObjectDescription& operator()( Element LogObject::*eptr)
	{
		struct Method
		{
			static std::string exec( const void* elem)
			{
				return boost::lexical_cast<std::string>( *(Element*)(elem));
			}
		};
		substitute( ++m_idx, &Method::exec, (std::size_t)&(((LogObject*)0)->*eptr));
		return *this;
	}
private:
	std::size_t m_idx;
};


struct LogObject
{
	static const LogObjectDescriptionBase* getLogObjectDescription(){return 0;}
};

template<typename T>
struct has_getLogObjectDescription_method
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<const LogObjectDescriptionBase* (*)()> struct tester_static_signature;

	template<typename U>
	static small_type has_matching_member(tester_static_signature<&U::getLogObjectDescription>*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};


struct LogString
{
	template <typename T>
	static typename boost::enable_if_c<
		has_getLogObjectDescription_method<T>::value
		,std::string>::type
	get( const T& t)
	{
		return boost::lexical_cast<std::string>(t);
	}
	template <typename T>
	static typename boost::enable_if_c<
		boost::has_left_shift<std::ostream,T>::value
		,std::string>::type
	get( const T& t)
	{
		std::ostringstream rt;
		rt << t;
		return rt.str();
	}
};

template <class StdExceptionClass, class LogObjectT>
struct Exception
	:public LogObjectT
	,public StdExceptionClass
{
public:
	virtual ~Exception() throw (){}

	template <typename A1>
	Exception( const A1& a1)
		:LogObjectT(a1)
		,StdExceptionClass( LogObjectT::getLogObjectDescription()->objToString(static_cast<LogObjectT*>(this)))
		{}

	template <typename A1, typename A2>
	Exception( const A1& a1, const A2& a2)
		:LogObjectT(a1,a2)
		,StdExceptionClass( LogObjectT::getLogObjectDescription()->objToString(static_cast<LogObjectT*>(this)))
		{}

	template <typename A1, typename A2, typename A3>
	Exception( const A1& a1, const A2& a2, const A3& a3)
		:LogObjectT(a1,a2,a3)
		,StdExceptionClass( LogObjectT::getLogObjectDescription()->objToString(static_cast<LogObjectT*>(this)))
		{}

	template <typename A1, typename A2, typename A3, typename A4>
	Exception( const A1& a1, const A2& a2, const A3& a3, const A4& a4)
		:LogObjectT(a1,a2,a3,a4)
		,StdExceptionClass( LogObjectT::getLogObjectDescription()->objToString(static_cast<LogObjectT*>(this)))
		{}
};



}} //namespace
#endif


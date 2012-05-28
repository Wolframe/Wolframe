/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
///\file serialize/mapContext.hpp
///\brief Defines the error handling of serialization/deserialization functions

#ifndef _Wolframe_SERIALIZE_STRUCT_MAPCONTEXT_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_MAPCONTEXT_HPP_INCLUDED
#include <string>
#include "filter/inputfilter.hpp"
#include "filter/outputfilter.hpp"

namespace _Wolframe {
namespace serialize {

struct Context
{
	Context();
	~Context(){}

	const char* getLastError() const		{return m_lasterror[0]?m_lasterror:0;}
	void append( const char* c, std::size_t n)	{m_content.append( c,n);}
	void clear();
	void followTagConsumed( bool v)			{m_followTagConsumed=v;}
	bool followTagConsumed()			{return m_followTagConsumed;}
	const std::string& content() const		{return m_content;}
	std::string& content()				{return m_content;}

	void setTag( const char* tag);
	void setError( const char* msg, const char* msgparam=0);
	bool printElem( langbind::OutputFilter::ElementType tp, const void* elem, std::size_t elemsize, langbind::OutputFilter& out);
private:
	char m_lasterror[ 256];
	std::string m_content;
	bool m_followTagConsumed;

	void setMsg( const char* m1, char dd, const char* m2, const char* m3=0);
};

}}
#endif


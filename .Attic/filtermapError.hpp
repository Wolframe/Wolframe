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
///\file serialize/struct/filtermapBase.hpp
///\brief Defines the non intrusive base class of serialization for the direct map

#ifndef _Wolframe_SERIALIZE_STRUCT_FILTERMAP_ERROR_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_FILTERMAP_ERROR_HPP_INCLUDED
#include "protocol/inputfilter.hpp"
#include "protocol/formatoutput.hpp"
#include "logger.hpp"
#include <cstddef>
#include <string>
#include <vector>

namespace _Wolframe {
namespace serialize {

class Context
{
public:
	enum {bufsize=4096,errbufsize=256};

	Context()
		:m_endtagConsumed(false)
		,m_buf(new char[bufsize+errbufsize])
	{
		m_lasterror = m_buf+errbufsize;
	}

	~Context()
	{
		delete [] m_buf;
	}

	const char* getLastError() const {return m_lasterror;}
	const std::string content() const {return m_content;}
	void endTagConsumed( bool v)	{m_endtagConsumed=v;}
	bool endTagConsumed()		{return m_endtagConsumed;}
	char* buf() const		{return m_buf;}
private:
	friend class DescriptionBase;
	char* m_lasterror;
	std::string m_content;
	bool m_endtagConsumed;
	char* m_buf;
};

}}//namespace
#endif


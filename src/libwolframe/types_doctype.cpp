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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file types_doctype.cpp
///\brief ADT for XML document type definition handling
#include "types/doctype.hpp"
#include "utils/miscUtils.hpp"
#include <cstring>
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::types;

static bool skip( std::string::const_iterator& cc, const std::string::const_iterator& ee)
{
	for (;cc != ee && *cc <= ' ' && *cc >= 0; ++cc);
	if (cc == ee) return false;
	return true;
}

static char getbrk( std::string::const_iterator& cc)
{
	if (*cc == '"' || *cc == '\'')
	{
		return *cc++;
	}
	else
	{
		return ' ';
	}
}

static char* nextid( char*& pp, std::string::const_iterator& cc, const std::string::const_iterator& ee)
{
	char* baseptr = pp;
	if (!skip( cc, ee)) return 0;
	char eb = getbrk( cc);

	for (;;)
	{
		if (ee == cc && eb != ' ') throw std::runtime_error( "incomplete document type definition");
		if (ee == cc || *cc == eb)
		{
			if (ee != cc) ++cc;
			*pp++ = '\0';
			return baseptr;
		}
		else
		{
			*pp++ = *cc++;
		}
	}
}

DocType::DocType()
	:rootid(0)
	,publicid(0)
	,systemid(0)
{}

DocType::DocType( const std::string& value)
	:rootid(0)
	,publicid(0)
	,systemid(0)
	,mem(std::malloc( value.size()+1), std::free)
{
	char* ptr = (char*)mem.get();
	char* pp = ptr;
	if (!ptr) throw std::bad_alloc();

	std::string::const_iterator cc=value.begin(), ee=value.end();

	rootid = nextid( pp, cc, ee);
	if (!rootid) throw std::runtime_error( "empty document type definition");
	systemid = nextid( pp, cc, ee);
	if (systemid && std::strcmp( systemid, "SYSTEM") == 0)
	{
		// ... tolerant parsing of XML identifiers in the DOCTYPE declaration
		systemid = nextid( pp, cc, ee);
		if (!systemid) throw std::runtime_error( "missing elements in document type definition");
	}
	else if (systemid && std::strcmp( systemid, "PUBLIC") == 0)
	{
		// ... tolerant parsing of XML identifiers in the DOCTYPE declaration
		publicid = nextid( pp, cc, ee);
		systemid = nextid( pp, cc, ee);
		if (!publicid || !systemid) throw std::runtime_error( "missing elements in document type definition");
	}
	else if (systemid)
	{
		char* tt = nextid( pp, cc, ee);
		if (tt)
		{
			publicid = systemid;
			systemid = tt;
		}
	}
	if (skip( cc, ee)) throw std::runtime_error( "too many elements in document type definition");
}

static void appendstr( std::string& rt, const char* str)
{
	if (std::strchr( str, '"'))
	{
		if (std::strchr( str, '\'')) throw std::runtime_error("illegal doctype definition (quotes)");
		rt.append( " \'");
		rt.append( str);
		rt.append( "\'");
	}
	else
	{
		rt.append( " \"");
		rt.append( str);
		rt.append( "\"");
	}
}

std::string DocType::tostring() const
{
	std::string rt;
	if (rootid)
	{
		rt.append( rootid);
		if (publicid)
		{
			rt.append( " PUBLIC ");
			appendstr( rt, publicid);
			appendstr( rt, systemid?systemid:"");
		}
		else if (systemid)
		{
			rt.append( " SYSTEM ");
			appendstr( rt, systemid);
		}
	}
	return rt;
}

std::string _Wolframe::types::getIdFromDoctype( const std::string& doctype)
{
	std::string rootid;
	std::string publicid;
	std::string systemid;
	std::string::const_iterator itr=doctype.begin(), end=doctype.end();

	if (utils::parseNextToken( rootid, itr, end))
	{
		if (utils::parseNextToken( publicid, itr, end))
		{
			if (publicid == "PUBLIC")
			{
				if (!utils::parseNextToken( publicid, itr, end)
				||  !utils::parseNextToken( systemid, itr, end))
				{
					throw std::runtime_error( "illegal doctype definition (public id)");
				}
				return utils::getFileStem( systemid);
			}
			else if (publicid == "SYSTEM")
			{
				if (!utils::parseNextToken( systemid, itr, end))
				{
					throw std::runtime_error( "illegal doctype definition (system id)");
				}
				return utils::getFileStem( systemid);
			}
			else if (utils::parseNextToken( systemid, itr, end))
			{
				if (itr != end)
				{
					throw std::runtime_error( "illegal doctype definition (not terminated)");
				}
				return utils::getFileStem( systemid);
			}
			else
			{
				return utils::getFileStem( publicid);
			}
		}
		else
		{
			return rootid;
		}
	}
	else
	{
		return "";
	}
}


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
#include "utils/parseUtils.hpp"
#include "utils/fileUtils.hpp"
#include <cstring>
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::types;

static bool skip( std::string::const_iterator& cc, const std::string::const_iterator& ee)
{
	for (;cc != ee && (unsigned char)*cc <= 32; ++cc);
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

static std::string nextid( std::string::const_iterator& cc, const std::string::const_iterator& ee)
{
	std::string rt;
	if (!skip( cc, ee)) return rt;
	char eb = getbrk( cc);

	for (;;)
	{
		if (cc == ee && eb != ' ') throw std::runtime_error( "incomplete document type definition");
		if (cc == ee || *cc == eb)
		{
			if (ee != cc) ++cc;
			return rt;
		}
		else
		{
			rt.push_back( *cc++);
		}
	}
}

DocType::DocType()
{}

DocType::DocType( const std::string& value)
{
	std::string::const_iterator cc=value.begin(), ee=value.end();

	rootid = nextid( cc, ee);
	if (rootid.empty()) throw std::runtime_error( "empty document type definition");
	systemid = nextid( cc, ee);
	if (!systemid.empty())
	{
		if (systemid == "SYSTEM")
		{
			// ... tolerant parsing of XML identifiers in the DOCTYPE declaration
			systemid = nextid( cc, ee);
			if (systemid.empty()) throw std::runtime_error( "missing elements in document type definition");
		}
		else if (systemid == "PUBLIC")
		{
			// ... tolerant parsing of XML identifiers in the DOCTYPE declaration
			publicid = nextid( cc, ee);
			systemid = nextid( cc, ee);
			if (systemid.empty() || publicid.empty()) throw std::runtime_error( "missing or empty elements in document type definition");
		}
		else
		{
			std::string tt = nextid( cc, ee);
			if (!tt.empty())
			{
				publicid = systemid;
				systemid = tt;
			}
		}
	}
	if (skip( cc, ee)) throw std::runtime_error( "too many elements in document type definition");
}

static void appendstr( std::string& rt, const std::string& str)
{
	if (std::strchr( str.c_str(), '"'))
	{
		if (std::strchr( str.c_str(), '\'')) throw std::runtime_error("illegal doctype definition (quotes)");
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
	if (!rootid.empty())
	{
		rt.append( rootid);
		if (!publicid.empty())
		{
			rt.append( " PUBLIC ");
			appendstr( rt, publicid);
			appendstr( rt, systemid);
		}
		else if (!systemid.empty())
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


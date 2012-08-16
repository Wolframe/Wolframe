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
///\brief Implementation of the processor of database commands
///\file src/database_processor.cpp
#include "database/processor.hpp"
#include "textwolf/xmlscanner.hpp"
#include "textwolf/cstringiterator.hpp"
#include "textwolf/charset.hpp"
#include <cstring>
#include <limits>
#include <sstream>
#include <iostream>
#include <locale>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe::db;

static std::string normalizeTagName( const std::string& tagname)
{
	std::string rt;
	static textwolf::XMLScannerBase::IsTagCharMap isTagCharMap;
	textwolf::CStringIterator itr( tagname.c_str(), tagname.size());
	typedef textwolf::XMLScanner<textwolf::CStringIterator,textwolf::charset::UTF8,textwolf::charset::UTF8,std::string> T;
	if (!T::parseStaticToken( isTagCharMap, itr, rt))
	{
		throw std::runtime_error( "illegal tag name");
	}
	return rt;
}

int TagTable::find( const char* tag, std::size_t tagsize) const
{
	const std::string tagnam( tag, tagsize);
	return find( tagnam);
}

int TagTable::find( const std::string& tagnam) const
{
	std::map< std::string, int>::const_iterator ii = m_map.find( tagnam);
	if (ii == m_map.end()) return 0;
	return ii->second;
}

int TagTable::get( const std::string& tagnam)
{
	std::map< std::string, int>::const_iterator ii = m_map.find( tagnam);
	if (ii == m_map.end())
	{
		m_map[ tagnam] = ++m_size;
		return m_size;
	}
	else
	{
		return ii->second;
	}
}

int TagTable::get( const char* tag, std::size_t tagsize)
{
	const std::string tagstr( tag, tagsize);
	return get( tagstr);
}

int TagTable::unused() const
{
	return m_size +1;
}

Structure::Structure( const TagTable* tagmap)
	:m_tagmap(tagmap)
{
	m_nodemem.alloc( 1);
	m_strmem.alloc( 1);
	m_data.push_back( std::vector<Node>());
}

Structure::Structure( const Structure& o)
	:m_nodemem(o.m_nodemem)
	,m_strmem(o.m_strmem)
	,m_data(o.m_data)
	,m_tagmap(o.m_tagmap){}

void Structure::openTag( const char* tag, std::size_t tagsize)
{
	const std::string tagstr( tag, tagsize);
	int mi = -(int)m_tagmap->find( tagstr);
	if (mi == 0) mi = -(int)m_tagmap->unused();

	m_data.back().push_back( Node( mi, 0));
	m_data.push_back( std::vector<Node>());
}

void Structure::closeTag()
{
	if (m_data.back().size())
	{
		std::size_t mi = m_nodemem.alloc( m_data.back().size());
		Node* nd = m_nodemem.base() + mi;
		std::vector<Node>::const_iterator itr = m_data.back().begin(), end = m_data.back().end();
		for (; itr != end; ++itr, ++nd)
		{
			*nd = *itr;
		}
		m_data.pop_back();
		m_data.back().back().second = -(int)mi;
	}
	else
	{
		m_data.pop_back();
	}
}

void Structure::pushValue( const char* val, std::size_t valsize)
{
	std::size_t mi = m_strmem.alloc( valsize+1);
	if (mi > (std::size_t)std::numeric_limits<int>::max()) throw std::bad_alloc();
	char* nd = m_strmem.base() + mi;
	std::memcpy( nd, val, (valsize+1)*sizeof(char));
	if (m_data.size() < 2 || m_data.back().size() > 0 || m_data[ m_data.size()-2].back().second != 0)
	{
		throw std::logic_error( "tag stack inconsintent");
	}
	m_data[ m_data.size()-2].back().second = (int)mi;
}

Path::Path( const std::string& pt, TagTable* tagmap)
{
	Element elem;
	std::string tagnam;
	std::string::const_iterator ii = pt.begin(), ee = pt.end();
	while (ii != ee)
	{
		if (*ii == '/')
		{
			++ii;
			if (*ii == '/')
			{
				elem.m_type = Find;
				++ii;
			}
			else
			{
				elem.m_type = Next;
			}
		}
		else
		{
			elem.m_type = Next;
			tagnam.clear();
			while (ii != ee && *ii != '/')
			{
				tagnam.push_back( *ii);
				++ii;
			}
			if (tagnam.empty())
			{
				throw std::runtime_error( std::string("empty tag in path '") + pt + "'" );
			}
			else if (tagnam == "..")
			{
				if (elem.m_type == Find)
				{
					throw std::runtime_error( std::string("selection '//..' is illegal in path '") + pt + "'" );
				}
				elem.m_type = Up;
				elem.m_id = 0;
			}
			else if (tagnam == ".")
			{
				if (elem.m_type == Find)
				{
					throw std::runtime_error( std::string("selection '//.' is illegal in path '") + pt + "'" );
				}
				elem.m_type = Current;
				elem.m_id = 0;
			}
			else if (tagnam == "*")
			{
				elem.m_id = 0;
			}
			else
			{
				tagnam = normalizeTagName( tagnam);
				elem.m_id = tagmap->get( tagnam);
			}
			m_path.push_back( elem);
		}
	}
}

Path::Path( const Path& o)
	:m_path(o.m_path){}

std::string Path::tostring() const
{
	std::vector<Element>::const_iterator ii = m_path.begin(), ee = m_path.end();
	std::ostringstream rt;
	for (; ii != ee; ++ii)
	{
		rt << elementTypeName( ii->m_type) << " " << ii->m_id << std::endl;
	}
	return rt.str();
}


FunctionCall::FunctionCall( const std::string& name, const std::string& selector, const std::vector<std::string>& arg)
	:m_name(name),m_selector(selector,&m_tagmap)
{
	std::vector<std::string>::const_iterator ii = arg.begin(), ee = arg.end();
	for (; ii != ee; ++ii)
	{
		Path pp( *ii, &m_tagmap);
		m_arg.push_back( pp);
	}
}

FunctionCall::FunctionCall( const FunctionCall& o)
	:m_tagmap(o.m_tagmap)
	,m_name(o.m_name)
	,m_selector(o.m_selector)
	,m_arg(o.m_arg) {}

TransactionProgram::TransactionProgram( const TransactionProgram& o)
	:m_call(o.m_call){}

TransactionProgram::TransactionProgram( const std::string& src)
{
	std::string::const_iterator ii = src.begin(), ee = src.end();
	for (; ii != ee; ++ii)
	{
		while (ii < ee && *ii > 0 && *ii < 32) ++ii;
		if (ii == ee) throw std::runtime_error( "unexpected end of expression");
		std::string functionname;
		while (ii < ee && std::isalnum( *ii))
		{
			functionname.push_back( *ii);
			++ii;
		}
		if (ii == ee) throw std::runtime_error( "unexpected end of expression");
		while (ii < ee && *ii > 0 && *ii < 32) ++ii;
		if (ii == ee) throw std::runtime_error( "unexpected end of expression");
		if (*ii != '(') throw std::runtime_error( "syntax error in expression '(' expected");
		++ii;
		std::string selector;
		while (ii < ee && *ii > 0 && *ii < 32) ++ii;
		if (ii == ee) throw std::runtime_error( "unexpected end of expression");
		while (ii < ee && *ii != ':')
		{
			selector.push_back( *ii);
			++ii;
		}
		if (ii == ee) throw std::runtime_error( "unexpected end of expression");
		while (ii < ee && *ii > 0 && *ii < 32) ++ii;
		if (ii == ee) throw std::runtime_error( "unexpected end of expression");
		std::vector<std::string> param;

		for (;;)
		{
			std::string pp;
			while (ii < ee && *ii > 0 && *ii < 32) ++ii;
			if (ii == ee) throw std::runtime_error( "unexpected end of expression");
			while (ii < ee && *ii != ',' && *ii != ')')
			{
				pp.push_back( *ii);
				++ii;
			}
			boost::trim( pp);
			if (ii == ee) throw std::runtime_error( "unexpected end of expression");
			param.push_back( pp);
			if (*ii == ')')
			{
				++ii;
				while (ii < ee && *ii > 0 && *ii < 32) ++ii;
				break;
			}
			else if (*ii == ',')
			{
				++ii;
				continue;
			}
		}
		FunctionCall cc( functionname, selector, param);
		m_call.push_back( cc);
		while (ii < ee && *ii > 0 && *ii < 32) ++ii;
		if (ii == ee) break;
		if (*ii != ';') throw std::runtime_error( "missing semicolon as expression separator");
	}
}



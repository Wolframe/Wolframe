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
#include "filter/token_filter.hpp"
#include "textwolf/xmlscanner.hpp"
#include "textwolf/cstringiterator.hpp"
#include "textwolf/charset.hpp"
#include <cstring>
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
	,m_rootidx(0)
{
	m_nodemem.alloc( 1);
	m_strmem.alloc( 1);
	m_data.push_back( std::vector<Node>());
}

Structure::Structure( const Structure& o)
	:m_nodemem(o.m_nodemem)
	,m_strmem(o.m_strmem)
	,m_data(o.m_data)
	,m_tagmap(o.m_tagmap)
	,m_rootidx(o.m_rootidx)
	{}

void Structure::openTag( const char* tag, std::size_t tagsize)
{
	const std::string tagstr( tag, tagsize);
	openTag( tagstr);
}

void Structure::openTag( const std::string& tagstr)
{
	int mi = -(int)m_tagmap->find( tagstr);
	if (mi == 0) mi = -(int)m_tagmap->unused();

	m_data.back().push_back( Node( 0, mi, 0, 0));
	m_data.push_back( std::vector<Node>());
}

void Structure::setParentLinks( std::size_t mi)
{
	Node* nd = &m_nodemem[mi];
	std::size_t ii = 0, nn = nd->nofchild(), ci = nd->childidx();
	for (; ii<nn; ++ii)
	{
		Node* cd = &m_nodemem[ ci + ii];
		cd->m_parent = mi;
		setParentLinks( ci);
	}
}

void Structure::closeTag()
{
	if (m_data.back().size())
	{
		std::size_t ni = m_data.back().size();
		std::size_t mi = m_nodemem.alloc( ni);
		Node* nd = &m_nodemem[mi];
		std::vector<Node>::const_iterator itr = m_data.back().begin(), end = m_data.back().end();
		for (; itr != end; ++itr, ++nd)
		{
			*nd = *itr;
		}
		m_data.pop_back();
		m_data.back().back().m_elementsize = ni;
		m_data.back().back().m_element = Node::ref_element( mi);
		if (m_data.size() == 1)
		{
			m_rootidx = mi;
		}
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
	if (m_data.size() < 2)
	{
		throw std::logic_error( "pushed value as root element");
	}
	if (m_data.back().size() > 0)
	{
		throw std::logic_error( "pushed value without tag context");
	}
	if (m_data[ m_data.size()-2].back().m_element != 0)
	{
		throw std::logic_error( "pushed more thatn one value in same tag context");
	}
	m_data[ m_data.size()-2].back().m_elementsize = valsize;
	m_data[ m_data.size()-2].back().m_element = Node::val_element( mi);
}

void Structure::pushValue( const std::string& val)
{
	pushValue( val.c_str(), val.size());
}

bool Structure::next( const Node& nd, int tag, std::vector<Node>& nextnd) const
{
	bool rt = false;
	std::size_t ii = 0, nn = nd.nofchild();
	Node* cd = &m_nodemem[ nd.childidx()];
	for (; ii<nn; ++ii)
	{
		if (tag == 0 || tag == cd[ ii].m_tag)
		{
			nextnd.push_back( cd[ ii]);
			rt = true;
		}
	}
	return rt;
}

bool Structure::find( const Node& nd, int tag, std::vector<Node>& findnd) const
{
	bool rt = false;
	std::size_t ii = 0, nn = nd.nofchild();
	Node* cd = &m_nodemem[ nd.childidx()];
	for (; ii<nn; ++ii)
	{
		if (tag == 0 || tag == cd[ ii].m_tag) findnd.push_back( cd[ ii]);
		if (cd[ ii].nofchild() > 0) rt |= find( cd[ ii], tag, findnd);
	}
	return rt;
}

bool Structure::up( const Node& nd, std::vector<Node>& rt) const
{
	if (nd.m_parent != 0)
	{
		rt.push_back( m_nodemem[ nd.m_parent]);
		return true;
	}
	return false;
}

Path::Path( const std::string& pt, TagTable* tagmap)
{
	Element elem;
	std::string tagnam;
	std::string::const_iterator ii = pt.begin(), ee = pt.end();
	while (ii != ee)
	{
		if (*ii == '$')
		{
			elem.m_type = Result;
			elem.m_tag = m_resultvar.size();
			++ii;
			while (ii < ee && std::isalnum( *ii))
			{
				m_resultvar.push_back( *ii);
			}
		}
		else if (*ii == '/')
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
				elem.m_tag = 0;
			}
			else if (tagnam == ".")
			{
				if (elem.m_type == Find)
				{
					throw std::runtime_error( std::string("selection '//.' is illegal in path '") + pt + "'" );
				}
				elem.m_type = Current;
				elem.m_tag = 0;
			}
			else if (tagnam == "*")
			{
				elem.m_tag = 0;
			}
			else
			{
				tagnam = normalizeTagName( tagnam);
				elem.m_tag = tagmap->get( tagnam);
			}
			m_path.push_back( elem);
		}
	}
	std::vector<Element>::const_iterator pi = m_path.begin(), pe = m_path.end();
	for (; pi != pe; ++pi)
	{
		if (pi->m_type == Result && m_path.size() > 1)
		{
			throw std::runtime_error( "referencing result variable in path");
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
		rt << elementTypeName( ii->m_type) << " " << ii->m_tag << std::endl;
	}
	return rt.str();
}


FunctionCall::FunctionCall( const std::string& n, const Path& s, const std::vector<Path>& a)
	:m_name(n)
	,m_selector(s)
	,m_arg(a) {}

FunctionCall::FunctionCall( const FunctionCall& o)
	:m_name(o.m_name)
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
		std::string selectorstr;
		while (ii < ee && *ii > 0 && *ii < 32) ++ii;
		if (ii == ee) throw std::runtime_error( "unexpected end of expression");
		while (ii < ee && *ii != ':')
		{
			selectorstr.push_back( *ii);
			++ii;
		}
		if (ii == ee) throw std::runtime_error( "unexpected end of expression");
		while (ii < ee && *ii > 0 && *ii < 32) ++ii;
		if (ii == ee) throw std::runtime_error( "unexpected end of expression");
		std::vector<std::string> paramstr;

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
			paramstr.push_back( pp);
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
		Path selector( selectorstr, &m_tagmap);
		std::vector<Path> param;
		std::vector<std::string>::const_iterator ai = paramstr.begin(), ae = paramstr.end();
		for (; ai != ae; ++ai)
		{
			Path pp( *ai, &m_tagmap);
			param.push_back( pp);
		}
		FunctionCall cc( functionname, selector, param);
		m_call.push_back( cc);

		while (ii < ee && *ii > 0 && *ii < 32) ++ii;
		if (ii == ee) break;
		if (*ii != ';') throw std::runtime_error( "missing semicolon as expression separator");
	}
}

bool TransactionProgram::execute( DatabaseInterface* , const std::string& content) const
{
	// [A] Parse content structure:
	Structure st( &m_tagmap);
	std::string::const_iterator ii = content.begin(), ee = content.end();

	while (ii != ee)
	{
		if (*ii == '\n')
		{
			++ii;
			continue;
		}
		langbind::TokenType toktype = (langbind::TokenType)(*ii++);
		std::string::const_iterator tt = ii;
		while (ii != ee && *ii != '\n') ++ii;
		const char* tok = content.c_str()+(tt-content.begin());
		std::size_t toksize = ii-tt;

		switch (toktype)
		{
			case langbind::TokenOpenTag:
				st.openTag( tok, toksize);
			break;
			case langbind::TokenCloseTag:
				st.closeTag();
			break;
			case langbind::TokenAttribute:
				st.openTag( tok, toksize);
			break;
			case langbind::TokenValue:
				st.pushValue( tok, toksize);
			break;
			default:
				throw std::runtime_error( std::string("illegal token in input '") + *ii + "'");
		}
	}
	// [B] Execute commands:
	std::vector<FunctionCall>::const_iterator ci = m_call.begin(), ce = m_call.end();
	for (; ci != ce; ++ci)
	{
		std::vector<Structure::Node> nodearray;
		std::vector<Structure::Node> nodearray2;
		nodearray.push_back( nodearray[ st.root()]);

		// [B.1] Find selected nodes:
		std::vector<Path::Element>::const_iterator si = ci->selector().begin(), se = ci->selector().end();
		for (; si != se; ++si)
		{
			nodearray2.clear();
			std::vector<Structure::Node>::const_iterator ni = nodearray.begin(), ne = nodearray.end();
			for (; ni != ne; ++ni)
			{
				switch (si->m_type)
				{
					case Path::Result:
						throw std::runtime_error( "referencing result in selector");

					case Path::Find:
						st.find( *ni, si->m_tag, nodearray2);
						break;

					case Path::Next:
						st.next( *ni, si->m_tag, nodearray2);
						break;

					case Path::Up:
						st.up( *ni, nodearray2);
						break;

					case Path::Current:
						break;
				}
			}
			nodearray = nodearray2;
		}
		// [B.2] For each selected node do expand the function call arguments and call the function:
		std::vector<Structure::Node>::const_iterator vi=nodearray.begin(),ve=nodearray.end();
		for (; vi!=ve; ++vi)
		{
		}
	}
	return true;
}




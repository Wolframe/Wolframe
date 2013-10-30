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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\brief Implementation of selector path for database transaction functions
///\file transactionfunction/Path.cpp
#include "transactionfunction/Path.hpp"

using namespace _Wolframe;
using namespace _Wolframe::db;

void Path::parseSelectorPath( const std::string& selector, TagTable* tagmap)
{
	Element elem;
	std::string::const_iterator ii = selector.begin(), ee = selector.end();
	for (; ii != ee && (unsigned char)*ii <= 32; ++ii);
	if (ii == ee) return;
	if (*ii == '/')
	{
		elem.m_type = Root;
		m_path.push_back( elem);
	}
	while (ii != ee)
	{
		if (*ii == '/')
		{
			++ii;
			if (ii == ee) break;
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
		}
		std::string tagnam;
		while (ii != ee && *ii != '/')
		{
			tagnam.push_back( *ii);
			++ii;
		}
		if (tagnam.empty())
		{
			throw std::runtime_error( std::string("empty tag in path '") + selector + "'" );
		}
		else if (tagnam == "..")
		{
			if (elem.m_type == Find)
			{
				throw std::runtime_error( std::string("selection '//..' is illegal in path '") + selector + "'" );
			}
			elem.m_type = Up;
			elem.m_tag = 0;
		}
		else if (tagnam == ".")
		{
			if (elem.m_type == Find)
			{
				throw std::runtime_error( std::string("selection '//.' is illegal in path '") + selector + "'" );
			}
			continue;
		}
		else if (tagnam == "*")
		{
			elem.m_tag = 0;
		}
		else
		{
			elem.m_tag = tagmap->get( tagnam);
		}
		m_path.push_back( elem);
	}
}

Path::Path( const std::string& selector, TagTable* tagmap)
{
	parseSelectorPath( selector, tagmap);
}

Path::Path( const Call::Param& param, TagTable* tagmap)
{
	typedef Call::Param Param;
	Element elem;
	switch (param.type)
	{
		case Param::NumericResultReference:
			elem.m_type = ResultIndex;
			elem.m_tag = boost::lexical_cast<unsigned short>( param.value);
			m_path.push_back( elem);
			break;
		case Param::SymbolicResultReference:
			elem.m_type = ResultSymbol;
			m_content = param.value;
			m_path.push_back( elem);
			break;
		case Param::Constant:
			elem.m_type = Constant;
			m_content = param.value;
			m_path.push_back( elem);
			break;
		case Param::InputSelectorPath:
			parseSelectorPath( param.value, tagmap);
			break;
	}
}

Path::Path( const Path& o)
	:m_path(o.m_path),m_content(o.m_content){}


void Path::rewrite( const std::map<int,int>& rwtab, int scope_functionidx_incr)
{
	std::vector<Element>::iterator pi = m_path.begin(), pe = m_path.end();
	for (; pi != pe; ++pi)
	{
		if (pi->m_type == ResultIndex || pi->m_type == ResultSymbol)
		{
			if (pi->m_scope_functionidx >= 0)
			{
				pi->m_scope_functionidx += scope_functionidx_incr;
			}
		}
		else if (pi->m_type == Next || pi->m_type == Find)
		{
			std::map<int,int>::const_iterator re = rwtab.find( pi->m_tag);
			if (re == rwtab.end()) throw std::logic_error( "rewrite table not complete");
			pi->m_tag = re->second;
		}
	}
}

void Path::append( const Path& o)
{
	m_path.insert( m_path.end(), o.begin(), o.end());
}

Path::ElementType Path::referenceType() const
{
	if (m_path.size() == 1) return m_path[0].m_type;
	return Root;
}

int Path::resultReferenceScope() const
{
	if (m_path.size() == 1) return m_path[0].m_scope_functionidx;
	return -1;
}

std::size_t Path::resultReferenceIndex() const
{
	if (m_path.size() == 1 && m_path[0].m_type == ResultIndex) return m_path[0].m_tag;
	throw std::logic_error("internal: illegal call of Path::resultReferenceIndex");
}

const std::string& Path::resultReferenceSymbol() const
{
	if (m_path.size() == 1 && m_path[0].m_type == ResultSymbol) return m_content;
	throw std::logic_error("internal: illegal call of Path::resultReferenceSymbol");
}

const std::string& Path::constantReference() const
{
	if (m_path.size() == 1 && m_path[0].m_type == Constant) return m_content;
	throw std::logic_error("internal: illegal call of Path::constantReference");
}

std::string Path::tostring() const
{
	switch (referenceType())
	{
		case ResultIndex:
		{
			std::string rt = std::string("$") + boost::lexical_cast<std::string>( resultReferenceIndex());
			if (resultReferenceScope() >= 0) rt = rt + ":" + boost::lexical_cast<std::string>( resultReferenceScope());
			return rt;
		}
		case ResultSymbol:
		{
			std::string rt = std::string("$") + resultReferenceSymbol();
			if (resultReferenceScope() >= 0) rt = rt + ":" + boost::lexical_cast<std::string>( resultReferenceScope());
			return rt;
		}
		case Constant: return std::string("'") + constantReference() + std::string("'");

		case Find:
		case Root:
		case Next:
		case Up:
		{
			std::vector<Element>::const_iterator ii = m_path.begin(), ee = m_path.end();
			std::ostringstream rt;
			for (; ii != ee; ++ii)
			{
				rt << elementTypeName( ii->m_type) << " " << ii->m_tag << std::endl;
			}
			return rt.str();
		}
	}
	throw std::logic_error( "internal: illegal state in Path::tostring()");
}

void Path::selectNodes( const TransactionFunctionInput::Structure& st, const TransactionFunctionInput::Structure::Node* nd, std::vector<const TransactionFunctionInput::Structure::Node*>& ar) const
{
	typedef TransactionFunctionInput::Structure::Node Node;
	std::vector<const Node*> ar1,ar2;
	ar1.push_back( nd);

	// [B.1] Find selected nodes:
	std::vector<Element>::const_iterator si = begin(), se = end();
	for (; si != se; ++si)
	{
		ar2.clear();
		std::vector<const Node*>::const_iterator ni = ar1.begin(), ne = ar1.end();
		for (; ni != ne; ++ni)
		{
			switch (si->m_type)
			{
				case ResultIndex:
				case ResultSymbol:
				case Constant:
					break;

				case Find:
					st.find( *ni, si->m_tag, ar2);
					break;

				case Root:
					ar2.push_back( st.root());
					break;

				case Next:
					st.next( *ni, si->m_tag, ar2);
					break;

				case Up:
					st.up( *ni, ar2);
					break;
			}
		}
		ar1 = ar2;
	}
	ar.insert( ar.end(), ar1.begin(), ar1.end());
}

ConstantReferencePath::ConstantReferencePath( const std::string& value)
{
	Element elem;
	elem.m_type = Constant;
	m_content = value;
	m_path.push_back( elem);
}



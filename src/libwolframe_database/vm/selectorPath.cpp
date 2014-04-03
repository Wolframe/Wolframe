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
//\file vm/selectorPath.cpp
//\brief Implementation of an input selector path
#include "database/vm/selectorPath.hpp"
#include "transactionfunction/InputStructure.hpp"
#include "transactionfunction/TagTable.hpp"

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::db::vm;
using namespace _Wolframe::db::tf;

SelectorPath::SelectorPath( const std::string& selector, TagTable* tagmap)
{
	Element elem;
	std::string::const_iterator ii = selector.begin(), ee = selector.end();
	for (; ii != ee && (unsigned char)*ii <= 32; ++ii);
	if (ii == ee) return;
	if (*ii == '/')
	{
		elem.m_type = Element::Root;
		m_path.push_back( elem);
	}
	else
	{
		elem.m_type = Element::Current;
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
				elem.m_type = Element::Find;
				++ii;
			}
			else
			{
				elem.m_type = Element::Next;
			}
		}
		else
		{
			elem.m_type = Element::Next;
		}
		std::string tagnam;
		while (ii != ee && *ii != '/')
		{
			tagnam.push_back( *ii);
			++ii;
		}
		if (tagnam.empty())
		{
			throw std::runtime_error( std::string("empty tag in path '") + selector + "'");
		}
		else if (tagnam == "..")
		{
			if (elem.m_type == Element::Find)
			{
				throw std::runtime_error( std::string("selection '//..' is illegal in path '") + selector + "'");
			}
			elem.m_type = Element::Up;
			elem.m_tag = 0;
		}
		else if (tagnam == ".")
		{
			if (elem.m_type == Element::Find)
			{
				throw std::runtime_error( std::string("selection '//.' is illegal in path '") + selector + "'");
			}
			continue;
		}
		else if (tagnam == "*")
		{
			elem.m_tag = 0;
		}
		else
		{
			if (std::strchr( tagnam.c_str(), '*') != 0)
			{
				throw std::runtime_error( "asterisk character '*' not allowed as character in a token other than the string \"*\"");
			}
			elem.m_tag = tagmap->get( tagnam);
		}
		m_path.push_back( elem);
	}
	std::string redu = tostring( tagmap);
	if (redu != selector)
	{
		throw std::runtime_error( std::string("illegal selection path string '") + selector + "' (reduced to '" + redu + "')");
	}
}

void SelectorPath::selectNodes( const InputStructure& st, const InputNodeVisitor& nv, std::vector<InputNodeIndex>& ar) const
{
	std::vector<InputNodeIndex> ar1,ar2;
	ar1.push_back( nv.m_nodeidx);

	// [B.1] Find selected nodes:
	std::vector<Element>::const_iterator si = begin(), se = end();
	for (; si != se; ++si)
	{
		ar2.clear();
		std::vector<InputNodeIndex>::const_iterator ni = ar1.begin(), ne = ar1.end();
		for (; ni != ne; ++ni)
		{
			switch (si->m_type)
			{
				case Element::Find:
					st.find( st.node(*ni), si->m_tag, ar2);
					break;

				case Element::Current:
					break;
				case Element::Root:
					ar2.push_back( st.rootindex());
					break;

				case Element::Next:
					st.next( st.node(*ni), si->m_tag, ar2);
					break;

				case Element::Up:
					st.up( st.node(*ni), ar2);
					break;
			}
		}
		ar1 = ar2;
	}
	ar.insert( ar.end(), ar1.begin(), ar1.end());
}

void SelectorPath::print( std::ostream& out, const TagTable* tagmap) const
{
	std::vector<Element>::const_iterator ii = m_path.begin(), ee = m_path.end();
	Element::Type prev = Element::Next;
	for (; ii != ee; ++ii)
	{
		switch (ii->m_type)
		{
			case Element::Current:
				break;
			case Element::Root:
				if (ii != m_path.begin())
				{
					throw std::runtime_error( "illegal path");
				}
				break;

			case Element::Next:
				if (prev != Element::Current)
				{
					out << "/";
				}
				out << tagmap->getstr(ii->m_tag);
				break;

			case Element::Find: 
				if (prev == Element::Current)
				{
					out << ".//";
				}
				else
				{
					out << "//";
				}
				out << tagmap->getstr(ii->m_tag);
				break;

			case Element::Up:
				if (prev == Element::Root)
				{
					throw std::runtime_error( "illegal path");
				}
				if (prev == Element::Current)
				{
					out << "..";
				}
				else
				{
					out << "/..";
				}
				break;
		}
		prev = ii->m_type;
	}
	if (prev == Element::Root)
	{
		out << "/";
	}
	else if (prev == Element::Current)
	{
		out << ".";
	}
}

std::string SelectorPath::tostring( const TagTable* tagmap) const
{
	std::ostringstream rt;
	print( rt, tagmap);
	return rt.str();
}

std::string SelectorPath::normalize( const std::string& pathstr)
{
	TagTable tm( false);
	return SelectorPath( pathstr, &tm).tostring( &tm);
}



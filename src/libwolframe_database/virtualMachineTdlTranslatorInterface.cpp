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
//\file virtualMachineTdlTranslatorInterface.cpp
//\brief Implementation of the helpers for building a virtual machine for database transactions out of TDL
#include "virtualMachineTdlTranslatorInterface.hpp"

using namespace _Wolframe;
using namespace _Wolframe::db;

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

std::string SelectorPath::tostring( const TagTable* tagmap) const
{
	std::vector<Element>::const_iterator ii = m_path.begin(), ee = m_path.end();
	std::ostringstream rt;
	bool prev_root = false;
	for (; ii != ee; ++ii)
	{
		switch (ii->m_type)
		{
			case Element::Root:
				if (ii == m_path.begin())
				{
					prev_root = true;
					rt << '/';
				}
				else
				{
					throw std::runtime_error( "illegal path");
				}
				break;

			case Element::Next:
				if (!prev_root)
				{
					rt << "/";
				}
				rt << tagmap->getstr(ii->m_tag);
				prev_root = false;
				break;

			case Element::Find: 
				if (prev_root)
				{
					rt << "/";
				}
				else
				{
					rt << "//";
				}
				rt << tagmap->getstr(ii->m_tag);
				prev_root = false;
				break;

			case Element::Up:
				if (prev_root) throw std::runtime_error( "illegal path");
				rt << "/..";
				prev_root = false;
				break;
		}
	}
	return rt.str();
}

void SelectorPath::selectNodes( const InputStructure& st, const NodeVisitor& nv, std::vector<NodeIndex>& ar) const
{
	std::vector<NodeVisitor::Index> ar1,ar2;
	ar1.push_back( nv.m_nodeidx);

	// [B.1] Find selected nodes:
	std::vector<Element>::const_iterator si = begin(), se = end();
	for (; si != se; ++si)
	{
		ar2.clear();
		std::vector<NodeVisitor::Index>::const_iterator ni = ar1.begin(), ne = ar1.end();
		for (; ni != ne; ++ni)
		{
			switch (si->m_type)
			{
				case Element::Find:
					st.find( st.node(*ni), si->m_tag, ar2);
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



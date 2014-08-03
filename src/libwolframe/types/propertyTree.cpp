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
//\file types/propertyTree.cpp
//\brief Implementation of a key value tree based on boost::property_tree::ptree with position info for better error reporting
#include "types/propertyTree.hpp"
#include "utils/printFormats.hpp"
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::types;

PropertyTree::Node::Node( const boost::property_tree::ptree& pt)
{
	boost::property_tree::ptree::const_iterator pi = pt.begin(), pe = pt.end();
	for (; pi != pe; ++pi)
	{
		add_child( pi->first, Node( pi->second));
	}
	m_data = pt.data();
}

PropertyTree::Node::Node( const Node& o)
	:m_subnodes(o.m_subnodes),m_position(o.m_position),m_data(o.m_data)
{}

void PropertyTree::Node::recursiveSetFileName( PropertyTree::Node& pt, const FileName& filename)
{
	Node::iterator pi = pt.begin(), pe = pt.end();
	for (; pi != pe; ++pi)
	{
		recursiveSetFileName( pi->second, filename);
	}
	pt.m_position.setFileName( filename);
}

std::string PropertyTree::Node::tostring( const utils::PrintFormat* pformat) const
{
	std::ostringstream out;
	print( out, *this, 0, pformat);
	return out.str();
}

void PropertyTree::Node::print( std::ostringstream& out, const Node& nd, int indent, const utils::PrintFormat* pformat)
{
	std::string indentstr;
	if (!pformat) pformat = utils::logPrintFormat();
	for (int ii=0; ii<indent; ++ii) indentstr.append( pformat->indent);
	for (Node::const_iterator ni=nd.begin(), ne=nd.end(); ni != ne; ++ni)
	{
		if (!ni->second.data().empty())
		{
			out << pformat->newitem << indentstr
				<< ni->first << pformat->assign
				<< pformat->startvalue << ni->second.data() << pformat->endvalue; 
		}
		else
		{
			out << indentstr << ni->first << pformat->openstruct;
			if (ni->second.begin() == ni->second.end())
			{
				out << pformat->closestruct;
			}
			else
			{
				print( out, ni->second, indent+1);
				out << pformat->newitem << indentstr << pformat->closestruct;
			}
		}
	}
}

const PropertyTree::Node& PropertyTree::Node::getChild( const std::string& nodename) const
{
	static const Node emptynode;
	bool found = false;
	const types::PropertyTree::Node* rt = &emptynode;
	types::PropertyTree::Node::const_iterator gi = begin(), ge = end();
	for (; gi != ge; ++gi)
	{
		if (boost::algorithm::iequals( gi->first, nodename))
		{
			if (found) throw std::runtime_error( std::string("duplicate '") + nodename + "' node in configuration");
			rt = &gi->second;
			found = true;
		}
	}
	return *rt;
}

std::vector<const PropertyTree::Node*> PropertyTree::Node::getChildren( const std::string& nodename) const
{
	std::vector<const PropertyTree::Node*> rt;
	types::PropertyTree::Node::const_iterator gi = begin(), ge = end();
	for (; gi != ge; ++gi)
	{
		if (boost::algorithm::iequals( gi->first, nodename))
		{
			rt.push_back( &gi->second);
		}
	}
	return rt;
}

PropertyTree::Node PropertyTree::Node::getChildrenJoined( const std::string& nodename) const
{
	Node rt( m_position);
	unsigned int defcnt = 0;
	Position first_def_pos;
	types::PropertyTree::Node::const_iterator gi = begin(), ge = end();
	for (; gi != ge; ++gi)
	{
		if (boost::algorithm::iequals( gi->first, nodename))
		{
			first_def_pos = gi->second.m_position;
			++defcnt;
			if (!gi->second.m_data.empty())
			{
				if (!rt.m_data.empty())
				{
					throw std::runtime_error( "cannot join nodes (multiple data definitions)");
				}
				rt.m_data = gi->second.m_data;
			}
			rt.m_subnodes.insert( rt.m_subnodes.end(), gi->second.begin(), gi->second.end());
		}
	}
	if (defcnt == 1)
	{
		rt.m_position = first_def_pos;
	}
	return rt;
}


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
///\file types/propertyTree.hpp
///\brief Interface to a key value tree based on boost::property_tree::ptree with position info for better error reporting

#ifndef _WOLFRAME_PROPERTY_TREE_HPP_INCLUDED
#define _WOLFRAME_PROPERTY_TREE_HPP_INCLUDED
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>

namespace _Wolframe {
namespace types {

class PropertyTree
{
public:
	class Position
	{
	public:
		unsigned int fileidx() const	{return m_fileidx;}
		unsigned int line() const	{return m_line;}
		unsigned int column() const	{return m_column;}
	
		Position()
			:m_fileidx(0),m_line(0),m_column(0){}
		Position( const Position& o)
			:m_fileidx(o.m_fileidx),m_line(o.m_line),m_column(o.m_column){}
		Position( unsigned int fileidx_, unsigned int line_, unsigned int column_)
			:m_fileidx(fileidx_),m_line(line_),m_column(column_){}

		void fileidx( unsigned int fileidx_)
		{
			m_fileidx = fileidx_;
		}

	private:
		unsigned int m_fileidx;
		unsigned int m_line;
		unsigned int m_column;
	};

	class Value
		:public std::string
	{
	public:
		Value(){}
		Value( const std::string& token)
			:std::string(token){}
		Value( const std::string& token, Position position_)
			:std::string(token),position(position_){}
		Value( const Value& o)
			:std::string(o),position(o.position){}

		Position position;
		std::string string() const	{return *this;}
	};

	class Node
		:public boost::property_tree::basic_ptree<std::string, Value>
	{
		typedef boost::property_tree::basic_ptree<std::string, Value> Parent;

	public:
		Node(){}
		Node( const Parent& o)
			:Parent(o){}
		Node( const std::string& val)
			:Parent(Value( val, Position())){}

		Node( const boost::property_tree::ptree& pt)
		{
			boost::property_tree::ptree::const_iterator pi = pt.begin(), pe = pt.end();
			for (; pi != pe; ++pi)
			{
				Parent::add_child( pi->first, Node( pi->second));
			}
			if (!pt.get_value<std::string>().empty())
			{
				Parent::put_value( Value( pt.data(), Position()));
			}
		}

		void setValue( const std::string& val)
		{
			Parent::put_value( Value( val, Position()));
		}

		void setFileIdx( unsigned int fileidx)
		{
			setFileIdx( *this, fileidx);
		}

	private:
		static void setFileIdx( Parent& pt, unsigned int fileidx)
		{
			Parent::iterator pi = pt.begin(), pe = pt.end();
			for (; pi != pe; ++pi)
			{
				setFileIdx( pi->second, fileidx);
			}
			pt.data().position.fileidx( fileidx);
		}
	};

	typedef boost::property_tree::ptree_bad_data BadDataException;

public:
	PropertyTree(){}
	PropertyTree( const PropertyTree& o)
		:m_root(o.m_root){}

	PropertyTree( const boost::property_tree::ptree& o, const std::string& filename_)
		:m_root(o)
	{
		m_filenames.push_back( filename_);
	}

	Node root() const			{return m_root;}
	Node::const_iterator begin() const	{return m_root.begin();}
	Node::const_iterator end() const	{return m_root.end();}

private:
	Node m_root;
	std::vector<std::string> m_filenames;
};

}}//namespace
#endif




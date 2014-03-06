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
#include <cstdlib>
#include <cstring>
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>

namespace _Wolframe {
namespace types {

class PropertyTree
{
public:
	typedef boost::shared_ptr<char> FileName;
	static FileName getFileName( const std::string& name)
	{
		char* cc = (char*)std::malloc( name.size()+1);
		if (!cc) throw std::bad_alloc();
		std::memcpy( cc, name.c_str(), name.size()+1);
		return boost::shared_ptr<char>( cc, std::free);
	}

	class Position
	{
	public:

	public:
		const char* filename() const	{return m_filename.get();}
		unsigned int line() const	{return m_line;}
		unsigned int column() const	{return m_column;}
	
		Position()
			:m_line(0),m_column(0){}
		Position( const Position& o)
			:m_filename(o.m_filename),m_line(o.m_line),m_column(o.m_column){}
		Position( FileName filename_, unsigned int line_, unsigned int column_)
			:m_filename(filename_),m_line(line_),m_column(column_){}
		Position( unsigned int line_, unsigned int column_)
			:m_line(line_),m_column(column_){}

		void setFileName( const FileName& filename_)
		{
			m_filename = filename_;
		}

	private:
		FileName m_filename;
		unsigned int m_line;
		unsigned int m_column;
	};

	class Value
		:public std::string
	{
	public:
		Value(){}
		Value( const std::string& token, Position position_=Position())
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
		Node( const Position& pos=Position())
		{
			data().position = pos;
		}

		Node( const Parent& o, const Position& pos=Position())
			:Parent(o)
		{
			data().position = pos;
		}

		Node( const std::string& val, const Position& pos=Position())
			:Parent( Value( val, pos)){}

		Node( const boost::property_tree::ptree& pt)
		{
			boost::property_tree::ptree::const_iterator pi = pt.begin(), pe = pt.end();
			for (; pi != pe; ++pi)
			{
				Parent::add_child( pi->first, Node( pi->second));
			}
			if (!pt.data().empty())
			{
				Parent::put_value( Value( pt.data()));
			}
		}

		void setValue( const std::string& val)
		{
			Parent::put_value( Value( val));
		}

		void setPosition( const Position& p)
		{
			data().position = p;
		}

		void recursiveSetFileName( const FileName& filename)
		{
			recursiveSetFileName( *this, filename);
		}

	private:
		static void recursiveSetFileName( Parent& pt, const FileName& filename)
		{
			Parent::iterator pi = pt.begin(), pe = pt.end();
			for (; pi != pe; ++pi)
			{
				recursiveSetFileName( pi->second, filename);
			}
			pt.data().position.setFileName( filename);
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
		m_root.recursiveSetFileName( getFileName( filename_));
	}

	PropertyTree( const Node& root_)
		:m_root(root_){}

	Node root() const			{return m_root;}
	Node::const_iterator begin() const	{return m_root.begin();}
	Node::const_iterator end() const	{return m_root.end();}

private:
	Node m_root;
};

}}//namespace
#endif




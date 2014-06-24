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
/// \file types/propertyTree.hpp
/// \brief Interface to a key value tree based on boost::property_tree::ptree with position info for better error reporting

#ifndef _WOLFRAME_PROPERTY_TREE_HPP_INCLUDED
#define _WOLFRAME_PROPERTY_TREE_HPP_INCLUDED
#include "utils/fileLineInfo.hpp"
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>

namespace _Wolframe {
namespace utils {
	struct PrintFormat;
}
namespace types {

/// \class PropertyTree
/// \brief Hierarchical data structure for representing configuration data trees
class PropertyTree
{
public:
	/// \brief Position in the source file where the node was defined
	typedef utils::FileLineInfo Position;
	/// \brief Filename of the source where the node was defined
	typedef utils::FileLineInfo::FileName FileName;

	/// \class Value
	/// \brief Property tree node value
	class Value
		:public std::string
	{
	public:
		/// \brief Default constructor
		Value(){}
		/// \brief Constructor
		Value( const std::string& token, Position position_=Position())
			:std::string(token),position(position_){}
		/// \brief Copy constructor
		Value( const Value& o)
			:std::string(o),position(o.position){}

		Position position;					///< position of node definition start in source
		std::string string() const	{return *this;}		///< cast to string (cast operator leads to problems here, so we define it with an explicit method call)
	};

	/// \class Node
	/// \brief Property tree node
	class Node
	{
	public:
		typedef std::pair<std::string,Node> Assignment;

	public:
		/// \brief Constructor
		explicit Node( const Position& pos=Position())
		{
			m_position = pos;
		}

		/// \brief Constructor
		explicit Node( const std::string& val, const Position& pos=Position())
			:m_position(pos),m_data(val){}

		/// \brief Constructor
		explicit Node( const boost::property_tree::ptree& pt);

		/// \brief Copy constructor
		Node( const Node& o);

		/// \brief Set the value of the node
		void setValue( const std::string& val)		{m_data = val;}

		/// \brief Set the position of the node in source
		void setPosition( const Position& p)		{m_position = p;}

		/// \brief Set the file name of the definition of this node in source recursively for all sub nodes where the file name of definition has not yet been defined
		void recursiveSetFileName( const FileName& filename)
		{
			recursiveSetFileName( *this, filename);
		}

		/// \brief Get the property sub tree of this node as string
		std::string tostring( const utils::PrintFormat* pformat=0) const;

		/// \brief Get a child node by name
		/// \remark no path expression argument allowed as in boost::property_tree
		const Node& getChild( const std::string& nodename) const;

		/// \brief Get a all children nodes with the a certain name
		/// \remark no path expression argument allowed as in boost::property_tree
		std::vector<const Node*> getChildren( const std::string& nodename) const;

		/// \brief Get a all children nodes with the a certain name joined to one node
		/// \remark no path expression argument allowed as in boost::property_tree
		Node getChildrenJoined( const std::string& nodename) const;

		/// \brief Add child reference
		void add_child( const std::string& name_, const Node& node_)
		{
			m_subnodes.push_back( Assignment( name_, node_));
		}

		bool empty() const					{return m_subnodes.empty();}
		const std::string& data() const				{return m_data;}
		const Position& position() const			{return m_position;}

		typedef std::vector<Assignment>::const_iterator const_iterator;
		typedef std::vector<Assignment>::iterator iterator;

		std::vector<Assignment>::const_iterator begin() const	{return m_subnodes.begin();}
		std::vector<Assignment>::const_iterator end() const	{return m_subnodes.end();}
		std::vector<Assignment>::iterator begin()		{return m_subnodes.begin();}
		std::vector<Assignment>::iterator end()			{return m_subnodes.end();}

	private:
		/// \brief Print the property sub tree of this node to a stream
		static void print( std::ostringstream& out, const Node& nd, int indent, const utils::PrintFormat* pformat=0);

		/// \brief Set the file name of the definition of the node 'pt' in source recursively for all sub nodes where the file name of definition has not yet been defined
		static void recursiveSetFileName( Node& pt, const FileName& filename);

	private:
		std::vector<Assignment> m_subnodes;			///< child node list
		Position m_position;					///< position of node definition start in source
		std::string m_data;					///< node value
	};

public:
	/// \brief Constructor
	PropertyTree(){}
	/// \brief Copy constructor
	PropertyTree( const PropertyTree& o)
		:m_root(o.m_root){}

	/// \brief Constructor
	PropertyTree( const boost::property_tree::ptree& o, const std::string& filename_)
		:m_root(o)
	{
		m_root.recursiveSetFileName( utils::FileLineInfo::getFileName( filename_));
	}

	/// \brief Constructor
	PropertyTree( const Node& root_)
		:m_root(root_){}

	/// \brief Get the root node of the tree
	const Node& root() const		{return m_root;}
	/// \brief Get the start iterator of the root node children
	Node::const_iterator begin() const	{return m_root.begin();}
	/// \brief Get the end iterator of the root node children
	Node::const_iterator end() const	{return m_root.end();}
	/// \brief Get the start iterator of the root node children
	Node::iterator begin()			{return m_root.begin();}
	/// \brief Get the end iterator of the root node children
	Node::iterator end()			{return m_root.end();}

	/// \brief Get this property tree as string
	std::string tostring( const utils::PrintFormat* pformat=0) const
	{
		return m_root.tostring( pformat);
	}

private:
	Node m_root;				///< root node of the property tree
};

}}//namespace
#endif




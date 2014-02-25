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
///\brief Internal interface for input data structure of transaction function
///\file transactionfunction/InputStructure.hpp
#ifndef _DATABASE_TRANSACTION_FUNCTION_INPUT_STRUCTURE_HPP_INCLUDED
#define _DATABASE_TRANSACTION_FUNCTION_INPUT_STRUCTURE_HPP_INCLUDED
#include "database/transactionFunction.hpp"
#include "transactionfunction/TagTable.hpp"
#include "types/allocators.hpp"
#include "filter/typedfilter.hpp"
#include "utils/printFormats.hpp"
#include <string>
#include <map>
#include <vector>

namespace _Wolframe {
namespace db {

class TransactionFunctionInput::Structure
{
public:
	///\brief Constructor
	explicit Structure( const TagTable* tagmap);
	///\brief Copy constructor
	Structure( const Structure& o);

	///\class Node
	///\brief Node of the structure
	struct Node
	{
		int m_parent;
		int m_tag;
		int m_tagstr;
		int m_arrayindex;
		int m_next;
		int m_value;
		int m_firstchild;
		int m_lastchild;

		Node()
			:m_parent(0)
			,m_tag(0)
			,m_tagstr(0)
			,m_arrayindex(-1)
			,m_next(0)
			,m_value(0)
			,m_firstchild(0)
			,m_lastchild(0){}

		Node( const Node& o)
			:m_parent(o.m_parent)
			,m_tag(o.m_tag)
			,m_tagstr(o.m_tagstr)
			,m_arrayindex(o.m_arrayindex)
			,m_next(o.m_next)
			,m_value(o.m_value)
			,m_firstchild(o.m_firstchild)
			,m_lastchild(o.m_lastchild){}
	};

	///\class NodeVisitor
	///\brief Tree node reference for tree building and traversal
	class NodeVisitor
	{
	public:
		typedef int Index;
		///\brief Constructor
		NodeVisitor( Index nodeidx_=0)		:m_nodeidx(nodeidx_){}
		///\brief Copy constructor
		NodeVisitor( const NodeVisitor& o)	:m_nodeidx(o.m_nodeidx){}

		Index m_nodeidx;			//< index of the visited node
	};

public://visit structure:
	bool case_sensitive() const					{return m_tagmap->case_sensitive();}

	///\brief Get the node pointer from a visitor reference
	const Node* node( const NodeVisitor& nv) const;
	Node* node( const NodeVisitor& nv);

	///\brief Get the visitor from a node pointer
	NodeVisitor visitor( const Node* nd) const			{return NodeVisitor( nd-m_nodemem.base());}
	NodeVisitor visitor( Node* nd)					{return NodeVisitor( nd-m_nodemem.base());}

	///\brief Get the root node of the tree
	const Node* root() const;
	///\brief Get the root node visitor of the tree
	NodeVisitor rootvisitor() const					{return NodeVisitor(0);}
	///\brief Get the root node index of the tree
	NodeVisitor::Index rootindex() const				{return 0;}

	///\brief Get all children of a node having a tag matching to tag or all children if tag is 0.
	///\param[out] rt where to append the result nodes
	void next( const Node* nd, int tag, std::vector<NodeVisitor::Index>& rt) const;
	///\brief Get all children (transitive, e.g. also children of children) of a node having a tag matching to tag or all children if tag is 0.
	///\param[out] rt where to append the result nodes
	void find( const Node* nd, int tag, std::vector<NodeVisitor::Index>& rt) const;
	///\brief Append parent of a node to a node list
	///\param[out] rt where to append the result node
	void up( const Node* nd, std::vector<NodeVisitor::Index>& rt) const;
	///\brief Get the value of a node or null, if not defined
	const types::Variant* contentvalue( const Node* nd) const;
	///\brief Get the name of the tag of a node
	const char* tagname( const Node* nd) const;
	///\brief Get the absolute path of a node as string with '/' as path element delimiter
	std::string nodepath( const NodeVisitor& nv) const;
	///\brief Get the absolute path of a node as string with '/' as path element delimiter
	std::string nodepath( const Node* nd) const;

	///\brief Print a structure in a specified format to an output stream
	void print( std::ostream& out, const utils::PrintFormat* pformat, const NodeVisitor& nv = NodeVisitor()) const;
	///\brief Get structure as string
	const std::string tostring( const NodeVisitor& nv = NodeVisitor(), const utils::PrintFormat* pformat=utils::logPrintFormat()) const;
	///\brief Find out if two tags are the same (depends on TagMap::case_sensitive())
	bool isequalTag( const std::string& t1, const std::string& t2) const;

	bool check( const NodeVisitor& nv = NodeVisitor()) const;

	///\brief Declaration of an element (element name plus node reference) of a structure
	typedef std::pair<std::string,NodeVisitor::Index> NodeAssignment;
	///\brief Create an input filter for a list of nodes and their content structures to pass to a function as parameters
	langbind::TypedInputFilter* createInputFilter( const std::vector<NodeAssignment>& nodes_) const;
	///\brief Create an output filter for the output of a preprocessing command
	langbind::TypedOutputFilter* createOutputFilter( const NodeVisitor& nv, const std::map<int, bool>& sourccetagmap);

public://create structure without explicit visitor context:
	void openTag( const types::Variant& tag)		{m_visitor = openTag( m_visitor, tag);}
	void closeTag()						{m_visitor = closeTag( m_visitor);}
	void pushValue( const types::VariantConst& val)		{pushValue( m_visitor, val);}

public://create structure with explicit visitor context:
	NodeVisitor visitTag( const NodeVisitor& nv, const std::string& tag) const;
	NodeVisitor visitOrOpenUniqTag( const NodeVisitor& nv, const std::string& tag);
	NodeVisitor openTag( const NodeVisitor& nv, const types::Variant& tag);
	NodeVisitor closeTag( const NodeVisitor& nv);
	void pushValue( const NodeVisitor& nv, const types::VariantConst& val);

private://create structure:
	NodeVisitor openTag( const NodeVisitor& nv, const std::string& tag);
	NodeVisitor createChildNode( const NodeVisitor& nv);
	NodeVisitor createSiblingNode( const NodeVisitor& nv);
	bool isArrayNode( const NodeVisitor& nv) const;

private://data:
	types::TypedArrayDoublingAllocator<Node> m_nodemem;	//< tree nodes
	std::vector<types::Variant> m_content;			//< tree values
	const TagTable* m_tagmap;				//< map names used in selections to node tag identifiers
	TagTable m_privatetagmap;				//< map unused names to node tag identifiers
	NodeVisitor m_visitor;					//< context for building the tree without visitor
};

}}//namespace
#endif


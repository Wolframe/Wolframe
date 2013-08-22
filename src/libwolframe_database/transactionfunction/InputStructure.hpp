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
///\brief Internal interface for input data structure of transaction function
///\file transactionfunction/InputStructure.hpp
#ifndef _DATABASE_TRANSACTION_FUNCTION_INPUT_STRUCTURE_HPP_INCLUDED
#define _DATABASE_TRANSACTION_FUNCTION_INPUT_STRUCTURE_HPP_INCLUDED
#include "database/transactionFunction.hpp"
#include "transactionfunction/TagTable.hpp"
#include "types/allocators.hpp"
#include "filter/typedfilter.hpp"
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
		int m_elementsize;
		int m_element;

		Node()
			:m_parent(0)
			,m_tag(0)
			,m_elementsize(0)
			,m_element(0){}

		Node( const Node& o)
			:m_parent(o.m_parent)
			,m_tag(o.m_tag)
			,m_elementsize(o.m_elementsize)
			,m_element(o.m_element){}

		Node( int p, int t, int size, int e)
			:m_parent(p)
			,m_tag(t)
			,m_elementsize(size)
			,m_element(e){}

		bool operator == (const Node& o) const;
		bool operator != (const Node& o) const		{return !operator==(o);}

		static int ref_element( std::size_t idx)	{if (idx >= (std::size_t)std::numeric_limits<int>::max()) throw std::bad_alloc(); return -(int)idx;}
		static int val_element( std::size_t idx)	{if (idx >= (std::size_t)std::numeric_limits<int>::max()) throw std::bad_alloc(); return (int)idx;}

		std::size_t childidx() const			{return (m_element < 0)?(std::size_t)-m_element:0;}
		std::size_t nofchild() const			{return (m_element < 0)?(std::size_t)m_elementsize:0;}
		std::size_t valueidx() const			{return (m_element > 0)?(std::size_t)m_element:0;}
	};

public://visit structure:
	Node root() const;
	void next( const Node& nd, int tag, std::vector<Node>& rt) const;
	void find( const Node& nd, int tag, std::vector<Node>& rt) const;
	void up( const Node& nd, std::vector<Node>& rt) const;
	const types::Variant* nodevalue( const Node& nd) const;

	///\brief Get structure as string
	const std::string tostring() const;

	typedef std::pair<std::string,std::string> NodeAssignment;
	///\brief Create an input filter for a node to pass it to a function
	langbind::TypedInputFilter* createFilter( const Node& nodeidx_, const std::vector<NodeAssignment>& noderenames_) const;

public://create structure:
	void setParentLinks( std::size_t mi);
	void openTag( const char* tag, std::size_t tagsize);
	void openTag( const std::string& tag);
	void closeTag();
	void createRootNode();
	void pushValue( const types::VariantConst& val);
	void check() const;

private:
	types::TypedArrayDoublingAllocator<Node> m_nodemem;	//< tree nodes
	std::vector<types::Variant> m_content;			//< tree values
	const TagTable* m_tagmap;				//< map names to node tag identifiers
	std::size_t m_rootidx;					//< root index of the tree
	std::size_t m_rootsize;					//< number of children of the root node

	typedef std::vector< std::vector<Node> > BuildNodeStruct;
	BuildNodeStruct m_data;					//< data structure for incomplete tree (under construction). empty when complete
};

}}//namespace
#endif


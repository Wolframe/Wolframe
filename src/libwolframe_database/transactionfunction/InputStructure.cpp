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
///\brief Implementation of input data structure for the transaction function
///\file transactionfunction/InputStructure.cpp
#include "transactionfunction/InputStructure.hpp"
#include "filter/typedfilter.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

bool TransactionFunctionInput::Structure::Node::operator == (const Node& o) const
{
	if (m_parent != o.m_parent) return false;
	if (m_tag != o.m_tag) return false;
	if (m_element != o.m_element) return false;
	if (m_elementsize != o.m_elementsize) return false;
	return true;
}

TransactionFunctionInput::Structure::Structure( const Structure& o)
	:m_nodemem(o.m_nodemem)
	,m_content(o.m_content)
	,m_tagmap(o.m_tagmap)
	,m_root(o.m_root)
	,m_strings(o.m_strings)
	,m_data(o.m_data)
	{}

TransactionFunctionInput::Structure::Structure( const TagTable* tagmap)
	:m_tagmap(tagmap)
{
	m_nodemem.alloc( 1);
	m_content.push_back( types::Variant());
	m_strings.push_back( '\0');
	m_data.push_back( std::vector<Node>());
}

void TransactionFunctionInput::Structure::check() const
{
	if (m_data.size() != 1)
	{
		throw std::runtime_error( "tags not balanced in structure");
	}
}

void TransactionFunctionInput::Structure::setParentLinks( std::size_t mi)
{
	Node* nd = &m_nodemem[mi];
	std::size_t ii = 0, nn = nd->nofchild(), ci = nd->childidx();
	for (; ii<nn; ++ii)
	{
		Node* cd = &m_nodemem[ ci + ii];
		if (cd->m_parent != (int)mi)
		{
			cd->m_parent = (int)mi;
			setParentLinks( ci + ii);
		}
	}
}

const std::string TransactionFunctionInput::Structure::tostring() const
{
	std::vector <std::pair< std::size_t, std::size_t> > stk;
	std::ostringstream rt;
	stk.push_back( std::pair< std::size_t, std::size_t>( m_root.m_elementsize, m_root.childidx()));
	while (stk.size())
	{
		std::size_t ii = stk.back().first;
		std::size_t mi = stk.back().second;
		--stk.back().first;
		if (ii>0)
		{
			const Node* cd = &m_nodemem[ mi + ii - 1];
			if (cd->valueidx())
			{
				std::size_t indent = stk.size() -1;
				const types::Variant* val = &m_content.at( cd->m_element);
				while (indent--) rt << '\t';
				if (cd->m_tag) rt << cd->m_tag << ": ";
				rt << "'" << val->tostring() << "'" << std::endl;
			}
			else
			{
				std::size_t indent = stk.size() -1;
				while (indent--) rt << '\t';
				rt << cd->m_tag << ":" << std::endl;
				stk.push_back( std::pair< std::size_t, std::size_t>( cd->nofchild(), cd->childidx()));
			}
		}
		else
		{
			stk.pop_back();
		}
	}
	return rt.str();
}

void TransactionFunctionInput::Structure::openTag( const char* tag, std::size_t tagsize)
{
	const std::string tagstr( tag, tagsize);
	openTag( tagstr);
}

void TransactionFunctionInput::Structure::openTag( const std::string& tagstr)
{
	int mi = (int)m_tagmap->find( tagstr);
	if (mi == 0) mi = (int)m_tagmap->unused();

	int tagstr_ = m_strings.size();
	m_strings.append( tagstr);
	m_strings.push_back( '\0');
	m_data.back().push_back( Node( 0, mi, tagstr_, 0, 0));
	m_data.push_back( std::vector<Node>());
}

void TransactionFunctionInput::Structure::createRootNode()
{
	std::size_t rootsize = m_data.back().size();
	std::size_t rootidx = m_nodemem.alloc( rootsize);
	Node* nd = &m_nodemem[ rootidx];
	m_root.m_elementsize = rootsize;
	m_root.m_element = Node::ref_element(rootidx);
	std::vector<Node>::const_iterator itr = m_data.back().begin(), end = m_data.back().end();
	for (; itr != end; ++itr, ++nd)
	{
		*nd = *itr;
	}
}

void TransactionFunctionInput::Structure::closeTag()
{
	if (m_data.size() == 1)
	{
		throw std::runtime_error( "tags in input not balanced");
	}
	if (m_data.back().size() == 0)
	{
		pushValue( types::Variant());
	}
	createRootNode();
	m_data.pop_back();
	m_data.back().back().m_elementsize = m_root.m_elementsize;
	m_data.back().back().m_element = m_root.m_element;

	if (m_data.size() == 1)
	{
		// top level tag closed, assuming end of structure,
		// creating root node and set all parent links:
		createRootNode();
		std::size_t rootidx = m_root.childidx();
		for (int ri=0; ri<m_root.m_elementsize; ++ri)
		{
			setParentLinks( rootidx+ri);
		}
	}
}

void TransactionFunctionInput::Structure::pushValue( const types::VariantConst& val)
{
	std::size_t mi = m_content.size();
	m_content.push_back( val);
	m_data.back().push_back( Node( 0, 0, 0, 0, Node::val_element( mi)));
}

void TransactionFunctionInput::Structure::next( const Node* nd, int tag, std::vector<const Node*>& nextnd) const
{
	std::size_t ii = 0, nn = nd->nofchild(), idx = nd->childidx();
	if (nn)
	{
		const Node* cd = &m_nodemem[ idx];
		for (; ii<nn; ++ii)
		{
			if (cd[ii].m_tag)
			{
				if (!tag || cd[ii].m_tag == tag)
				{
					nextnd.push_back( cd+ii);
				}
			}
		}
	}
}

void TransactionFunctionInput::Structure::find( const Node* nd, int tag, std::vector<const Node*>& findnd) const
{
	std::size_t ii = 0, nn = nd->nofchild(), idx = nd->childidx();
	if (nn)
	{
		const Node* cd = &m_nodemem[ idx];
		for (; ii<nn; ++ii)
		{
			if (cd[ii].m_tag)
			{
				if (!tag || cd[ii].m_tag == tag)
				{
					findnd.push_back( cd+ii);
				}
			}
			if (nd->childidx())
			{
				find( cd+ii, tag, findnd);
			}
		}
	}
}

void TransactionFunctionInput::Structure::up( const Node* nd, std::vector<const Node*>& rt) const
{
	if (nd->m_parent != 0)
	{
		rt.push_back( &m_nodemem[ nd->m_parent]);
	}
	else
	{
		throw std::runtime_error( "selecting /.. from root");
	}
}

const TransactionFunctionInput::Structure::Node* TransactionFunctionInput::Structure::root() const
{
	return &m_root;
}

const TransactionFunctionInput::Structure::Node* TransactionFunctionInput::Structure::child( const Node* nd, int idx) const
{
	if (idx >= nd->m_elementsize) return 0;
	const Node* rt = &m_nodemem[ nd->childidx() + idx];
	return rt;
}

const types::Variant* TransactionFunctionInput::Structure::contentvalue( const Node* nd) const
{
	if (!nd->m_tag)
	{
		std::size_t validx = nd->valueidx();
		if (validx) return &m_content.at( validx);
	}
	return 0;
}

const types::Variant* TransactionFunctionInput::Structure::nodevalue( const Node* nd) const
{
	const types::Variant* rt = 0;
	std::size_t ii = 0, nn = nd->nofchild(), idx = nd->childidx();
	if (nn)
	{
		const Node* cd = &m_nodemem[ idx];
		for (; ii<nn; ++ii)
		{
			if (!cd[ii].m_tag)
			{
				if (rt) throw std::runtime_error( "node selected has more than one value");
				std::size_t validx = cd[ii].valueidx();
				if (validx) rt = &m_content.at( validx);
			}
		}
	}
	return rt;
}

const char* TransactionFunctionInput::Structure::tagname( int tagstridx) const
{
	return (tagstridx>0) ? (m_strings.c_str() + tagstridx) : 0;
}

bool TransactionFunctionInput::Structure::isequalTag( const std::string& t1, const std::string& t2) const
{
	if (m_tagmap->case_sensitive())
	{
		return boost::algorithm::iequals( t1, t2);
	}
	else
	{
		return t1 == t2;
	}
}


namespace {
///\class Filter
///\brief Input filter for calling preprocessing functions
class Filter
	:public langbind::TypedInputFilter
{
public:
	typedef TransactionFunctionInput::Structure::NodeAssignment NodeAssignment;
	typedef TransactionFunctionInput::Structure::Node Node;

	Filter()
		:types::TypeSignature("db::TransactionFunctionInput::Structure::Filter", __LINE__)
		,m_structure(0){}

	Filter( const Filter& o)
		:types::TypeSignature("db::TransactionFunctionInput::Structure::Filter", __LINE__)
		,langbind::TypedInputFilter(o)
		,m_structure(o.m_structure)
		,m_noderenames(o.m_noderenames)
		,m_stack(o.m_stack){}

	Filter( const TransactionFunctionInput::Structure* structure_, const Node* node_, const std::vector<NodeAssignment>& noderenames_)
		:types::TypeSignature("db::TransactionFunctionInput::Structure::Filter", __LINE__)
		,m_structure(structure_)
		,m_noderenames(noderenames_)
	{
		m_stack.push_back( node_);
	}

	virtual ~Filter(){}
	virtual TypedInputFilter* copy() const		{return new Filter( *this);}

	virtual bool getNext( ElementType& type, types::VariantConst& element)
	{
		if (m_stack.empty())
		{
			setState( langbind::InputFilter::Error, "internal: call of input filter after close");
			return false;
		}
		for (;;)
		{
			const Node* chld = m_structure->child( m_stack.back().node, m_stack.back().idx);
			const char* tagnamestr;
			if (chld)
			{
				m_stack.pop_back();
				type = TypedInputFilter::CloseTag;
				element.init();
				return true;
			}
			else if ((tagnamestr = m_structure->tagname( chld->m_tagstr)) != 0)
			{
				++m_stack.back().idx;
				m_stack.push_back( chld);
				type = TypedInputFilter::OpenTag;

				if (m_stack.size() == 1 && !m_noderenames.empty())
				{
					std::vector<NodeAssignment>::const_iterator ti = m_noderenames.begin(), te = m_noderenames.end();
					for (; ti != te; ++ti)
					{
						if (m_structure->isequalTag( ti->first, tagnamestr))
						{
							element = ti->second;
							return true;
						}
					}
				}
				element = tagnamestr;
				return true;
			}
			else
			{
				std::size_t validx = m_stack.back().node->valueidx();
				if (validx)
				{
					++m_stack.back().idx;
					type = TypedInputFilter::Value;
					element = m_structure->contentvalue( m_stack.back().node);
					return true;
				}
			}
			++m_stack.back().idx;
		}
	}

private:
	struct StackElement
	{
		const Node* node;
		int idx;
		ElementType lasttype;

		StackElement()
			:idx(0),lasttype(TypedInputFilter::CloseTag){}
		StackElement( const StackElement& o)
			:node(o.node),idx(o.idx),lasttype(o.lasttype){}
		StackElement( const Node* node_)
			:node(node_),idx(0),lasttype(TypedInputFilter::CloseTag){}
	};

	const TransactionFunctionInput::Structure* m_structure;
	std::vector<NodeAssignment> m_noderenames;
	std::vector<StackElement> m_stack;
};
}//namespace

langbind::TypedInputFilter* TransactionFunctionInput::Structure::createFilter( const Node* node, const std::vector<NodeAssignment>& noderenames) const
{
	langbind::TypedInputFilter* rt = new Filter( this, node, noderenames);
	if (!m_tagmap->case_sensitive())
	{
		rt->setFlags( langbind::TypedInputFilter::PropagateNoCase);
	}
	return rt;
}




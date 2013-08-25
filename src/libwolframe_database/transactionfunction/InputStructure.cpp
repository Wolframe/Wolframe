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
	,m_privatetagmap(o.m_privatetagmap)
	,m_root(o.m_root)
	,m_data(o.m_data)
	{}

TransactionFunctionInput::Structure::Structure( const TagTable* tagmap)
	:m_tagmap(tagmap),m_privatetagmap(tagmap->case_sensitive())
{
	m_nodemem.alloc( 1);
	m_content.push_back( types::Variant());
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

void TransactionFunctionInput::Structure::openTag( const types::Variant& tag)
{
	if (tag.type() == types::Variant::string_)
	{
		std::string tagstr( tag.tostring());
		int tag_ = (int)m_tagmap->find( tagstr);
		if (tag_ == 0) tag_ = (int)m_tagmap->unused();
		int tagstr_ = (int)m_privatetagmap.get( tagstr);
		m_data.back().push_back( Node( 0, tag_, tagstr_, 0, 0));
		m_data.push_back( std::vector<Node>());
	}
	else
	{
		int arrayindex;
		try
		{
			arrayindex = (int)tag.toint();
		}
		catch (const std::runtime_error& e)
		{
			throw std::runtime_error( "array index cannot be converted to integer");
		}
		if (arrayindex < 0)
		{
			throw std::runtime_error( "array index is negative");
		}
		if (m_data.size() == 1)
		{
			m_data.back().push_back( Node( 0, 0, 0, 0, 0));
		}
		else
		{
			const Node& pred = m_data.at( m_data.size()-2).back();
			m_data.back().push_back( Node( 0, pred.m_tag, pred.m_tagstr, 0, 0));
		}
		m_data.back().back().m_arrayindex = arrayindex;
		m_data.push_back( std::vector<Node>());
	}
}

bool TransactionFunctionInput::Structure::isArrayNode() const
{
	int arrayindex = -1;
	std::vector<Node>::const_iterator ni = m_data.back().begin(), ne = m_data.back().end();
	for (; ni != ne; ++ni)
	{
		if (ni->m_arrayindex <= arrayindex) return false;
		arrayindex = ni->m_arrayindex;
	}
	return true;
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
	if (isArrayNode() && m_data.size() >= 2)
	{
		// ... in case of an array the owner of the array takes over the elements of the array
		std::vector<Node>& uu = m_data.at( m_data.size()-2);
		std::vector<Node>& tt = m_data.at( m_data.size()-1);
		std::vector<Node>::const_iterator ti = tt.begin(), te = tt.end();
		for (; ti != te; ++ti) uu.push_back( *ti);
		m_data.pop_back();
	}
	else
	{
		if (m_data.back().size() == 0)
		{
			pushValue( types::Variant());
		}
		createRootNode();
		m_data.pop_back();
		m_data.back().back().m_elementsize = m_root.m_elementsize;
		m_data.back().back().m_element = m_root.m_element;
	}

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
	return m_privatetagmap.getstr( tagstridx);
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
		,m_nodelist(o.m_nodelist)
		,m_stack(o.m_stack)
		,m_elementBuf(o.m_elementBuf)
	{
		m_nodeitr = m_nodelist.begin() + (o.m_nodeitr - o.m_nodelist.begin());
	}

	Filter( const TransactionFunctionInput::Structure* structure_, const std::vector<NodeAssignment>& nodelist_)
		:types::TypeSignature("db::TransactionFunctionInput::Structure::Filter", __LINE__)
		,m_structure(structure_)
		,m_nodelist(nodelist_)
	{
		m_nodeitr = m_nodelist.begin();
	}

	virtual ~Filter(){}
	virtual TypedInputFilter* copy() const		{return new Filter( *this);}

	virtual bool getNext( ElementType& type, types::VariantConst& element)
	{
		if (!m_elementBuf.empty())
		{
			type = m_elementBuf.back().first;
			element = m_elementBuf.back().second;
			m_elementBuf.pop_back();
			return true;
		}
		if (m_stack.empty())
		{
			if (!m_structure)
			{
				// ... calling getNext after final CloseTag
				setState( langbind::InputFilter::Error, "internal: illegal call of input filter");
				return false;
			}
			if (m_nodeitr == m_nodelist.end())
			{
				// ... end of content marker
				type = TypedInputFilter::CloseTag;
				element.init();
				m_structure = 0;
				return true;
			}
			else
			{
				// ... skip to next element in the node list
				m_stack.push_back( m_nodeitr->second);
				type = TypedInputFilter::OpenTag;
				element = m_nodeitr->first;
				++m_nodeitr;
				return true;
			}
		}
		for (;;)
		{
			const Node* chld = m_structure->child( m_stack.back().node, m_stack.back().idx);
			const char* tagnamestr;
			if (!chld)
			{
				if (flag( SerializeWithIndices) && m_stack.back().idx > 0)
				{
					// ... We have to close additional array indices added to array element tags
					const Node* prevchld = m_structure->child( m_stack.back().node, m_stack.back().idx-1);
					if (prevchld && prevchld->m_arrayindex >= 0)
					{
						// ... after array element at end of structure add missing: Close <index>
						m_elementBuf.push_back( Element( TypedInputFilter::CloseTag, types::Variant()));
					}
				}
				m_stack.pop_back();
				type = TypedInputFilter::CloseTag;
				element.init();
				return true;
			}
			else if ((tagnamestr = m_structure->tagname( chld->m_tagstr)) != 0)
			{
				if (flag( SerializeWithIndices))
				{
					// ... We have to add array indices to array element tags
					if (chld->m_arrayindex >= 0)
					{
						// ... [A] element of an array
						if (chld->m_arrayindex > m_stack.back().lastarrayindex)
						{
							if (m_stack.back().lastarrayindex == -1)
							{
								// ... [A.1] new array -> Open <name>; Open <index>
								m_elementBuf.push_back( Element( TypedInputFilter::OpenTag, chld->m_arrayindex));
								type = TypedInputFilter::OpenTag;
								element = tagnamestr;
							}
							else
							{
								// ... [A.1] next array element of open array -> Close <index>; Open <index>
								m_elementBuf.push_back( Element( TypedInputFilter::OpenTag, chld->m_arrayindex));
								type = TypedInputFilter::CloseTag;
								element.init();
							}
						}
						else
						{
							// ... [A.3] new array following array -> Close previous array before opening new one: Close <index>; Close <name>; Open <name>; Open <index>
							m_elementBuf.push_back( Element( TypedInputFilter::OpenTag, chld->m_arrayindex));
							m_elementBuf.push_back( Element( TypedInputFilter::OpenTag, tagnamestr));
							m_elementBuf.push_back( Element( TypedInputFilter::CloseTag, types::Variant()));
							type = TypedInputFilter::CloseTag;
							element.init();
						}
						++m_stack.back().idx;
						m_stack.back().lastarrayindex = chld->m_arrayindex;
						m_stack.push_back( chld);
						return true;
					}
					else
					{
						// ... [S] single element (not an array element)
						if (m_stack.back().lastarrayindex >= 0)
						{
							// ... [S.1] last element belonged to an array: Close (2nd); Open <name>
							m_elementBuf.push_back( Element( TypedInputFilter::OpenTag, tagnamestr));
							m_stack.back().lastarrayindex = -1;
							type = TypedInputFilter::CloseTag;
							element.init();
						}
						else
						{
							// ... [S.2] last element was also a single element
							type = TypedInputFilter::OpenTag;
							element = tagnamestr;
						}
						++m_stack.back().idx;
						m_stack.push_back( chld);
						return true;
					}
				}
				else
				{
					++m_stack.back().idx;
					m_stack.push_back( chld);
					type = TypedInputFilter::OpenTag;
					element = tagnamestr;
					return true;
				}
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
		int lastarrayindex;
		ElementType lasttype;

		StackElement()
			:idx(0),lastarrayindex(-1),lasttype(TypedInputFilter::CloseTag){}
		StackElement( const StackElement& o)
			:node(o.node),idx(o.idx),lastarrayindex(o.lastarrayindex),lasttype(o.lasttype){}
		StackElement( const Node* node_)
			:node(node_),idx(0),lastarrayindex(-1),lasttype(TypedInputFilter::CloseTag){}
	};

	const TransactionFunctionInput::Structure* m_structure;
	std::vector<NodeAssignment> m_nodelist;
	std::vector<NodeAssignment>::const_iterator m_nodeitr;
	std::vector<StackElement> m_stack;
	typedef std::pair<ElementType, types::VariantConst> Element;
	std::vector<Element> m_elementBuf;
};
}//namespace

langbind::TypedInputFilter* TransactionFunctionInput::Structure::createFilter( const std::vector<NodeAssignment>& nodelist) const
{
	langbind::TypedInputFilter* rt = new Filter( this, nodelist);
	if (!m_tagmap->case_sensitive())
	{
		rt->setFlags( langbind::TypedInputFilter::PropagateNoCase);
	}
	return rt;
}




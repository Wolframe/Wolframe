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
#include "logger/logger-v1.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

TransactionFunctionInput::Structure::Structure( const Structure& o)
	:m_nodemem(o.m_nodemem)
	,m_content(o.m_content)
	,m_tagmap(o.m_tagmap)
	,m_privatetagmap(o.m_privatetagmap)
	,m_visitor(o.m_visitor)
	{}

TransactionFunctionInput::Structure::Structure( const TagTable* tagmap)
	:m_tagmap(tagmap),m_privatetagmap(tagmap->case_sensitive())
{
	m_nodemem.alloc( 1);
	Node* nd = m_nodemem.base();
	nd->m_arrayindex = -1;
	nd->m_parent = -1;
	m_content.push_back( types::Variant());
}

const TransactionFunctionInput::Structure::Node*
	TransactionFunctionInput::Structure::node( const NodeVisitor& nv) const
{
	return &m_nodemem[ nv.m_nodeidx];
}

TransactionFunctionInput::Structure::Node*
	TransactionFunctionInput::Structure::node( const NodeVisitor& nv)
{
	return &m_nodemem[ nv.m_nodeidx];
}


const std::string TransactionFunctionInput::Structure::tostring( const NodeVisitor& nv) const
{
	std::vector <int> stk;
	std::ostringstream rt;
	stk.push_back( nv.m_nodeidx);

	while (stk.size())
	{
		std::size_t ni = stk.back();
		if (!ni)
		{
			stk.pop_back();
			continue;
		}
		const Node* nd = node( ni);
		std::size_t indent = stk.size() -1;

		stk.back() = nd->m_next;
		if (nd->m_value)
		{
			const types::Variant* val = &m_content.at( nd->m_value);
			while (indent--) rt << '\t';
			if (nd->m_tag) rt << nd->m_tag << " ";
			if (nd->m_tagstr) rt << tagname( nd) << ": ";
			rt << "'" << val->tostring() << "'" << std::endl;
		}
		else
		{
			while (indent--) rt << '\t';
			if (nd->m_tag) rt << nd->m_tag << " ";
			if (nd->m_tagstr) rt << tagname(nd) << ": ";
			rt << std::endl;
			stk.back() = nd->m_next;
			if (nd->m_firstchild)
			{
				stk.push_back( nd->m_firstchild);
			}
		}
	}
	return rt.str();
}

TransactionFunctionInput::Structure::NodeVisitor
	TransactionFunctionInput::Structure::createChildNode( const NodeVisitor& visitor)
{
	Node* nd = node( visitor);
	int idx = (int)m_nodemem.alloc( 1);
	if (!nd->m_lastchild)
	{
		nd->m_lastchild = nd->m_firstchild = idx;
	}
	else
	{
		Node* lc = node( nd->m_lastchild);
		lc->m_next = nd->m_lastchild = idx;
	}
	Node* chld = node( idx);
	chld->m_arrayindex = -1;
	chld->m_parent = visitor.m_nodeidx;
	return NodeVisitor(idx);
}

TransactionFunctionInput::Structure::NodeVisitor
	TransactionFunctionInput::Structure::createSiblingNode( const NodeVisitor& visitor)
{
	if (!visitor.m_nodeidx) throw std::runtime_error( "try to create sibling node of root");
	return createChildNode( NodeVisitor( node( visitor.m_nodeidx)->m_parent));
}


TransactionFunctionInput::Structure::NodeVisitor
	TransactionFunctionInput::Structure::openTag( const NodeVisitor& visitor, const types::Variant& tag)
{
	TransactionFunctionInput::Structure::NodeVisitor rt = createChildNode( visitor);
	Node* nd = node( rt);

	if (tag.type() == types::Variant::string_)
	{
		std::string tagstr( tag.tostring());
		nd->m_tag = (int)m_tagmap->find( tagstr);
		if (nd->m_tag == 0) nd->m_tag = (int)m_tagmap->unused();
		nd->m_tagstr = (int)m_privatetagmap.get( tagstr);
	}
	else
	{
		try
		{
			nd->m_arrayindex = (int)tag.toint();
		}
		catch (const std::runtime_error& e)
		{
			throw std::runtime_error( "array index cannot be converted to integer");
		}
		if (nd->m_arrayindex < 0)
		{
			throw std::runtime_error( "array index is negative");
		}
		if (nd->m_parent)
		{
			Node* pd = node( nd->m_parent);
			nd->m_tag = pd->m_tag;
			nd->m_tagstr = pd->m_tagstr;
		}
	}
	return rt;
}

bool TransactionFunctionInput::Structure::isArrayNode( const NodeVisitor& visitor) const
{
	const Node* nd = node( visitor);
	if (!nd->m_firstchild) return false;
	const Node* cd = node( nd->m_firstchild);
	int arrayindex = -1;
	for (;;)
	{
		if (cd->m_arrayindex <= arrayindex) return false;
		arrayindex = cd->m_arrayindex;

		if (!cd->m_next) break;
		cd = node( cd->m_next);
	}
	return true;
}

TransactionFunctionInput::Structure::NodeVisitor
	TransactionFunctionInput::Structure::closeTag( const NodeVisitor& visitor)
{
	Node* nd = node( visitor);
	if (nd->m_parent < 0) throw std::runtime_error( "tags not balanced in input (close tag)");

	NodeVisitor rt( nd->m_parent);
	if (isArrayNode( visitor) && nd->m_parent != 0)
	{
		// In case of an array the granparent of the array parent takes over the children of the array
		// and the parent is deleted from the tree:
		Node* cd = node( nd->m_firstchild);
		for (;;)
		{
			cd->m_tag = nd->m_tag;
			cd->m_tagstr = nd->m_tagstr;	//... tag names taken from father that will disappear
			if (!cd->m_next) break;
			cd = node( cd->m_next);
		}
		Node* pn = node( nd->m_parent);

		if (visitor.m_nodeidx != pn->m_lastchild)
		{
			throw std::logic_error("internal: illegal call of closeTag");
		}
		if (pn->m_firstchild == pn->m_lastchild)
		{
			pn->m_firstchild = visitor.m_nodeidx;
		}
		pn->m_lastchild = nd->m_lastchild;
		*nd = *node( nd->m_firstchild);		// ... first child gets joined to granparent children and father disappears
	}
	return rt;
}

void TransactionFunctionInput::Structure::pushValue( const NodeVisitor& visitor, const types::VariantConst& val)
{
	Node* nd = node( visitor);
	if (nd->m_value) throw std::runtime_error( "multiple values assigned to one node in the data tree");

	nd->m_value = m_content.size();
	m_content.push_back( val);
}


void TransactionFunctionInput::Structure::next( const Node* nd, int tag, std::vector<const Node*>& nextnd) const
{
	if (!nd->m_firstchild) return;
	const Node* cd = node( nd->m_firstchild);
	if (!tag)
	{
		for (;;)
		{
			nextnd.push_back( cd);
			if (!cd->m_next) break;
			cd = node( cd->m_next);
		}
	}
	else
	{
		for (;;)
		{
			if (cd->m_tag == tag) nextnd.push_back( cd);
			if (!cd->m_next) break;
			cd = node( cd->m_next);
		}
	}
}

void TransactionFunctionInput::Structure::find( const Node* nd, int tag, std::vector<const Node*>& findnd) const
{
	if (!nd->m_firstchild) return;
	const Node* cd = node( nd->m_firstchild);
	if (!tag)
	{
		for (;;)
		{
			findnd.push_back( cd);
			if (cd->m_firstchild) find( cd, tag, findnd);
			if (!cd->m_next) break;
			cd = node( cd->m_next);
		}
	}
	else
	{
		for (;;)
		{
			if (cd->m_tag == tag) findnd.push_back( cd);
			if (cd->m_firstchild) find( cd, tag, findnd);
			if (!cd->m_next) break;
			cd = node( cd->m_next);
		}
	}
}

void TransactionFunctionInput::Structure::up( const Node* nd, std::vector<const Node*>& rt) const
{
	if (nd->m_parent != -1)
	{
		rt.push_back( node( nd->m_parent));
	}
	else
	{
		throw std::runtime_error( "selecting /.. from root");
	}
}

const TransactionFunctionInput::Structure::Node*
	TransactionFunctionInput::Structure::root() const
{
	return node(0);
}

const types::Variant* TransactionFunctionInput::Structure::contentvalue( const Node* nd) const
{
	if (nd->m_value)
	{
		return &m_content.at( nd->m_value);
	}
	return 0;
}

const char* TransactionFunctionInput::Structure::tagname( const Node* nd) const
{
	return m_privatetagmap.getstr( nd->m_tagstr);
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

	///\brief Default constructor
	Filter()
		:types::TypeSignature("db::TransactionFunctionInput::Structure::Filter", __LINE__)
		,m_structure(0)
		,m_elementitr(0)
	{
		m_nodeitr = m_nodelist.begin();
	}

	///\brief Copy constructor
	Filter( const Filter& o)
		:types::TypeSignature("db::TransactionFunctionInput::Structure::Filter", __LINE__)
		,langbind::TypedInputFilter(o)
		,m_structure(o.m_structure)
		,m_nodelist(o.m_nodelist)
		,m_stack(o.m_stack)
		,m_elementbuf(o.m_elementbuf)
		,m_elementitr(o.m_elementitr)
	{
		m_nodeitr = m_nodelist.begin() + (o.m_nodeitr - o.m_nodelist.begin());
	}

	///\brief Constructor
	Filter( const TransactionFunctionInput::Structure* structure_, const std::vector<NodeAssignment>& nodelist_)
		:types::TypeSignature("db::TransactionFunctionInput::Structure::Filter", __LINE__)
		,m_structure(structure_)
		,m_nodelist(nodelist_)
		,m_elementitr(0)
	{
		m_nodeitr = m_nodelist.begin();
	}

	virtual ~Filter(){}

	///\brief Implementation of TypedInputFilter::copy()
	virtual TypedInputFilter* copy() const		{return new Filter( *this);}

	///\brief Implementation of TypedInputFilter::getNext(ElementType&,types::VariantConst&)
	virtual bool getNext( ElementType& type, types::VariantConst& element)
	{
		for (;;)
		{
			// return buffered elements first:
			if (m_elementitr < m_elementbuf.size())
			{
				std::pair<ElementType, types::VariantConst>& ee = m_elementbuf.at( m_elementitr++);
				type = ee.first;
				element = ee.second;
				if (m_elementitr == m_elementbuf.size())
				{
					m_elementbuf.clear();
				}
				return true;
			}
			// fetch new parameter node if the current one is finished:
			if (m_stack.empty())
			{
				if (!m_structure)
				{
					// ... calling getNext after final CloseTag
					setState( langbind::InputFilter::Error, "internal: illegal call of input filter");
					return false;
				}
				while (m_nodeitr != m_nodelist.end() && !m_nodeitr->second)
				{
					//... skip empty nodes (null values)
					++m_nodeitr;
				}
				if (m_nodeitr == m_nodelist.end())
				{
					// ... end of content marker (final close- EoD)
					m_structure = 0;
					type = TypedInputFilter::CloseTag;
					element.init();
					return true;
				}
				// ... push the next node in the list to process
				m_stack.push_back( StackElement( m_nodeitr->second, false));
				if (!m_nodeitr->first.empty())
				{
					// ... only open non empty tags
					type = TypedInputFilter::OpenTag;
					element = m_nodeitr->first;
					++ m_stack.back().closetagcnt;
					++ m_nodeitr;
					return true;
				}
				else
				{
					// ... empty tag = embedded content, do not open anclosing tag
					++m_nodeitr;
				}
				// ! The case that the parameter node is an array node (m_arrayindex) is not respected.
				//	It is treated as a single node.
			}
			// Structure fetching statemachine:
			if (m_stack.back().node)
			{
				if (!m_stack.back().childrenvisited && m_stack.back().node->m_firstchild)
				{
					// mark children as visited:
					m_stack.back().childrenvisited = true;

					// push children (first child node) on the stack:
					const Node* cnod = m_structure->node( m_stack.back().node->m_firstchild);
					type = TypedInputFilter::OpenTag;
					element = m_structure->tagname( cnod);
					m_stack.push_back( StackElement( cnod, true));
					++ m_stack.back().closetagcnt;

					if (cnod->m_arrayindex >= 0)
					{
						++ m_stack.back().closetagcnt;
						m_elementbuf.push_back( Element( TypedInputFilter::OpenTag, cnod->m_arrayindex));
					}
					return true;
				}
				if (m_stack.back().node->m_next && m_stack.back().visitnext)
				{
					// replace current node on the stack by its next neighbour
					const Node* snod = m_structure->node( m_stack.back().node->m_next);
					if (flag( SerializeWithIndices))
					{
						const Node* tnod = m_stack.back().node;
						if (snod->m_arrayindex >= 0)
						{
							// ... element is in array
							if (snod->m_arrayindex > tnod->m_arrayindex
								&&  snod->m_tag == tnod->m_tag
								&&  snod->m_tagstr == tnod->m_tagstr)
							{
								// ... next element is in same array than element before: close <index>; open <index>
								type = TypedInputFilter::CloseTag; element.init();
								m_elementbuf.push_back( Element( TypedInputFilter::OpenTag, snod->m_arrayindex));
							}
							else if (tnod->m_arrayindex == -1)
							{
								// ... new array after single element: close <tag>; open <tag>; open <index>
								type = TypedInputFilter::CloseTag; element.init();
								m_elementbuf.push_back( Element( TypedInputFilter::OpenTag, m_structure->tagname( snod)));
								m_elementbuf.push_back( Element( TypedInputFilter::OpenTag, snod->m_arrayindex));
								++ m_stack.back().closetagcnt;
							}
							else
							{
								// ... new array after array element of previous array: close <index>; close <tag>; open <tag>; open <index>
								type = TypedInputFilter::CloseTag; element.init();
								m_elementbuf.push_back( Element( TypedInputFilter::CloseTag, types::Variant()));
								m_elementbuf.push_back( Element( TypedInputFilter::OpenTag, m_structure->tagname( snod)));
								m_elementbuf.push_back( Element( TypedInputFilter::OpenTag, snod->m_arrayindex));
							}
						}
						else if (tnod->m_arrayindex >= 0)
						{
							// ... new single element after array: close <index>; close <tag>; open <tag>
							type = TypedInputFilter::CloseTag; element.init();
							m_elementbuf.push_back( Element( TypedInputFilter::CloseTag, types::Variant()));
							m_elementbuf.push_back( Element( TypedInputFilter::OpenTag, m_structure->tagname( snod)));
							-- m_stack.back().closetagcnt;
						}
						else
						{
							// ... new single element after single element: close <tag>; open <tag>
							type = TypedInputFilter::CloseTag; element.init();
							m_elementbuf.push_back( Element( TypedInputFilter::OpenTag, m_structure->tagname( snod)));
						}
					}
					else//if (!flag( SerializeWithIndices))
					{
						// ... any case is the same: close <tag>; open <tag>
						type = TypedInputFilter::CloseTag; element.init();
						m_elementbuf.push_back( Element( TypedInputFilter::OpenTag, m_structure->tagname( snod)));
					}
					m_stack.back().node = snod;			//... skip to next element
					m_stack.back().childrenvisited = false;		//... and its children
					return true;
				}
				if (m_stack.back().closetagcnt > 1)
				{
					// close element related tag (e.g. index tag):
					type = TypedInputFilter::CloseTag; element.init();
					-- m_stack.back().closetagcnt;
					return true;
				}
				const types::Variant* val = m_structure->contentvalue( m_stack.back().node);
				if (val)
				{
					// fetch content value after all children and neighbour nodes
					element = *val;
					type = TypedInputFilter::Value;
					return true;
				}
			}
			while (m_stack.back().closetagcnt > 0)
			{
				// close element enclosing tags:
				m_elementbuf.push_back( Element( TypedInputFilter::CloseTag, types::Variant()));
				-- m_stack.back().closetagcnt;
			}
			m_stack.pop_back();
			continue;
		}
	}

private:
	struct StackElement
	{
		const Node* node;
		bool childrenvisited;
		bool visitnext;
		int closetagcnt;

		StackElement()
			:node(0),childrenvisited(false),visitnext(false),closetagcnt(0){}
		StackElement( const StackElement& o)
			:node(o.node),childrenvisited(o.childrenvisited),visitnext(o.visitnext),closetagcnt(o.closetagcnt){}
		StackElement( const Node* node_, bool visitnext_)
			:node(node_),childrenvisited(false),visitnext(visitnext_),closetagcnt(0){}
	};

	const TransactionFunctionInput::Structure* m_structure;		//< structure to get the elements from
	std::vector<NodeAssignment> m_nodelist;				//< list of element nodes in the structure
	std::vector<NodeAssignment>::const_iterator m_nodeitr;		//< iterator on elements nodes of the structure
	std::vector<StackElement> m_stack;				//< visit tree hierarchy stack
	typedef std::pair<ElementType, types::VariantConst> Element;	//< element for buffer
	std::vector<Element> m_elementbuf;				//< buffer for elements in states producing more than one element
	std::size_t m_elementitr;					//< iterator on elements in buffer
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




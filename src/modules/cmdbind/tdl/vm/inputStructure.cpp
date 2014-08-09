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
///\brief Implementation of the input data structure for transaction functions
///\file vm/inputStructure.cpp

#include "inputStructure.hpp"
#include "filter/typedfilter.hpp"
#include "utils/printFormats.hpp"
#include "logger-v1.hpp"
#include <boost/algorithm/string.hpp>
#include <iostream>

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::db::vm;

InputStructure::InputStructure( const InputStructure& o)
	:m_nodemem(o.m_nodemem)
	,m_content(o.m_content)
	,m_tagmap(o.m_tagmap)
	,m_privatetagmap(o.m_privatetagmap)
	,m_visitor(o.m_visitor)
	,m_done(o.m_done)
	{}

InputStructure::InputStructure( const TagTable* tagmap)
	:m_tagmap(tagmap),m_privatetagmap(tagmap->case_sensitive()),m_done(false)
{
	m_nodemem.alloc( 1);
	InputNode* nd = m_nodemem.base();
	nd->m_arrayindex = -1;
	nd->m_parent = -1;
	m_content.push_back( types::Variant());
}

const InputNode* InputStructure::node( const InputNodeVisitor& nv) const
{
	return &m_nodemem[ nv.m_nodeidx];
}

InputNode* InputStructure::node( const InputNodeVisitor& nv)
{
	return &m_nodemem[ nv.m_nodeidx];
}

namespace {
struct StackElement
{
	int nodeidx;
	bool childrenvisited;
	bool headerprinted;
	bool printsiblings;
	StackElement()
		:nodeidx(0),childrenvisited(false),headerprinted(false),printsiblings(true){}
	StackElement( int nodeidx_)
		:nodeidx(nodeidx_),childrenvisited(false),headerprinted(false),printsiblings(true){}
	StackElement( const StackElement& o)
		:nodeidx(o.nodeidx),childrenvisited(o.childrenvisited),headerprinted(o.headerprinted),printsiblings(o.printsiblings){}
};
}//anonymous namespace

bool InputStructure::check( const InputNodeVisitor& nv) const
{
	bool rt = true;
	const InputNode* nd = node( nv);
	if (nv.m_nodeidx)
	{
		int ci = nv.m_nodeidx;
		for (; node(ci)->m_next != 0; ci=node(ci)->m_next){}

		const InputNode* pn = node( nd->m_parent);
		if (ci != pn->m_lastchild)
		{
			std::cout << "HALLY GALLY AT " << (int)__LINE__ << " DIFF " << nv.m_nodeidx << " " << pn->m_lastchild << std::endl;
			rt = false;
		}
	}
	if (nd->m_firstchild)
	{
		rt &= check( InputNodeVisitor( nd->m_firstchild));
	}
	return rt;
}

void InputStructure::print( std::ostream& out, const utils::PrintFormat* pformat, const InputNodeVisitor& nv) const
{
	std::vector <StackElement> stk;
	stk.push_back( nv.m_nodeidx);
	stk.back().headerprinted = true;
	stk.back().printsiblings = false;

	while (stk.size())
	{
		const InputNode* nd = node( stk.back().nodeidx);
		if (!stk.back().headerprinted)
		{
			out << pformat->newitem;
			std::size_t indent = stk.size() -1;
			while (indent--) out << pformat->indent;

			if (nd->m_tagstr) out << tagname(nd) << pformat->endheader;
			stk.back().headerprinted = true;
		}
		if (nd->m_firstchild && !stk.back().childrenvisited)
		{
			stk.back().childrenvisited = true;
			out << pformat->openstruct;
			stk.push_back( nd->m_firstchild);
			continue;
		}
		if (nd->m_value)
		{
			out << pformat->newitem;
			std::size_t indent = stk.size() -1;
			while (indent--) out << pformat->indent;

			const types::Variant* val = &m_content.at( nd->m_value);
			out << val->typeName() << pformat->itemdelimiter << pformat->startvalue;
			if (pformat->maxitemsize)
			{
				out << utils::getLogString( *val, pformat->maxitemsize);
			}
			else
			{
				out << val->tostring();
			}
			out << pformat->endvalue;
		}
		if (stk.back().printsiblings && nd->m_next)
		{
			stk.back().nodeidx = nd->m_next;
			stk.back().childrenvisited = false;
			stk.back().headerprinted = false;
		}
		else
		{
			stk.pop_back();
			if (!stk.empty()) out << pformat->closestruct;
		}
	}
}

const std::string InputStructure::tostring( const InputNodeVisitor& nv, const utils::PrintFormat* pformat) const
{
	std::ostringstream rt;
	InputStructure::print( (std::ostream&)rt, pformat, nv);
	return rt.str();
}

std::string InputStructure::nodepath( const InputNodeVisitor& nv) const
{
	std::vector<const char*> pt;
	int ni = nv.m_nodeidx;
	while (ni)
	{
		const InputNode* nd = node( ni);
		const char* tn = tagname( nd);
		if (tn) pt.push_back( tn);
		ni = nd->m_parent;
	}
	std::string rt;
	while (pt.size())
	{
		rt.push_back('/');
		rt.append( pt.back());
		pt.pop_back();
	}
	return rt;
}

std::string InputStructure::nodepath( const InputNode* nd) const
{
	return nodepath( visitor( nd));
}

InputNodeVisitor InputStructure::createChildNode( const InputNodeVisitor& nv)
{
	int idx = (int)m_nodemem.alloc( 1);
	InputNode*nd = node( nv);
	if (!nd->m_lastchild)
	{
		nd->m_lastchild = nd->m_firstchild = idx;
	}
	else
	{
		InputNode*lc = node( nd->m_lastchild);
		lc->m_next = nd->m_lastchild = idx;
	}
	InputNode*chld = node( idx);
	chld->m_arrayindex = -1;
	chld->m_parent = nv.m_nodeidx;
	return InputNodeVisitor(idx);
}

InputNodeVisitor InputStructure::createSiblingNode( const InputNodeVisitor& nv)
{
	if (!nv.m_nodeidx) throw std::runtime_error( "try to create sibling node of root");
	return createChildNode( InputNodeVisitor( node( nv.m_nodeidx)->m_parent));
}

InputNodeVisitor InputStructure::visitTag( const InputNodeVisitor& nv, const std::string& tag) const
{
	const InputNode* nd = node( nv);
	if (!nd->m_firstchild) throw std::runtime_error( std::string( "node with name '") + tag + "' does not exist in structure at '" + nodepath(nv) + "' (node is empty)");
	int tgs = (int)m_privatetagmap.find( tag);
	if (!tgs) throw std::runtime_error( std::string( "node with name '") + tag + "' does not exist in structure (at all) at '" + nodepath(nv) + "'");
	const InputNode* cd = node( nd->m_firstchild);
	InputNodeVisitor rt;
	for (;;)
	{
		if (cd->m_tagstr == tgs)
		{
			if (rt.m_nodeidx) throw std::runtime_error( std::string( "cannot make unique selection of node with name '") + tag + "' in structure at '" + nodepath(nv) + "' (it exists more than once)");
			rt = visitor( cd);
		}
		if (!cd->m_next) break;
		cd = node( cd->m_next);
	}
	if (!rt.m_nodeidx)
	{
		throw std::runtime_error( std::string( "node with name '") + tag + "' does not exist in structure at '" + nodepath(nv) + "' (no node name matched)");
	}
	return rt;
}

InputNodeVisitor InputStructure::visitOrOpenUniqTag( const InputNodeVisitor& nv, const std::string& tag)
{
	InputNode*nd = node( nv);
	if (!nd->m_firstchild) return openTag( nv, tag);
	int tgs = (int)m_privatetagmap.get( tag);
	if (!tgs) return openTag( nv, tag);
	InputNode*cd = node( nd->m_firstchild);
	InputNodeVisitor rt;
	for (;;)
	{
		if (cd->m_tagstr == tgs)
		{
			if (rt.m_nodeidx) throw std::runtime_error( std::string( "cannot make unique selection of node with name '") + tag + "' in structure at '" + nodepath(nv) + "' (it exists more than once)");
			rt = visitor( cd);
		}
		if (!cd->m_next) break;
		cd = node( cd->m_next);
	}
	if (!rt.m_nodeidx) return openTag( nv, tag);
	return rt;
}

InputNodeVisitor InputStructure::openTag( const InputNodeVisitor& nv, const std::string& tag)
{
	if (m_done)
	{
		throw std::runtime_error( "tags not balanced in input (open tag after final close)");
	}
	InputNodeVisitor rt = createChildNode( nv);
	InputNode*nd = node( rt);

	nd->m_tag = (int)m_tagmap->find( tag);
	if (nd->m_tag == 0) nd->m_tag = (int)m_tagmap->unused();
	nd->m_tagstr = (int)m_privatetagmap.get( tag);
	return rt;
}

InputNodeVisitor InputStructure::openTag( const InputNodeVisitor& nv, const types::Variant& tag)
{
	if (m_done)
	{
		throw std::runtime_error( "tags not balanced in input (open tag after final close)");
	}
	InputNodeVisitor rt = createChildNode( nv);
	InputNode*nd = node( rt);

	if (tag.type() == types::Variant::String)
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
			throw std::runtime_error( std::string("array index cannot be converted to integer at '") + nodepath(nv) + "'");
		}
		if (nd->m_arrayindex < 0)
		{
			throw std::runtime_error( std::string( "array index is negative at '") + nodepath(nv) + "'");
		}
	}
	return rt;
}

bool InputStructure::isArrayNode( const InputNodeVisitor& nv) const
{
	const InputNode* nd = node( nv);
	if (!nd->m_firstchild) return false;
	const InputNode* cd = node( nd->m_firstchild);
	int arrayindex = -1;
	for (;;)
	{
		if (cd->m_tagstr) return false;
		if (cd->m_arrayindex <= arrayindex) return false;
		arrayindex = cd->m_arrayindex;

		if (!cd->m_next) break;
		cd = node( cd->m_next);
	}
	return true;
}

InputNodeVisitor InputStructure::closeTag( const InputNodeVisitor& nv)
{
	InputNode* nd = node( nv);
	if (nd->m_parent < 0)
	{
		if (m_done) throw std::runtime_error( "tags not balanced in input (close tag)");
		m_done = true;
		return nv;
	}
	InputNodeVisitor rt( nd->m_parent);

	InputNode*pn = node( nd->m_parent);
	if (nv.m_nodeidx != pn->m_lastchild)
	{
		throw std::logic_error("internal: illegal call of closeTag");
	}

	if (isArrayNode( nv))
	{
		// In case of an array the granparent of the array parent takes over the children of the array
		// and the parent is deleted from the tree.
		if (!nd->m_firstchild)
		{
			//... if there are no children then the empty array reference is deleted:
			if (pn->m_lastchild == pn->m_firstchild)
			{
				pn->m_lastchild = pn->m_firstchild = 0;
			}
			else
			{
				int ni = pn->m_firstchild;
				InputNode*cd = node(ni);
				while (cd->m_next && cd->m_next != nv.m_nodeidx)
				{
					cd = node( ni=cd->m_next);
				}
				if (!cd->m_next) throw std::logic_error("internal: corrupt tree");
				cd->m_next = 0;
				pn->m_lastchild = ni;
			}
		}
		else
		{
			// ... there are children, so they inherit parent attributes and get added as granparents children
			InputNode*cd = node( nd->m_firstchild);
			for (;;)
			{
				cd->m_tag = nd->m_tag;
				cd->m_tagstr = nd->m_tagstr;	//... tag names taken from father that will disappear
				cd->m_parent = nd->m_parent;	//... parent line to granparent from father that will disappear
				if (!cd->m_next) break;
				cd = node( cd->m_next);
			}
			if (nd->m_lastchild == nd->m_firstchild)
			{
				*nd = *node( nd->m_firstchild);
			}
			else
			{
				int lastchild = nd->m_lastchild;
				*nd = *node( nd->m_firstchild);
				pn->m_lastchild = lastchild;
			}
		}
	}
	return rt;
}

void InputStructure::pushValue( const InputNodeVisitor& nv, const types::VariantConst& val)
{
	if (m_done)
	{
		throw std::runtime_error( "tags not balanced in input (value after final close)");
	}
	InputNode*nd = node( nv);
	if (nd->m_value)
	{
		throw std::runtime_error( std::string( "multiple values assigned to one node in the data tree at '") + nodepath(nv) + "'");
	}
	nd->m_value = m_content.size();
	m_content.push_back( val);
}


void InputStructure::next( const InputNode*nd, int tag, std::vector<InputNodeIndex>& nextnd) const
{
	if (!nd->m_firstchild) return;
	InputNodeIndex ci = nd->m_firstchild;
	const InputNode* cd = node( ci);
	if (!tag)
	{
		for (;;)
		{
			nextnd.push_back( ci);
			if (!cd->m_next) break;
			cd = node( ci=cd->m_next);
		}
	}
	else
	{
		for (;;)
		{
			if (cd->m_tag == tag) nextnd.push_back( ci);
			if (!cd->m_next) break;
			cd = node( ci=cd->m_next);
		}
	}
}

void InputStructure::find( const InputNode* nd, int tag, std::vector<InputNodeIndex>& findnd) const
{
	if (!nd->m_firstchild) return;
	InputNodeIndex ci = nd->m_firstchild;
	const InputNode* cd = node( ci);
	if (!tag)
	{
		for (;;)
		{
			findnd.push_back( ci);
			if (cd->m_firstchild) find( cd, tag, findnd);
			if (!cd->m_next) break;
			cd = node( ci=cd->m_next);
		}
	}
	else
	{
		for (;;)
		{
			if (cd->m_tag == tag) findnd.push_back( ci);
			if (cd->m_firstchild) find( cd, tag, findnd);
			if (!cd->m_next) break;
			cd = node( ci=cd->m_next);
		}
	}
}

void InputStructure::up( const InputNode* nd, std::vector<InputNodeIndex>& rt) const
{
	if (nd->m_parent != -1)
	{
		rt.push_back( nd->m_parent);
	}
	else
	{
		throw std::runtime_error( "selecting /.. from root");
	}
}

const InputNode* InputStructure::root() const
{
	return node(0);
}

const types::Variant* InputStructure::contentvalue( const InputNode* nd) const
{
	if (nd->m_value)
	{
		return &m_content.at( nd->m_value);
	}
	return 0;
}

const char* InputStructure::tagname( const InputNode* nd) const
{
	return m_privatetagmap.getstr( nd->m_tagstr);
}

bool InputStructure::isequalTag( const std::string& t1, const std::string& t2) const
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
///\class InputFilter
///\brief Input InputFilter for calling preprocessing functions
class InputFilter
	:public langbind::TypedInputFilter
{
public:
	typedef InputStructure::NodeAssignment NodeAssignment;

	///\brief Default constructor
	InputFilter()
		:langbind::TypedInputFilter("dbpreprocin")
		,m_structure(0)
		,m_elementitr(0)
	{
		m_nodeitr = m_nodelist.begin();
	}

	///\brief Copy constructor
	InputFilter( const InputFilter& o)
		:langbind::TypedInputFilter(o)
		,m_structure(o.m_structure)
		,m_nodelist(o.m_nodelist)
		,m_stack(o.m_stack)
		,m_elementbuf(o.m_elementbuf)
		,m_elementitr(o.m_elementitr)
	{
		m_nodeitr = m_nodelist.begin() + (o.m_nodeitr - o.m_nodelist.begin());
	}

	///\brief Constructor
	InputFilter( const InputStructure* structure_, const std::vector<NodeAssignment>& nodelist_)
		:langbind::TypedInputFilter("dbpreprocin")
		,m_structure(structure_)
		,m_nodelist(nodelist_)
		,m_elementitr(0)
	{
		m_nodeitr = m_nodelist.begin();
	}

	///\brief Destructor
	virtual ~InputFilter(){}

	///\brief Implementation of TypedInputFilter::copy()
	virtual TypedInputFilter* copy() const		{return new InputFilter( *this);}

	///\brief Implementation of TypedInputFilter::getNext(ElementType&,types::VariantConst&)
	virtual bool getNext( ElementType& type, types::VariantConst& element)
	{
		for (;;)
		{
			// [1] return buffered elements first:
			if (m_elementitr < m_elementbuf.size())
			{
				std::pair<ElementType, types::VariantConst>& ee = m_elementbuf.at( m_elementitr++);
				type = ee.first;
				element = ee.second;
				if (m_elementitr == m_elementbuf.size())
				{
					m_elementbuf.clear();
					m_elementitr = 0;
				}
				return true;
			}
			// [2] fetch new parameter node if the current one is finished:
			if (m_stack.empty())
			{
				if (!m_structure)
				{
					// ... calling getNext after final CloseTag
					setState( langbind::InputFilter::Error, "internal: illegal call of input filter");
					return false;
				}
				while (m_nodeitr != m_nodelist.end() && m_nodeitr->second < 0)
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
				m_stack.push_back( StackElement( m_structure->node( m_nodeitr->second), false));
				if (!m_nodeitr->first.empty())
				{
					// ... only open non empty tags
					type = TypedInputFilter::OpenTag;
					element = m_nodeitr->first;
					++m_stack.back().closetagcnt;
					++m_nodeitr;
					return true;
				}
				else
				{
					// ... empty tag = embedded content
					++m_nodeitr;
				}
				// ! The case that the parameter node is an array node (m_arrayindex) is not respected.
				//	It is treated as a single node.
			}
			// [3] structure element fetching statemachine:
			if (m_stack.back().node)
			{
				if (!m_stack.back().childrenvisited && m_stack.back().node->m_firstchild)
				{
					// mark children as visited:
					m_stack.back().childrenvisited = true;

					// push children (first child node) on the stack:
					const InputNode* cnod = m_structure->node( m_stack.back().node->m_firstchild);
					type = TypedInputFilter::OpenTag;
					element = m_structure->tagname( cnod);
					m_stack.push_back( StackElement( cnod, true));
					++m_stack.back().closetagcnt;

					if (cnod->m_arrayindex >= 0)
					{
						++m_stack.back().closetagcnt;
						m_elementbuf.push_back( Element( TypedInputFilter::OpenTag, cnod->m_arrayindex));
					}
					return true;
				}
				if (!m_stack.back().valuevisited)
				{
					// mark children as visited:
					m_stack.back().valuevisited = true;

					// return value:
					const types::Variant* val = m_structure->contentvalue( m_stack.back().node);
					if (val)
					{
						type = TypedInputFilter::Value;
						element = *val;
						return true;
					}
				}
				if (m_stack.back().node->m_next && m_stack.back().visitnext)
				{
					// replace current node on the stack by its next neighbour
					const InputNode* snod = m_structure->node( m_stack.back().node->m_next);
					if (flag( SerializeWithIndices))
					{
						const InputNode* tnod = m_stack.back().node;
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
								++m_stack.back().closetagcnt;
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
					m_stack.back().valuevisited = false;		//... and its value
					return true;
				}
				// ... from here on we push all elements to return on the element buffer for the top stack element
				while (m_stack.back().closetagcnt > 1)
				{
					// close element related tag (e.g. index tag):
					m_elementbuf.push_back( Element( TypedInputFilter::CloseTag, types::Variant()));
					-- m_stack.back().closetagcnt;
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
		const InputNode* node;
		bool childrenvisited;
		bool valuevisited;
		bool visitnext;
		int closetagcnt;

		StackElement()
			:node(0),childrenvisited(false),valuevisited(false),visitnext(false),closetagcnt(0){}
		StackElement( const StackElement& o)
			:node(o.node),childrenvisited(o.childrenvisited),valuevisited(o.valuevisited),visitnext(o.visitnext),closetagcnt(o.closetagcnt){}
		StackElement( const InputNode* node_, bool visitnext_)
			:node(node_),childrenvisited(false),valuevisited(false),visitnext(visitnext_),closetagcnt(0){}
	};

	const InputStructure* m_structure;		//< structure to get the elements from
	std::vector<NodeAssignment> m_nodelist;				//< list of element nodes in the structure
	std::vector<NodeAssignment>::const_iterator m_nodeitr;		//< iterator on elements nodes of the structure
	std::vector<StackElement> m_stack;				//< visit tree hierarchy stack
	typedef std::pair<ElementType, types::VariantConst> Element;	//< element for buffer
	std::vector<Element> m_elementbuf;				//< buffer for elements in states producing more than one element
	std::size_t m_elementitr;					//< iterator on elements in buffer
};


///\class OutputFilter
///\brief Input OutputFilter for calling preprocessing functions
class OutputFilter
	:public langbind::TypedOutputFilter
{
public:
	///\brief Default constructor
	OutputFilter()
		:langbind::TypedOutputFilter("dbpreprocout")
		,m_structure(0)
		,m_lasttype( langbind::TypedInputFilter::Value)
		,m_taglevel(0)
	{}

	///\brief Copy constructor
	OutputFilter( const OutputFilter& o)
		:langbind::TypedOutputFilter(o)
		,m_structure(o.m_structure)
		,m_visitor(o.m_visitor)
		,m_lasttype(o.m_lasttype)
		,m_taglevel(o.m_taglevel)
		,m_sourccetagmap(o.m_sourccetagmap)
	{}

	///\brief Constructor
	OutputFilter( InputStructure* structure_, const InputNodeVisitor& visitor_, const std::map<int, bool>& sourccetagmap_)
		:langbind::TypedOutputFilter("dbpreprocout")
		,m_structure(structure_)
		,m_visitor(visitor_)
		,m_lasttype( langbind::TypedInputFilter::Value)
		,m_taglevel(0)
		,m_sourccetagmap(sourccetagmap_)
	{}

	///\brief Destructor
	virtual ~OutputFilter(){}

	///\brief Implementation of TypedOutputFilter::copy()
	virtual TypedOutputFilter* copy() const		{return new OutputFilter( *this);}

	///\brief Implementation of TypedOutputFilter::print(ElementType,const types::VariantConst&)
	virtual bool print( ElementType type, const types::VariantConst& element)
	{
		LOG_DATA << "[transaction input] push element " << langbind::InputFilter::elementTypeName( type) << " '" << utils::getLogString( element) << "'";
		switch (type)
		{
			case langbind::TypedInputFilter::OpenTag:
				++m_taglevel;
				m_visitor = m_structure->openTag( m_visitor, element);
				if (m_taglevel == 1 && m_sourccetagmap.find( m_structure->node(m_visitor)->m_tagstr) != m_sourccetagmap.end())
				{
					throw std::runtime_error( "forbidden assignment ot result to tag that already exists in input");
				}
			break;
			case langbind::TypedInputFilter::CloseTag:
				-- m_taglevel;
				m_visitor = m_structure->closeTag( m_visitor);
			break;
			case langbind::TypedInputFilter::Attribute:
				m_visitor = m_structure->openTag( m_visitor, element);
				if (m_taglevel == 0 && m_sourccetagmap.find( m_structure->node(m_visitor)->m_tagstr) != m_sourccetagmap.end())
				{
					throw std::runtime_error( "forbidden assignment ot result to tag that already exists in input");
				}
			break;
			case langbind::TypedInputFilter::Value:
				m_structure->pushValue( m_visitor, element);
				if (m_lasttype == langbind::TypedInputFilter::Attribute)
				{
					m_visitor = m_structure->closeTag( m_visitor);
				}
			break;
		}
		m_lasttype = type;
		return true;
	}

private:
	InputStructure* m_structure;	//< structure to print the elements to
	InputNodeVisitor m_visitor;
	ElementType m_lasttype;
	int m_taglevel;
	const std::map<int, bool> m_sourccetagmap;
};

}//namespace

langbind::TypedInputFilter* InputStructure::createInputFilter( const std::vector<NodeAssignment>& nodelist) const
{
	langbind::TypedInputFilter* rt = new InputFilter( this, nodelist);
	if (!m_tagmap->case_sensitive())
	{
		rt->setFlags( langbind::TypedInputFilter::PropagateNoCase);
	}
	return rt;
}

langbind::TypedOutputFilter* InputStructure::createOutputFilter( const InputNodeVisitor& nv, const std::map<int, bool>& sourccetagmap)
{
	langbind::TypedOutputFilter* rt = new OutputFilter( this, nv, sourccetagmap);
	return rt;
}



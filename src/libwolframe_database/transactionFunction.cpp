/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
///\brief Implementation of the builder of transaction input and reader of output
///\file transactionFunction.cpp
#include "types/allocators.hpp"
#include "utils/miscUtils.hpp"
#include "database/transactionFunction.hpp"
#include "database/transaction.hpp"
#include "textwolf/xmlscanner.hpp"
#include "textwolf/cstringiterator.hpp"
#include "textwolf/charset.hpp"
#include <cstring>
#include <sstream>
#include <iostream>
#include <locale>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

class TransactionFunction::TagTable
{
public:
	TagTable()
		:m_size(0){}
	TagTable( const TagTable& o)
		:m_size(o.m_size),m_map(o.m_map){}

	int find( const char* tag, std::size_t tagsize) const;
	int find( const std::string& tagstr) const;
	int get( const std::string& tagstr);
	int get( const char* tag, std::size_t tagsize);
	int unused() const;

	std::map<int,int> insert( const TagTable& o)
	{
		std::map<int,int> rt;
		std::map< std::string, int>::const_iterator oi = o.m_map.begin(), oe = o.m_map.end();
		for (; oi != oe; ++oi)
		{
			rt[ oi->second] = get( oi->first);
		}
		return rt;
	}
private:
	int m_size;
	std::map< std::string, int> m_map;
};

class TransactionFunctionInput::Structure
{
public:
	Structure( const TransactionFunction::TagTable* tagmap);
	Structure( const Structure& o);

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
		std::size_t valuesize() const			{return (m_element > 0)?(std::size_t)m_elementsize:0;}
	};

	Node root() const;
	void next( const Node& nd, int tag, std::vector<Node>& rt) const;
	void find( const Node& nd, int tag, std::vector<Node>& rt) const;
	void up( const Node& nd, std::vector<Node>& rt) const;
	const char* nodevalue( const Node& nd) const;

	const std::string tostring() const;

	void setParentLinks( std::size_t mi);
	void openTag( const char* tag, std::size_t tagsize);
	void openTag( const std::string& tag);
	void closeTag();
	void createRootNode();
	void pushValue( const char* val, std::size_t valsize);
	void pushValue( const std::string& val);
	void check() const;

private:
	types::TypedArrayDoublingAllocator<Node> m_nodemem;
	types::TypedArrayDoublingAllocator<char> m_strmem;
	const TransactionFunction::TagTable* m_tagmap;
	std::size_t m_rootidx;
	std::size_t m_rootsize;
	typedef std::vector< std::vector<Node> > BuildNodeStruct;
	BuildNodeStruct m_data;
};

class Path
{
public:
	enum ElementType
	{
		Next,
		Find,
		Up,
		Result,
		Constant
	};

	static const char* elementTypeName( ElementType i)
	{
		static const char* ar[] ={"Next","Find","Current","Up","Result","Constant"};
		return ar[(int)i];
	}

	struct Element
	{
		ElementType m_type;
		int m_tag;
	};

	Path(){}
	Path( const std::string& src, TransactionFunction::TagTable* tagmap, std::string* strings);
	Path( const Path& o);
	std::string tostring() const;

	std::size_t resultReference() const;
	std::size_t constantReference() const;
	void selectNodes( const TransactionFunctionInput::Structure& st, const TransactionFunctionInput::Structure::Node& nd, std::vector<TransactionFunctionInput::Structure::Node>& ar) const;

	std::vector<Element>::const_iterator begin() const		{return m_path.begin();}
	std::vector<Element>::const_iterator end() const		{return m_path.end();}
	std::size_t size() const					{return m_path.size();}

	void rewrite( const std::map<int,int>& rwtab)
	{
		std::vector<Element>::iterator pi = m_path.begin(), pe = m_path.end();
		for (; pi != pe; ++pi)
		{
			std::map<int,int>::const_iterator re = rwtab.find( pi->m_tag);
			if (re == rwtab.end()) throw std::logic_error( "rewrite table not complete");
			pi->m_tag = re->second;
		}
	}

	void append( const Path& o)
	{
		m_path.insert( m_path.end(), o.begin(), o.end());
	}

private:
	std::vector<Element> m_path;
};

class FunctionCall
{
public:
	FunctionCall()
		:m_nonemptyResult(false)
		,m_uniqueResult(false){}
	FunctionCall( const FunctionCall& o);
	FunctionCall( const std::string& resname, const std::string& name, const Path& selector, const std::vector<Path>& arg, bool setNonemptyResult_, bool setUniqueResult_, std::size_t level_);

	const Path& selector() const			{return m_selector;}
	const std::vector<Path>& arg() const		{return m_arg;}
	const std::string& name() const			{return m_name;}
	const std::string& resultname() const		{return m_resultname;}
	void resultname( const char* r)			{m_resultname = r;}

	bool hasResultReference() const;
	bool hasNonemptyResult() const			{return m_nonemptyResult;}
	bool hasUniqueResult() const			{return m_uniqueResult;}
	std::size_t level() const			{return m_level;}

private:
	std::string m_resultname;
	std::string m_name;
	Path m_selector;
	std::vector<Path> m_arg;
	bool m_nonemptyResult;
	bool m_uniqueResult;
	std::size_t m_level;
};

struct TransactionFunction::Impl
{
	std::string m_resultname;
	std::vector<std::string> m_elemname;
	std::vector<bool> m_elemunique;
	std::vector<FunctionCall> m_call;
	TagTable m_tagmap;
	std::string m_strings;

	Impl( const std::vector<TransactionDescription>& description, const types::keymap<TransactionFunctionR>& functionmap);
	Impl( const Impl& o);
};


static std::string normalizeTagName( const std::string& tagname)
{
	std::string rt;
	static textwolf::XMLScannerBase::IsTagCharMap isTagCharMap;
	textwolf::CStringIterator itr( tagname.c_str(), tagname.size());
	typedef textwolf::XMLScanner<textwolf::CStringIterator,textwolf::charset::UTF8,textwolf::charset::UTF8,std::string> T;
	if (!T::parseStaticToken( isTagCharMap, itr, rt))
	{
		throw std::runtime_error( "illegal tag name");
	}
	return rt;
}

int TransactionFunction::TagTable::find( const char* tag, std::size_t tagsize) const
{
	const std::string tagnam( tag, tagsize);
	return find( tagnam);
}

int TransactionFunction::TagTable::find( const std::string& tagnam) const
{
	std::map< std::string, int>::const_iterator ii = m_map.find( tagnam);
	if (ii == m_map.end()) return 0;
	return ii->second;
}

int TransactionFunction::TagTable::get( const std::string& tagnam)
{
	std::map< std::string, int>::const_iterator ii = m_map.find( tagnam);
	if (ii == m_map.end())
	{
		m_map[ tagnam] = ++m_size;
		return m_size;
	}
	else
	{
		return ii->second;
	}
}

int TransactionFunction::TagTable::get( const char* tag, std::size_t tagsize)
{
	const std::string tagstr( tag, tagsize);
	return get( tagstr);
}

int TransactionFunction::TagTable::unused() const
{
	return m_size +1;
}

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
	,m_strmem(o.m_strmem)
	,m_tagmap(o.m_tagmap)
	,m_rootidx(o.m_rootidx)
	,m_rootsize(o.m_rootsize)
	,m_data(o.m_data)
	{}

TransactionFunctionInput::Structure::Structure( const TransactionFunction::TagTable* tagmap)
	:m_tagmap(tagmap)
	,m_rootidx(0)
	,m_rootsize(0)
{
	m_nodemem.alloc( 1);
	m_strmem.alloc( 1);
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
	stk.push_back( std::pair< std::size_t, std::size_t>( m_rootsize, m_rootidx));
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
				const char* val = &m_strmem[ cd->m_element];
				while (indent--) rt << '\t';
				if (cd->m_tag) rt << cd->m_tag << ": ";
				rt << "'" << val << "'" << std::endl;
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

	m_data.back().push_back( Node( 0, mi, 0, 0));
	m_data.push_back( std::vector<Node>());
}

void TransactionFunctionInput::Structure::createRootNode()
{
	m_rootsize = m_data.back().size();
	m_rootidx = m_nodemem.alloc( m_rootsize);
	Node* nd = &m_nodemem[ m_rootidx];
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
		pushValue( "", 0);
	}
	createRootNode();
	m_data.pop_back();
	m_data.back().back().m_elementsize = m_rootsize;
	m_data.back().back().m_element = Node::ref_element( m_rootidx);

	if (m_data.size() == 1)
	{
		// top level tag closed, assuming end of structure,
		// creating root node and set all parent links:
		createRootNode();
		for (std::size_t ri=0; ri<m_rootsize; ++ri)
		{
			setParentLinks( m_rootidx+ri);
		}
	}
}

void TransactionFunctionInput::Structure::pushValue( const char* val, std::size_t valsize)
{
	std::size_t mi = m_strmem.alloc( valsize+1);
	char* mem = m_strmem.base() + mi;
	std::memcpy( mem, val, (valsize+1)*sizeof(char));

	m_data.back().push_back( Node( 0, 0, valsize, Node::val_element( mi)));
}

void TransactionFunctionInput::Structure::pushValue( const std::string& val)
{
	pushValue( val.c_str(), val.size());
}

void TransactionFunctionInput::Structure::next( const Node& nd, int tag, std::vector<Node>& nextnd) const
{
	std::size_t ii = 0, nn = nd.nofchild(), idx = nd.childidx();
	if (nn)
	{
		const Node* cd = &m_nodemem[ idx];
		for (; ii<nn; ++ii)
		{
			if (cd[ii].m_tag)
			{
				if (!tag || cd[ii].m_tag == tag)
				{
					nextnd.push_back( cd[ii]);
				}
			}
		}
	}
}

void TransactionFunctionInput::Structure::find( const Node& nd, int tag, std::vector<Node>& findnd) const
{
	std::size_t ii = 0, nn = nd.nofchild(), idx = nd.childidx();
	if (nn)
	{
		const Node* cd = &m_nodemem[ idx];
		for (; ii<nn; ++ii)
		{
			if (cd[ii].m_tag)
			{
				if (!tag || cd[ii].m_tag == tag)
				{
					findnd.push_back( cd[ii]);
				}
			}
			if (nd.childidx())
			{
				find( cd[ii], tag, findnd);
			}
		}
	}
}

void TransactionFunctionInput::Structure::up( const Node& nd, std::vector<Node>& rt) const
{
	if (nd.m_parent != 0)
	{
		rt.push_back( m_nodemem[ nd.m_parent]);
	}
	else
	{
		throw std::runtime_error( "selecting /.. from root");
	}
}

TransactionFunctionInput::Structure::Node TransactionFunctionInput::Structure::root() const
{
	Node rt( 0, 0, m_rootsize, Node::ref_element(m_rootidx));
	return rt;
}

const char* TransactionFunctionInput::Structure::nodevalue( const Node& nd) const
{
	const char* rt = 0;
	std::size_t ii = 0, nn = nd.nofchild(), idx = nd.childidx();
	if (nn)
	{
		const Node* cd = &m_nodemem[ idx];
		for (; ii<nn; ++ii)
		{
			if (!cd[ii].m_tag)
			{
				if (rt) throw std::runtime_error( "node selected has more than one value");
				std::size_t validx = cd[ii].valueidx();
				if (validx) rt = &m_strmem[ validx];
			}
		}
	}
	return rt;
}

Path::Path( const std::string& pt, TransactionFunction::TagTable* tagmap, std::string* strings)
{
	Element elem;
	std::string::const_iterator ii = pt.begin(), ee = pt.end();
	while (ii != ee)
	{
		if (*ii == '\'' || *ii == '\"')
		{
			std::string constant;
			utils::parseNextToken( constant, ii, ee);
			elem.m_type = Constant;
			elem.m_tag = strings->size();
			strings->append( constant);
			strings->push_back( '\0');
			if (!m_path.empty()) throw std::runtime_error( "unexpected string constant in path");
			if (utils::gotoNextToken( ii, ee)) throw std::runtime_error( "unexpected token after string constant argument");
			m_path.push_back( elem);
		}
		else if (*ii == '$')
		{
			elem.m_type = Result;
			std::string resno;
			for (++ii; ii < ee && *ii >= '0' && *ii <= '9'; ++ii)
			{
				resno.push_back( *ii);
			}
			if (resno.size() == 0 || resno.size() > 999) throw std::runtime_error( "illegal result reference (only numbers between 1 and 999 allowed)");
			elem.m_tag = std::atoi( resno.c_str());
			if (elem.m_tag == 0) throw std::runtime_error( "referencing result with index 0");
			m_path.push_back( elem);
			continue;
		}
		else if (*ii == '/')
		{
			++ii;
			if (ii == ee) break;
			if (*ii == '/')
			{
				elem.m_type = Find;
				++ii;
			}
			else
			{
				elem.m_type = Next;
			}
		}
		else
		{
			elem.m_type = Next;
		}
		std::string tagnam;
		while (ii != ee && *ii != '/')
		{
			tagnam.push_back( *ii);
			++ii;
		}
		if (tagnam.empty())
		{
			throw std::runtime_error( std::string("empty tag in path '") + pt + "'" );
		}
		else if (tagnam == "..")
		{
			if (elem.m_type == Find)
			{
				throw std::runtime_error( std::string("selection '//..' is illegal in path '") + pt + "'" );
			}
			elem.m_type = Up;
			elem.m_tag = 0;
		}
		else if (tagnam == ".")
		{
			if (elem.m_type == Find)
			{
				throw std::runtime_error( std::string("selection '//.' is illegal in path '") + pt + "'" );
			}
			continue;
		}
		else if (tagnam == "*")
		{
			elem.m_tag = 0;
		}
		else
		{
			tagnam = normalizeTagName( tagnam);
			elem.m_tag = tagmap->get( tagnam);
		}
		m_path.push_back( elem);
	}
	std::vector<Element>::const_iterator pi = m_path.begin(), pe = m_path.end();
	for (; pi != pe; ++pi)
	{
		if (pi->m_type == Result && m_path.size() > 1)
		{
			throw std::runtime_error( "referencing result variable in path");
		}
	}
}

Path::Path( const Path& o)
	:m_path(o.m_path){}

std::size_t Path::resultReference() const
{
	if (m_path.size() && m_path[0].m_type == Result) return m_path[0].m_tag;
	return 0;
}

std::size_t Path::constantReference() const
{
	if (m_path.size() && m_path[0].m_type == Constant) return m_path[0].m_tag;
	return 0;
}

std::string Path::tostring() const
{
	std::vector<Element>::const_iterator ii = m_path.begin(), ee = m_path.end();
	std::ostringstream rt;
	for (; ii != ee; ++ii)
	{
		rt << elementTypeName( ii->m_type) << " " << ii->m_tag << std::endl;
	}
	return rt.str();
}

void Path::selectNodes( const TransactionFunctionInput::Structure& st, const TransactionFunctionInput::Structure::Node& nd, std::vector<TransactionFunctionInput::Structure::Node>& ar) const
{
	typedef TransactionFunctionInput::Structure::Node Node;
	std::vector<Node> ar1,ar2;
	ar1.push_back( nd);

	// [B.1] Find selected nodes:
	std::vector<Element>::const_iterator si = begin(), se = end();
	for (; si != se; ++si)
	{
		ar2.clear();
		std::vector<Node>::const_iterator ni = ar1.begin(), ne = ar1.end();
		for (; ni != ne; ++ni)
		{
			switch (si->m_type)
			{
				case Result:
				case Constant:
					break;

				case Find:
					st.find( *ni, si->m_tag, ar2);
					break;

				case Next:
					st.next( *ni, si->m_tag, ar2);
					break;

				case Up:
					st.up( *ni, ar2);
					break;
			}
		}
		ar1 = ar2;
	}
	ar.insert( ar.end(), ar1.begin(), ar1.end());
}

FunctionCall::FunctionCall( const std::string& r, const std::string& n, const Path& s, const std::vector<Path>& a, bool q, bool u, std::size_t l)
	:m_resultname(r)
	,m_name(n)
	,m_selector(s)
	,m_arg(a)
	,m_nonemptyResult(q)
	,m_uniqueResult(u)
	,m_level(l){}

FunctionCall::FunctionCall( const FunctionCall& o)
	:m_resultname(o.m_resultname)
	,m_name(o.m_name)
	,m_selector(o.m_selector)
	,m_arg(o.m_arg)
	,m_nonemptyResult(o.m_nonemptyResult)
	,m_uniqueResult(o.m_uniqueResult)
	,m_level(o.m_level){}

bool FunctionCall::hasResultReference() const
{
	std::vector<Path>::const_iterator pi,pe;
	for (pi=m_arg.begin(),pe=m_arg.end(); pi!=pe; ++pi)
	{
		if (pi->resultReference()) return true;
	}
	return false;
}

static bool isAlphaNumeric( char ch)
{
	if (ch >= '0' && ch <= '9') return true;
	if (ch >= 'A' && ch <= 'Z') return true;
	if (ch >= 'a' && ch <= 'z') return true;
	if (ch == '_') return true;
	return false;
}

static bool checkResultIdentifier( const std::string& id)
{
	if (id == ".") return true;
	std::string::const_iterator ii = id.begin(), ie = id.end();
	while (ii != ie && isAlphaNumeric( *ii)) ++ii;
	return (ii == ie);
}

TransactionFunctionInput::TransactionFunctionInput( const TransactionFunction* func_)
	:m_structure( new Structure( func_->tagmap()))
	,m_func(func_)
	,m_lasttype( langbind::TypedInputFilter::Value){}

TransactionFunctionInput::~TransactionFunctionInput()
{
	delete m_structure;
}

TransactionFunctionInput::TransactionFunctionInput( const TransactionFunctionInput& o)
	:langbind::TypedOutputFilter(o)
	,m_structure( new Structure( *o.m_structure))
	,m_func(o.m_func)
	,m_lasttype(o.m_lasttype){}

bool TransactionFunctionInput::print( ElementType type, const Element& element)
{
	switch (type)
	{
		case langbind::TypedInputFilter::OpenTag:
			m_structure->openTag( element.tostring());
		break;
		case langbind::TypedInputFilter::CloseTag:
			m_structure->closeTag();
		break;
		case langbind::TypedInputFilter::Attribute:
			m_structure->openTag( element.tostring());
		break;
		case langbind::TypedInputFilter::Value:
			m_structure->pushValue( element.tostring());
			if (m_lasttype == langbind::TypedInputFilter::Attribute)
			{
				m_structure->closeTag();
			}
		break;
	}
	m_lasttype = type;
	return true;
}

static void bindArguments( TransactionInput& ti, const FunctionCall& call, const TransactionFunctionInput* inputst, const TransactionFunctionInput::Structure::Node& selectornode, const std::string& constants)
{
	typedef TransactionFunctionInput::Structure::Node Node;

	std::vector<Path>::const_iterator pi=call.arg().begin(), pe=call.arg().end();
	for (std::size_t argidx=1; pi != pe; ++pi,++argidx)
	{
		std::size_t idx;
		if ((idx = pi->resultReference()) != 0)
		{
			ti.bindCommandArgAsResultReference( idx);
		}
		else if ((idx = pi->constantReference()) != 0)
		{
			ti.bindCommandArgAsValue( constants.c_str() + idx);
		}
		else
		{
			std::vector<Node> param;
			pi->selectNodes( inputst->structure(), selectornode, param);
			if (param.size() == 0)
			{
				ti.bindCommandArgAsNull();
			}
			else
			{
				std::vector<Node>::const_iterator gs = param.begin(), gi = param.begin()+1, ge = param.end();
				for (; gi != ge; ++gi)
				{
					if (*gs != *gi) throw std::runtime_error( "more than one node selected in db call argument");
				}
				const char* value = inputst->structure().nodevalue( *gs);
				if (value)
				{
					ti.bindCommandArgAsValue( value);
				}
				else
				{
					ti.bindCommandArgAsNull();
				}
			}
		}
	}
}

static void getOperationInput( const TransactionFunctionInput* this_, TransactionInput& rt, std::size_t startfidx, std::size_t level, std::vector<FunctionCall>::const_iterator ci, std::vector<FunctionCall>::const_iterator ce, const std::vector<TransactionFunctionInput::Structure::Node>& rootnodearray)
{
	typedef TransactionFunctionInput::Structure::Node Node;
	std::size_t fidx = startfidx;
	for (; ci != ce; ++ci,++fidx)
	{
		if (ci->hasNonemptyResult()) rt.setNonemptyResult( fidx);
		if (ci->hasUniqueResult()) rt.setUniqueResult( fidx);

		// Select the nodes to execute the command with:
		std::vector<Node> nodearray;
		std::vector<Node>::const_iterator ni = rootnodearray.begin(), ne = rootnodearray.end();
		for (; ni != ne; ++ni)
		{
			ci->selector().selectNodes( this_->structure(), *ni, nodearray);
		}
		// For each selected node do expand the function call arguments:
		std::vector<Node>::const_iterator vi=nodearray.begin(), ve=nodearray.end();
		for (; vi != ve; ++vi)
		{
			if (ci->level() > level)
			{
				if (!ci->name().empty()) throw std::logic_error("passing arguments expected when calling OPERATION");
				rt.startCommand( fidx, ci->level(), ci->name());
				bindArguments( rt, *ci, this_, *vi, this_->func()->impl().m_strings);

				std::vector<FunctionCall>::const_iterator ca = ci;
				for (++ca; ca != ce; ++ca)
				{
					if (ca->level() < level || (ca->level() == level && ca->name().empty())) break;
				}
				getOperationInput( this_, rt, fidx, ci->level(), ++ci, ca, nodearray);
				ci = ca;
				--ci;
			}
			else
			{
				rt.startCommand( fidx, ci->level(), ci->name());
				bindArguments( rt, *ci, this_, *vi, this_->func()->impl().m_strings);
			}
		}
	}
}

TransactionInput TransactionFunctionInput::get() const
{
	TransactionInput rt;
	std::vector<FunctionCall>::const_iterator ci = m_func->impl().m_call.begin(), ce = m_func->impl().m_call.end();

	std::vector<Structure::Node> nodearray;
	nodearray.push_back( structure().root());
	getOperationInput( this, rt, 0, 1, ci, ce, nodearray);
	return rt;
}

struct TransactionFunctionOutput::Impl
{
	int m_state;
	int m_rowidx;
	int m_residx;
	int m_colidx;
	int m_colend;
	std::string m_rootname;
	std::vector<std::string> m_resname;
	std::vector<bool> m_resunique;
	db::TransactionOutput::result_iterator m_resitr;
	db::TransactionOutput::result_iterator m_resend;
	db::TransactionOutput::row_iterator m_rowitr;
	db::TransactionOutput::row_iterator m_rowend;
	db::TransactionOutput m_data;

	Impl( const std::string& rootname_, const std::vector<std::string>& resname_, const std::vector<bool>& resunique_, const db::TransactionOutput& data_)
		:m_state(0)
		,m_rowidx(0)
		,m_residx(0)
		,m_colidx(0)
		,m_colend(0)
		,m_rootname(rootname_)
		,m_resname(resname_)
		,m_resunique(resunique_)
		,m_data(data_){}

	void resetIterator()
	{
		m_state = 0;
	}

	bool hasResultName( std::size_t functionidx)
	{
		return (m_resname[ functionidx].size() != 1 || m_resname[ functionidx][0] != '.');
	}

	bool getNext( ElementType& type, TypedFilterBase::Element& element, bool doSerializeWithIndices)
	{
		for (;;) switch (m_state)
		{
			case 0:
				m_resitr = m_data.begin();
				m_residx = 1;
				m_resend = m_data.end();
				m_state = 1;
				if (!m_rootname.empty())
				{
					type = TypedInputFilter::OpenTag;
					element = m_rootname;
					return true;
				}
				/* no break here !*/
			case 1:
				if (m_resitr == m_resend)
				{
					// close tag marking end of data:
					type = TypedInputFilter::CloseTag;
					element = TypedInputFilter::Element();
					return true;
				}
				if (m_resname[ m_resitr->functionidx()].empty())
				{
					// result is not part of output:
					++m_resitr;
					++m_residx;
					continue;
				}
				m_rowitr = m_resitr->begin();
				m_rowend = m_resitr->end();
				m_rowidx = 0;
				m_colidx = 0;
				m_colend = m_resitr->nofColumns();
				m_state = 2;
				if (doSerializeWithIndices
				&&  hasResultName( m_resitr->functionidx()))
				{
					// open array tag:
					type = TypedInputFilter::OpenTag;
					element = m_resname[ m_resitr->functionidx()];
					return true;
				}
				/* no break here !*/
			case 2:
				m_colidx = 0;
				if (m_rowitr == m_rowend)
				{
					if (doSerializeWithIndices
					&&  !m_resunique[ m_resitr->functionidx()]
					&&  hasResultName( m_resitr->functionidx()))
					{
						// close array tag:
						type = TypedInputFilter::CloseTag;
						element = TypedInputFilter::Element();
						// next result:
						++m_resitr;
						++m_residx;
						m_state = 1;
						return true;
					}
					else
					{
						// next result:
						++m_resitr;
						++m_residx;
						m_state = 1;
						continue;
					}
				}
				m_state = 3;
				if (doSerializeWithIndices)
				{
					// tag is array (element) index
					if (!m_resunique[ m_resitr->functionidx()])
					{
						// ... we set it only for non unique results
						type = TypedInputFilter::OpenTag;
						element = TypedInputFilter::Element( m_rowidx+1);
						return true;
					}
					else
					{
						continue;
					}
				}
				else if (hasResultName( m_resitr->functionidx()))
				{
					// tag is result (element) name
					type = TypedInputFilter::OpenTag;
					element = m_resname[ m_resitr->functionidx()];
					return true;
				}
				/* no break here !*/
			case 3:
				if (m_colidx == m_colend)
				{
					++m_rowitr;
					++m_rowidx;
					m_state = 2;
					continue;
				}
				if ((*m_rowitr)[ m_colidx])
				{
					m_state = 4;
					type = TypedInputFilter::OpenTag;
					element = m_resitr->columnName( m_colidx);
					return true;
				}
				else
				{
					m_state = 6;
					++m_colidx;
					continue;
				}
			case 4:
				m_state = 5;
				type = TypedInputFilter::Value;
				element = (*m_rowitr)[ m_colidx];
				return true;
			case 5:
				m_state = 6;
				type = TypedInputFilter::CloseTag;
				element = TypedInputFilter::Element();
				++m_colidx;
				return true;
			case 6:
				m_state = 3;
				if (m_colidx == m_colend)
				{
					if (doSerializeWithIndices
					||  hasResultName( m_resitr->functionidx()))
					{
						type = TypedInputFilter::CloseTag;
						element = TypedInputFilter::Element();
						return true;
					}
				}
				continue;
		}
	}
};

TransactionFunctionOutput::TransactionFunctionOutput( const std::string& rootname_, const std::vector<std::string>& resname_, const std::vector<bool>& resunique_, const db::TransactionOutput& data_)
	:m_impl( new Impl( rootname_, resname_, resunique_, data_)){}

TransactionFunctionOutput::~TransactionFunctionOutput()
{
	delete m_impl;
}

bool TransactionFunctionOutput::getNext( ElementType& type, TypedFilterBase::Element& element)
{
	bool rt = m_impl->getNext( type, element, flag( SerializeWithIndices));
	return rt;
}

void TransactionFunctionOutput::resetIterator()
{
	m_impl->resetIterator();
}

TransactionFunction::Impl::Impl( const std::vector<TransactionDescription>& description, const types::keymap<TransactionFunctionR>& functionmap)
{
	typedef TransactionDescription::Error Error;
	TransactionDescription::ElementName elementName = TransactionDescription::Call;
	m_strings.push_back( '\0');

	std::vector<std::size_t> functionidx;
	std::vector<TransactionDescription>::const_iterator di = description.begin(), de = description.end();
	for (; di != de; ++di)
	{
		m_elemunique.push_back( di->unique);
		elementName = TransactionDescription::Call;
		std::size_t eidx = di - description.begin();
		// Parse the function call
		std::string::const_iterator ci = di->call.begin(), ce = di->call.end();
		utils::gotoNextToken( ci, ce);

		std::string functionname;
		while (ci < ce && isAlphaNumeric( *ci))
		{
			functionname.push_back( *ci);
			++ci;
		}
		if (functionname.empty())
		{
			throw Error( elementName, eidx, "identifier expected for name of function");
		}
		utils::gotoNextToken( ci, ce);
		if (*ci != '(')
		{
			throw Error( elementName, eidx, "'(' expected after function name");
		}
		++ci; utils::gotoNextToken( ci, ce);

		// Parse parameter list:
		std::vector<std::string> paramstr;
		if (*ci == ')')
		{
			// ... empty parameter list
			++ci;
		}
		else
		{
			for (;;)
			{
				std::string pp;
				while (ci < ce && *ci != ',' && *ci != ')')
				{
					pp.push_back( *ci);
					++ci;
				}
				boost::trim( pp);
				if (pp.empty())
				{
					throw Error( elementName, eidx, "empty element in parameter list");
				}
				paramstr.push_back( pp);

				utils::gotoNextToken( ci, ce);
				if (*ci == ')')
				{
					++ci;
					break;
				}
				else if (*ci == ',')
				{
					++ci; utils::gotoNextToken( ci, ce);
					continue;
				}
			}
		}
		if (utils::gotoNextToken( ci, ce))
		{
			throw Error( elementName, eidx, "unexpected token after function call");
		}

		// Build Function call object for parsed function:
		try
		{
			elementName = TransactionDescription::Selector;
			Path selector( di->selector, &m_tagmap, &m_strings);
			elementName = TransactionDescription::Call;
			std::vector<Path> param;
			std::vector<std::string>::const_iterator ai = paramstr.begin(), ae = paramstr.end();
			for (; ai != ae; ++ai)
			{
				Path pp( *ai, &m_tagmap, &m_strings);
				param.push_back( pp);
			}
			types::keymap<TransactionFunctionR>::const_iterator fui = functionmap.find( functionname);
			if (fui == functionmap.end())
			{
				FunctionCall cc( di->output, functionname, selector, param, di->nonempty, di->unique, 1);
				m_call.push_back( cc);
				functionidx.push_back( eidx);
			}
			else
			{
				Impl* func = fui->second->m_impl;
				std::map<int,int> rwtab = m_tagmap.insert( func->m_tagmap);

				if (di->output.size())
				{
					throw Error( elementName, eidx, "INTO not supported for call of OPERATION");
				}
				if (di->nonempty)
				{
					throw Error( elementName, eidx, "NONEMTY not supported for call of OPERATION");
				}
				if (di->unique)
				{
					throw Error( elementName, eidx, "UNIQUE not supported for call of OPERATION");
				}
				FunctionCall paramstk( "", "", selector, param, false, false, 2);
				m_call.push_back( paramstk);
				functionidx.push_back( eidx);

				std::vector<FunctionCall>::const_iterator fsi = func->m_call.begin(), fse = func->m_call.end();
				for (; fsi != fse; ++fsi)
				{
					std::string resultname;
					if (!fsi->resultname().empty())
					{
						if (!func->m_resultname.empty())
						{
							resultname.append( func->m_resultname);
						}
						resultname.append( "/");
						resultname.append( fsi->resultname());
					}
					Path fselector = fsi->selector();
					fselector.rewrite( rwtab);
					std::vector<Path> fparam = fsi->arg();
					std::vector<Path>::iterator fai = fparam.begin(), fae = fparam.end();
					for (; fai != fae; ++fai) fai->rewrite( rwtab);
					FunctionCall cc( resultname, fsi->name(), fselector, fparam, false, false, fsi->level() + 1);
					m_call.push_back( cc);
					functionidx.push_back( eidx);
				}
			}
		}
		catch (const std::runtime_error& e)
		{
			throw Error( elementName, eidx, e.what());
		}
	}

	// calculating common result name prefix:
	elementName = TransactionDescription::Output;
	std::vector<FunctionCall>::iterator ci = m_call.begin(), ce = m_call.end();
	m_resultname.clear();
	ci = m_call.begin(), ce = m_call.end();
	for (; ci != ce; ++ci)
	{
		std::size_t eidx = functionidx[ci - m_call.begin()];
		std::string prefix;
		const char* pp = std::strchr( ci->resultname().c_str(), '/');
		if (pp)
		{
			prefix.append( ci->resultname().c_str(), pp-ci->resultname().c_str());
			if (std::strchr( pp+1, '/'))
			{
				throw Error( elementName, eidx, "illegal result prefix. Only one '/' allowed");
			}
			if (m_resultname.empty())
			{
				m_resultname = prefix;
				ci->resultname( pp+1);
			}
			else if (m_resultname == prefix)
			{
				ci->resultname( pp+1);
			}
			else
			{
				throw Error( elementName, eidx, "no common result prefix");
			}
		}
		if (m_resultname == ".")
		{
			m_resultname = "";
		}
		if (!checkResultIdentifier( ci->resultname()) || !checkResultIdentifier( m_resultname))
		{
			throw Error( elementName, eidx, "'.' or identifier or two identifiers separated by a '/' expected for output");
		}
	}

	// checking the program
	elementName = TransactionDescription::Selector;
	ci = m_call.begin(), ce = m_call.end();
	for (; ci != ce; ++ci)
	{
		std::size_t eidx = functionidx[ci - m_call.begin()];
		if (ci->selector().resultReference())
		{
			throw Error( elementName, eidx, "undefined: result variable reference in selector");
		}
		if (ci->selector().constantReference())
		{
			throw Error( elementName, eidx, "undefined: constant as selector");
		}
	}
	elementName = TransactionDescription::Output;
	ci = m_call.begin();
	if (ci != ce)
	{
		if (ci->hasResultReference())
		{
			throw Error( elementName, 0, "result variable reference in first command leads to an empty result");
		}
	}
	ci = m_call.begin(), ce = m_call.end();
	for (; ci != ce; ++ci)
	{
		m_elemname.push_back( ci->resultname());
	}
}

TransactionFunction::Impl::Impl( const Impl& o)
	:m_resultname(o.m_resultname)
	,m_elemname(o.m_elemname)
	,m_call(o.m_call)
	,m_tagmap(o.m_tagmap)
	,m_strings(o.m_strings){}


TransactionFunction::TransactionFunction( const std::string& name_, const std::vector<TransactionDescription>& description, const types::keymap<TransactionFunctionR>& functionmap)
	:m_name(name_)
	,m_impl( new Impl( description, functionmap)){}

TransactionFunction::TransactionFunction( const TransactionFunction& o)
	:m_name(o.m_name)
	,m_impl( new Impl( *o.m_impl)){}

TransactionFunction::~TransactionFunction()
{
	delete m_impl;
}

const TransactionFunction::TagTable* TransactionFunction::tagmap() const
{
	return &m_impl->m_tagmap;
}

TransactionFunctionInput* TransactionFunction::getInput() const
{
	return new TransactionFunctionInput( this);
}

TransactionFunctionOutput* TransactionFunction::getOutput( const db::TransactionOutput& o) const
{
	return new TransactionFunctionOutput( m_impl->m_resultname, m_impl->m_elemname, m_impl->m_elemunique, o);
}

TransactionFunction* _Wolframe::db::createTransactionFunction( const std::string& name, const std::vector<TransactionDescription>& description, const types::keymap<TransactionFunctionR>& functionmap)
{
	return new TransactionFunction( name, description, functionmap);
}



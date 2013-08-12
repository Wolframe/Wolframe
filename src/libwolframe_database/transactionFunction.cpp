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
///\brief Implementation of the builder of transaction input and reader of output
///\file transactionFunction.cpp
#include "types/allocators.hpp"
#include "utils/parseUtils.hpp"
#include "database/transactionFunction.hpp"
#include "database/transaction.hpp"
#include "logger/logger-v1.hpp"
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
	};

	Node root() const;
	void next( const Node& nd, int tag, std::vector<Node>& rt) const;
	void find( const Node& nd, int tag, std::vector<Node>& rt) const;
	void up( const Node& nd, std::vector<Node>& rt) const;
	const types::Variant* nodevalue( const Node& nd) const;

	const std::string tostring() const;

	void setParentLinks( std::size_t mi);
	void openTag( const char* tag, std::size_t tagsize);
	void openTag( const std::string& tag);
	void closeTag();
	void createRootNode();
	void pushValue( const types::VariantConst& val);
	void check() const;

private:
	types::TypedArrayDoublingAllocator<Node> m_nodemem;
	std::vector<types::Variant> m_content;
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
		Root,
		Next,
		Find,
		Up,
		Result,
		Constant
	};

	static const char* elementTypeName( ElementType i)
	{
		static const char* ar[] ={"Root","Next","Find","Current","Up","Result","Constant"};
		return ar[(int)i];
	}

	struct Element
	{
		ElementType m_type;
		int m_tag;
	};

	Path(){}
	Path( const std::string& src, TransactionFunction::TagTable* tagmap);
	Path( const Path& o);
	std::string tostring() const;

	std::size_t resultReference() const;
	const char* constantReference() const;
	void selectNodes( const TransactionFunctionInput::Structure& st, const TransactionFunctionInput::Structure::Node& nd, std::vector<TransactionFunctionInput::Structure::Node>& ar) const;

	std::vector<Element>::const_iterator begin() const		{return m_path.begin();}
	std::vector<Element>::const_iterator end() const		{return m_path.end();}
	std::size_t size() const					{return m_path.size();}

	void rewrite( const std::map<int,int>& rwtab)
	{
		std::vector<Element>::iterator pi = m_path.begin(), pe = m_path.end();
		for (; pi != pe; ++pi)
		{
			if (pi->m_type == Next || pi->m_type == Find)
			{
				std::map<int,int>::const_iterator re = rwtab.find( pi->m_tag);
				if (re == rwtab.end()) throw std::logic_error( "rewrite table not complete");
				pi->m_tag = re->second;
			}
		}
	}

	void append( const Path& o)
	{
		m_path.insert( m_path.end(), o.begin(), o.end());
	}

private:
	std::vector<Element> m_path;
	std::string m_constant;
};

class FunctionCall
{
public:
	FunctionCall()
		:m_nonemptyResult(false)
		,m_uniqueResult(false){}
	FunctionCall( const FunctionCall& o);
	FunctionCall( const std::vector<std::string>& resname, const std::string& name, const Path& selector, const std::vector<Path>& arg, bool setNonemptyResult_, bool setUniqueResult_, std::size_t level_, const types::keymap<std::string>& hints_=types::keymap<std::string>());

	const Path& selector() const					{return m_selector;}
	const std::vector<Path>& arg() const				{return m_arg;}
	const std::string& name() const					{return m_name;}
	const std::vector<std::string>& resultnames() const		{return m_resultnames;}
	void resultnames( std::vector<std::string>& r)			{m_resultnames = r;}

	bool hasResultReference() const;
	bool hasNonemptyResult() const					{return m_nonemptyResult;}
	bool hasUniqueResult() const					{return m_uniqueResult;}
	std::size_t level() const					{return m_level;}

	const char* getErrorHint( const std::string& errorclass) const	{types::keymap<std::string>::const_iterator hi = m_hints.find( errorclass); return (hi==m_hints.end())?0:hi->second.c_str();}

private:
	std::vector<std::string> m_resultnames;
	std::string m_name;
	Path m_selector;
	std::vector<Path> m_arg;
	bool m_nonemptyResult;
	bool m_uniqueResult;
	std::size_t m_level;
	types::keymap<std::string> m_hints;
};

class ResultElement
{
public:
	enum Type {OpenTag,CloseTag,Value,FunctionStart,FunctionEnd,IndexStart,IndexEnd,OperationStart,OperationEnd};
	static const char* typeName( Type i)
	{
		static const char* ar[] = {"OpenTag","CloseTag","Value","FunctionStart","FunctionEnd","IndexStart","IndexEnd","OperationStart","OperationEnd"};
		return ar[ (int)i];
	}

	ResultElement()
		:m_type(CloseTag),m_idx(0){}
	ResultElement( Type type_, std::size_t idx_)
		:m_type(type_),m_idx(idx_){}
	ResultElement( const ResultElement& o)
		:m_type(o.m_type),m_idx(o.m_idx){}

	Type type() const		{return m_type;}
	std::size_t idx() const		{return m_idx;}

private:
	Type m_type;
	std::size_t m_idx;
};

class TransactionFunctionOutput::ResultStruct
{
public:
	ResultStruct()
	{
		m_strings.push_back( '\0');
	}
	ResultStruct( const ResultStruct& o)
		:m_ar(o.m_ar),m_strings(o.m_strings){}

	struct ContentElement
	{
		const char* value;
		ResultElement::Type type;
		std::size_t idx;

		ContentElement()
			:value(0),type(ResultElement::CloseTag),idx(0){}
		ContentElement( const ContentElement& o)
			:value(o.value),type(o.type),idx(o.idx){}
	};

	class const_iterator
	{
	public:
		explicit const_iterator( const ResultStruct* struct_=0)
			:m_struct(struct_)
		{
			if (m_struct)
			{
				m_itr = m_struct->m_ar.begin();
				init();
			}
		}
		const_iterator( const const_iterator& o)
			:m_content(o.m_content)
			,m_itr(o.m_itr)
			,m_struct(o.m_struct){}

		bool operator==(const const_iterator& o) const
		{
			if (!o.m_struct)
			{
				if (!m_struct) return true;
				if (m_itr == m_struct->m_ar.end()) return true;
			}
			else if (!m_struct)
			{
				if (!o.m_struct) return true;
				if (o.m_itr == o.m_struct->m_ar.end()) return true;
			}
			else if (m_struct == o.m_struct && m_itr == o.m_itr)
			{
				return true;
			}
			return false;
		}
		bool operator!=(const const_iterator& o) const
		{
			return !operator==(o);
		}

		const_iterator& operator++()	{if (m_struct) {++m_itr; init();} return *this;}
		const_iterator operator++(int)	{const_iterator rt=*this; ++(*this); return rt;}

		const ContentElement* operator->() const
		{
			return &m_content;
		}

		const ContentElement& operator*() const
		{
			return m_content;
		}

	private:
		void init()
		{
			if (m_itr != m_struct->m_ar.end())
			{
				m_content.value = (m_itr->type() != ResultElement::OpenTag)?0:(m_itr->idx()==0?0:m_struct->m_strings.c_str()+m_itr->idx());
				m_content.idx = m_itr->idx();
				m_content.type = m_itr->type();
			}
			else
			{
				m_content.value = 0;
				m_content.idx = 0;
				m_content.type = ResultElement::CloseTag;
			}
		}

	private:
		ContentElement m_content;
		std::vector<ResultElement>::const_iterator m_itr;
		const ResultStruct* m_struct;
	};

public:
	const_iterator begin() const
	{
		return const_iterator(this);
	}

	const_iterator end() const
	{
		return const_iterator();
	}

	void openTag( const std::string& name)
	{
		m_ar.push_back( ResultElement( ResultElement::OpenTag, m_strings.size()));
		m_strings.append( name);
		m_strings.push_back( '\0');
	}

	void closeTag()
	{
		m_ar.push_back( ResultElement( ResultElement::CloseTag, 0));
	}

	void addValueReference( std::size_t functionidx)
	{
		m_ar.push_back( ResultElement( ResultElement::Value, functionidx));
	}

	void addMark( ResultElement::Type mrk, std::size_t functionidx)
	{
		m_ar.push_back( ResultElement( mrk, functionidx));
	}

	void addEmbeddedResult( const ResultStruct& o, std::size_t functionidx)
	{
		const_iterator ri = o.begin(), re = o.end();
		for (; ri != re; ++ri)
		{
			switch (ri->type)
			{
				case ResultElement::Value:
					addValueReference( functionidx + ri->idx);
					break;
				case ResultElement::OpenTag:
					openTag( ri->value);
					break;
				case ResultElement::CloseTag:
					closeTag();
					break;
				case ResultElement::FunctionStart:
				case ResultElement::FunctionEnd:
				case ResultElement::IndexStart:
				case ResultElement::IndexEnd:
				case ResultElement::OperationStart:
				case ResultElement::OperationEnd:
					addMark( ri->type, functionidx + ri->idx);
					break;
			}
		}
	}

	std::string tostring() const
	{
		std::ostringstream rt;
		const_iterator ri = begin(), re = end();
		for (; ri != re; ++ri)
		{
			if (ri->type == ResultElement::OpenTag)
			{
				rt << ResultElement::typeName(ri->type) << " '" << (ri->value?ri->value:"") << "'; ";
			}
			else if (ri->type == ResultElement::CloseTag)
			{
				rt << ResultElement::typeName(ri->type) << "; ";
			}
			else
			{
				rt << ResultElement::typeName(ri->type) << " " << ri->idx<< "; ";
			}
		}
		return rt.str();
	}

private:
	std::vector<ResultElement> m_ar;
	std::string m_strings;
};


struct TransactionFunction::Impl
{
	TransactionFunctionOutput::ResultStructR m_resultstruct;
	std::vector<FunctionCall> m_call;
	TagTable m_tagmap;

	Impl( const std::vector<TransactionDescription>& description, const std::string& resultname, const types::keymap<TransactionFunctionR>& functionmap);
	Impl( const Impl& o);
};


static std::string normalizeTagName( const std::string& tagname)
{
	std::string rt;
	std::string::const_iterator ti = tagname.begin(), te = tagname.end();
	for (; ti != te; ++ti)
	{
		if (*ti == '\\')
		{
			++ti;
			if (ti != te && *ti == 'u')
			{
				// Unicode (UTF-16) excoded character support with \uXXXX
				unsigned short hexnum = 0;
				int ci;
				for (ci=0; ci<4 && ti != te; ++ti,++ci)
				{
					if (*ti >= '0' && *ti <= '9') hexnum = hexnum * 16 + (*ti - '0');
					else if (*ti >= 'a' && *ti <= 'f') hexnum = hexnum * 16 + 10 + (*ti - 'a');
					else if (*ti >= 'A' && *ti <= 'F') hexnum = hexnum * 16 + 10 + (*ti - 'A');
					else break;
				}
				if (ci == 0) throw std::runtime_error( "illegal character in element name");
				wchar_t wstr[2];
				wstr[0] = hexnum;
				wstr[1] = 0;
				char buf[32];
				sprintf( buf, "%ls", wstr);
				rt.append( buf);
			}
			else
			{
				rt.push_back( '\\');
			}
			--ti;
		}
		else
		{
			rt.push_back( *ti);
		}
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
	,m_content(o.m_content)
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
		pushValue( types::Variant());
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

void TransactionFunctionInput::Structure::pushValue( const types::VariantConst& val)
{
	std::size_t mi = m_content.size();
	m_content.push_back( val);
	m_data.back().push_back( Node( 0, 0, 0, Node::val_element( mi)));
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

const types::Variant* TransactionFunctionInput::Structure::nodevalue( const Node& nd) const
{
	const types::Variant* rt = 0;
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
				if (validx) rt = &m_content.at( validx);
			}
		}
	}
	return rt;
}

Path::Path( const std::string& pt, TransactionFunction::TagTable* tagmap)
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
			elem.m_tag = 0;
			m_constant.append( constant);
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
			if (*ii == '(') throw std::runtime_error( "unexpected token '(' - result reference $ + number expected");
			if (resno.size() == 0 || resno.size() > 999) throw std::runtime_error( "illegal result reference (only numbers between 1 and 999 allowed)");
			elem.m_tag = std::atoi( resno.c_str());
			if (elem.m_tag == 0) throw std::runtime_error( "referencing result with index 0");
			m_path.push_back( elem);
			continue;
		}
		else if (*ii == '/')
		{
			bool isRoot = (ii == pt.begin());
			++ii;
			if (ii == ee) break;
			if (*ii == '/')
			{
				elem.m_type = Find;
				++ii;
			}
			else
			{
				elem.m_type = isRoot?Root:Next;
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
	if (m_path.size() && m_path[0].m_type == Result)
	{
		return m_path[0].m_tag;
	}
	return 0;
}

const char* Path::constantReference() const
{
	if (m_path.size() && m_path[0].m_type == Constant) return m_constant.c_str();
	return 0;
}

std::string Path::tostring() const
{
	if (constantReference()) return m_constant;

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

				case Root:
					st.next( st.root(), si->m_tag, ar2);
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

FunctionCall::FunctionCall( const std::vector<std::string>& r, const std::string& n, const Path& s, const std::vector<Path>& a, bool q, bool u, std::size_t l, const types::keymap<std::string>& hints_)
	:m_resultnames(r)
	,m_name(n)
	,m_selector(s)
	,m_arg(a)
	,m_nonemptyResult(q)
	,m_uniqueResult(u)
	,m_level(l)
	,m_hints(hints_){}

FunctionCall::FunctionCall( const FunctionCall& o)
	:m_resultnames(o.m_resultnames)
	,m_name(o.m_name)
	,m_selector(o.m_selector)
	,m_arg(o.m_arg)
	,m_nonemptyResult(o.m_nonemptyResult)
	,m_uniqueResult(o.m_uniqueResult)
	,m_level(o.m_level)
	,m_hints(o.m_hints){}

bool FunctionCall::hasResultReference() const
{
	std::vector<Path>::const_iterator pi,pe;
	for (pi=m_arg.begin(),pe=m_arg.end(); pi!=pe; ++pi)
	{
		if (pi->resultReference()) return true;
	}
	return false;
}

TransactionFunctionInput::TransactionFunctionInput( const TransactionFunction* func_)
	:types::TypeSignature("database::TransactionFunctionInput", __LINE__)
	,m_structure( new Structure( func_->tagmap()))
	,m_func(func_)
	,m_lasttype( langbind::TypedInputFilter::Value){}

TransactionFunctionInput::~TransactionFunctionInput(){}

TransactionFunctionInput::TransactionFunctionInput( const TransactionFunctionInput& o)
	:types::TypeSignature("database::TransactionFunctionInput", __LINE__)
	,langbind::TypedOutputFilter(o)
	,m_structure( o.m_structure)
	,m_func(o.m_func)
	,m_lasttype(o.m_lasttype){}

bool TransactionFunctionInput::print( ElementType type, const types::VariantConst& element)
{
	LOG_DATA << "[transaction input] push element " << langbind::InputFilter::elementTypeName( type) << " '" << element.tostring() << "'";
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

static void bindArguments( TransactionInput& ti, const FunctionCall& call, const TransactionFunctionInput* inputst, const TransactionFunctionInput::Structure::Node& selectornode)
{
	typedef TransactionFunctionInput::Structure::Node Node;

	std::vector<Path>::const_iterator pi=call.arg().begin(), pe=call.arg().end();
	for (std::size_t argidx=1; pi != pe; ++pi,++argidx)
	{
		std::size_t idx;
		const char* cns;
		if ((idx = pi->resultReference()) != 0)
		{
			ti.bindCommandArgAsResultReference( idx);
		}
		else if ((cns = pi->constantReference()) != 0)
		{
			ti.bindCommandArgAsValue( types::VariantConst( cns));
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
				const types::Variant* valref = inputst->structure().nodevalue( *gs);
				if (valref)
				{
					ti.bindCommandArgAsValue( *valref);
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
		TransactionInput::cmd_iterator ti = rt.begin(), te = rt.end();
		for (; ti != te; ++ti)
		{
			if (ti->functionidx() == fidx)
			{
				if (ci->hasNonemptyResult())
				{
					ti->setNonemptyResult();
				}
				if (ci->hasUniqueResult())
				{
					ti->setUniqueResult();
				}
			}
		}

		// Select the nodes to execute the command with:
		std::vector<Node> nodearray;
		std::vector<Node>::const_iterator ni = rootnodearray.begin(), ne = rootnodearray.end();
		for (; ni != ne; ++ni)
		{
			ci->selector().selectNodes( this_->structure(), *ni, nodearray);
		}
		std::vector<FunctionCall>::const_iterator ca = ci;
		if (ci->level() > level)
		{
			if (!ci->name().empty()) throw std::logic_error("passing arguments expected when calling OPERATION");
			std::size_t nextfidx = fidx;
			for (++ca; ca != ce; ++ca,++nextfidx)
			{
				if (ca->level() < level || (ca->level() == level && ca->name().empty())) break;
			}
			std::vector<Node>::const_iterator vi=nodearray.begin(), ve=nodearray.end();
			for (; vi != ve; ++vi)
			{
				rt.startCommand( fidx, ci->level(), ci->name());
				bindArguments( rt, *ci, this_, *vi);
				std::vector<Node> opnodearray;
				opnodearray.push_back( *vi);
				getOperationInput( this_, rt, fidx+1, ci->level(), ci+1, ca, opnodearray);
			}
			ci = ca;
			ci--;
			fidx = nextfidx;
		}
		else
		{
			// For each selected node do expand the function call arguments:
			std::vector<Node>::const_iterator vi=nodearray.begin(), ve=nodearray.end();
			for (; vi != ve; ++vi)
			{
				rt.startCommand( fidx, ci->level(), ci->name());
				bindArguments( rt, *ci, this_, *vi);
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
	struct StackElement
	{
		ResultElement::Type m_type;
		std::size_t m_cnt;
		ResultStruct::const_iterator m_structitr;

		StackElement( ResultElement::Type type_, ResultStruct::const_iterator structitr_)
			:m_type(type_),m_cnt(0),m_structitr(structitr_){}
		StackElement( const StackElement& o)
			:m_type(o.m_type),m_cnt(o.m_cnt),m_structitr(o.m_structitr){}
	};

	int m_valuestate;
	int m_colidx;
	int m_colend;
	bool m_endofoutput;
	bool m_started;
	ResultStructR m_resultstruct;
	ResultStruct::const_iterator m_structitr;
	ResultStruct::const_iterator m_structend;
	std::vector<StackElement> m_stack;
	db::TransactionOutput::result_const_iterator m_resitr;
	db::TransactionOutput::result_const_iterator m_resend;
	std::vector<db::TransactionOutput::CommandResult::Row>::const_iterator m_rowitr;
	std::vector<db::TransactionOutput::CommandResult::Row>::const_iterator m_rowend;
	db::TransactionOutputR m_data;

	Impl( const ResultStructR& resultstruct_, const db::TransactionOutputR& data_)
		:m_valuestate(0)
		,m_colidx(0)
		,m_colend(0)
		,m_endofoutput(false)
		,m_resultstruct(resultstruct_)
		,m_data(data_)
	{
		resetIterator();
	}

	Impl( const Impl& o)
		:m_valuestate(o.m_valuestate)
		,m_colidx(o.m_colidx)
		,m_colend(o.m_colend)
		,m_endofoutput(o.m_endofoutput)
		,m_started(o.m_started)
		,m_resultstruct(o.m_resultstruct)
		,m_structitr(o.m_structitr)
		,m_structend(o.m_structend)
		,m_stack(o.m_stack)
		,m_resitr(o.m_resitr)
		,m_resend(o.m_resend)
		,m_rowitr(o.m_rowitr)
		,m_rowend(o.m_rowend)
		,m_data(o.m_data)
	{}

	void resetIterator()
	{
		m_valuestate = 0;
		m_colidx = 0;
		m_colend = 0;
		m_endofoutput = false;
		m_started = false;
		m_structitr = m_resultstruct->begin();
		m_structend = m_resultstruct->end();
		m_stack.clear();
		m_resitr = m_data->begin();
		m_resend = m_data->end();
	}

	bool getNext( ElementType& type, types::VariantConst& element, bool doSerializeWithIndices)
	{
		if (!m_started)
		{
			LOG_DATA << "[transaction result structure] " << m_resultstruct->tostring();
			m_started = true;
		}
		while (m_structitr != m_structend)
		{
			if (!doSerializeWithIndices)
			{
				if (m_structitr->type == ResultElement::IndexStart || m_structitr->type == ResultElement::IndexEnd)
				{
					++m_structitr;
					continue;
				}
			}
			switch (m_structitr->type)
			{
				case ResultElement::OpenTag:
					type = TypedInputFilter::OpenTag;
					element = m_structitr->value;
					++m_structitr;
					return true;

				case ResultElement::CloseTag:
					type = TypedInputFilter::CloseTag;
					element.init();
					++m_structitr;
					return true;

				case ResultElement::OperationStart:
					m_stack.push_back( StackElement( ResultElement::OperationEnd, m_structitr));
					++m_structitr;
					continue;

				case ResultElement::FunctionStart:
					if (m_resitr == m_resend || m_resitr->functionidx() > m_structitr->idx)
					{
						for (++m_structitr; m_structitr != m_structend && m_structitr->type != ResultElement::FunctionEnd; ++m_structitr);
						if (m_structitr == m_structend) throw std::logic_error("illegal stack in transaction result iterator");
						++m_structitr;
						continue;
					}
					else if (m_resitr->functionidx() < m_structitr->idx || m_resitr->begin() == m_resitr->end())
					{
						++m_resitr;
						continue;
					}
					m_stack.push_back( StackElement( ResultElement::FunctionEnd, m_structitr));
					++m_structitr;
					m_rowitr = m_resitr->begin();
					m_rowend = m_resitr->end();
					m_colidx = 0;
					m_colend = m_resitr->nofColumns();
					continue;

				case ResultElement::IndexStart:
					m_stack.push_back( StackElement( ResultElement::IndexEnd, m_structitr));
					++m_structitr;
					type = TypedInputFilter::OpenTag;
					element = types::VariantConst( (unsigned int)++m_stack.back().m_cnt);
					return true;

				case ResultElement::IndexEnd:
					if (m_valuestate == 0)
					{
						m_valuestate = 1;
						type = TypedInputFilter::CloseTag;
						element.init();
						return true;
					}
					m_valuestate = 0;
					if (m_stack.back().m_type != m_structitr->type)
					{
						throw std::logic_error( "illegal state in transaction result construction");
					}
					if (m_resitr != m_resend && m_resitr->functionidx() <= m_stack.back().m_structitr->idx)
					{
						m_structitr = m_stack.back().m_structitr;
						type = TypedInputFilter::OpenTag;
						element = types::VariantConst( (unsigned int)++m_stack.back().m_cnt);
						++m_structitr;
						return true;
					}
					else
					{
						m_stack.pop_back();
					}
					++m_structitr;
					continue;

				case ResultElement::OperationEnd:
					if (m_stack.back().m_type != m_structitr->type)
					{
						throw std::logic_error( "illegal state in transaction result construction (OperationEnd)");
					}
					if (m_resitr != m_resend && m_resitr->functionidx() <= m_stack.back().m_structitr->idx)
					{
						m_structitr = m_stack.back().m_structitr;
					}
					else
					{
						m_stack.pop_back();
					}
					++m_structitr;
					continue;

				case ResultElement::FunctionEnd:
					if (m_stack.back().m_type != m_structitr->type)
					{
						throw std::logic_error( "illegal state in transaction result construction (FunctionEnd)");
					}
					if (m_resitr != m_resend && m_resitr->functionidx() <= m_stack.back().m_structitr->idx)
					{
						m_structitr = m_stack.back().m_structitr;
					}
					else
					{
						m_stack.pop_back();
					}
					++m_structitr;
					continue;

				case ResultElement::Value:
					if (m_valuestate == 0)
					{
						if (m_rowitr == m_rowend)
						{
							++m_structitr;
							++m_resitr;
							if (m_resitr != m_resend)
							{
								m_rowitr = m_resitr->begin();
								m_rowend = m_resitr->end();
								m_colidx = 0;
								m_colend = m_resitr->nofColumns();
							}
							continue;
						}
						if (m_colidx == m_colend)
						{
							++m_structitr;
							++m_rowitr;
							if (m_rowitr == m_rowend)
							{
								++m_resitr;
								if (m_resitr != m_resend)
								{
									m_rowitr = m_resitr->begin();
									m_rowend = m_resitr->end();
									m_colidx = 0;
									m_colend = m_resitr->nofColumns();
								}
							}
							else
							{
								m_colidx = 0;
							}
							continue;
						}
						if (!m_rowitr->at(m_colidx).defined())
						{
							++m_colidx;
							continue;
						}
						type = TypedInputFilter::OpenTag;
						element = m_resitr->columnName( m_colidx);
						if (element.type() == types::Variant::string_ && element.charsize() == 0)
						{
							//... untagged content value (column name '_')
							type = langbind::TypedInputFilter::Value;
							element = m_rowitr->at(m_colidx);
							m_valuestate = 0;
							++m_colidx;
							return true;
						}
						else
						{
							m_valuestate = 1;
							return true;
						}
					}
					if (m_valuestate == 1)
					{
						type = langbind::TypedInputFilter::Value;
						element = m_rowitr->at(m_colidx);
						m_valuestate = 2;
						return true;
					}
					if (m_valuestate == 2)
					{
						type = langbind::TypedInputFilter::CloseTag;
						element.init();
						m_valuestate = 0;
						++m_colidx;
						return true;
					}
					throw std::logic_error( "illegal state (transaction result iterator)");
			}
		}
		if (!m_stack.empty())
		{
			throw std::logic_error( "illegal state (stack not empty after iterating transaction result)");
		}
		if (!m_endofoutput)
		{
			type = TypedInputFilter::CloseTag;
			element.init();
			m_endofoutput = true;
			return true;
		}
		return false;
	}
};


TransactionFunctionOutput::TransactionFunctionOutput( const ResultStructR& resultstruct_, const db::TransactionOutput& data_)
	:types::TypeSignature("database::TransactionFunctionOutput", __LINE__)
	,m_impl( new Impl( resultstruct_, db::TransactionOutputR( new db::TransactionOutput( data_))))
{}

TransactionFunctionOutput::TransactionFunctionOutput( const TransactionFunctionOutput& o)
	:types::TypeSignature("database::TransactionFunctionOutput", __LINE__)
	,langbind::TypedInputFilter(o)
	,m_impl( new Impl( *o.m_impl))
{}

TransactionFunctionOutput::~TransactionFunctionOutput()
{
	delete m_impl;
}

bool TransactionFunctionOutput::getNext( ElementType& type, types::VariantConst& element)
{
	bool rt = m_impl->getNext( type, element, flag( SerializeWithIndices));
	return rt;
}

void TransactionFunctionOutput::resetIterator()
{
	m_impl->resetIterator();
}

TransactionFunction::Impl::Impl( const std::vector<TransactionDescription>& description, const std::string& resultname, const types::keymap<TransactionFunctionR>& functionmap)
	:m_resultstruct( new TransactionFunctionOutput::ResultStruct())
{
	typedef TransactionDescription::Error Error;

	if (!resultname.empty())
	{
		m_resultstruct->openTag( resultname);
	}
	std::vector<TransactionDescription>::const_iterator di = description.begin(), de = description.end();
	for (; di != de; ++di)
	{
		std::size_t eidx = di - description.begin();

		// Build Function call object for parsed function:
		try
		{
			Path selector( di->selector, &m_tagmap);
			if (selector.resultReference())
			{
				throw Error( eidx, "undefined: result variable reference in selector");
			}
			if (selector.constantReference())
			{
				throw Error( eidx, "undefined: constant as selector");
			}
			std::vector<Path> param;
			std::vector<std::string>::const_iterator ai = di->call.second.begin(), ae = di->call.second.end();
			for (; ai != ae; ++ai)
			{
				Path pp( *ai, &m_tagmap);
				param.push_back( pp);
			}
			types::keymap<TransactionFunctionR>::const_iterator fui = functionmap.find( di->call.first);
			if (fui == functionmap.end())
			{
				FunctionCall cc( di->outputs, di->call.first, selector, param, di->nonempty, di->unique, 1, di->hints);
				if (!di->outputs.empty())
				{
					// this is just the wrapping structure, iteration is only done with the last tag (othwerwise
					// we get into definition problems)
					bool hasOutput = (di->outputs.size() > 1 || (di->outputs.size() == 1 && di->outputs[0] != "."));
					for (std::vector<std::string>::size_type i = 0; i < di->outputs.size()-1; i++)
					{
						m_resultstruct->openTag( di->outputs[i] );
					}
					m_resultstruct->addMark( ResultElement::FunctionStart, m_call.size());
					if (hasOutput) {
						m_resultstruct->openTag( di->outputs[di->outputs.size()-1]);
					}
					if (!di->unique) m_resultstruct->addMark( ResultElement::IndexStart, m_call.size());
					m_resultstruct->addValueReference( m_call.size());
					if (!di->unique) m_resultstruct->addMark( ResultElement::IndexEnd, m_call.size());
					if (hasOutput) {
						m_resultstruct->closeTag( );
					}
					m_resultstruct->addMark( ResultElement::FunctionEnd, m_call.size());
					for (std::vector<std::string>::size_type i = 0; i < di->outputs.size()-1; i++)
					{
						m_resultstruct->closeTag( );
					}
				}
				m_call.push_back( cc);
			}
			else
			{
				Impl* func = fui->second->m_impl;
				std::map<int,int> rwtab = m_tagmap.insert( func->m_tagmap);
				if (!di->hints.empty())
				{
					throw Error( eidx, "No ON ERROR hints supported for call of OPERATION");
				}
				if (di->nonempty)
				{
					throw Error( eidx, "NONEMTY not supported for call of OPERATION");
				}
				if (di->unique)
				{
					throw Error( eidx, "UNIQUE not supported for call of OPERATION");
				}
				if (!di->outputs.empty() && !di->outputs[0].empty( ) )
				{
					m_resultstruct->addMark( ResultElement::OperationStart, m_call.size());

					bool hasOutput = (di->outputs.size() > 1 || (di->outputs.size() == 1 && di->outputs[0] != "."));
					if (hasOutput)
					{
						std::vector<std::string>::const_iterator it;
						std::vector<std::string>::const_iterator end = di->outputs.end();
						for (it = di->outputs.begin( ); it != end; it++)
						{
							m_resultstruct->openTag( *it );
						}
					}
					m_resultstruct->addEmbeddedResult( *func->m_resultstruct, m_call.size()+1);
					if (hasOutput)
					{
						for (std::vector<std::string>::size_type i = 0; i < di->outputs.size(); i++)
						{
							m_resultstruct->closeTag();
						}
					}
				}
				FunctionCall paramstk( std::vector<std::string>(), "", selector, param, false, false, 1 + 1/*level*/);
				m_call.push_back( paramstk);

				std::vector<FunctionCall>::const_iterator fsi = func->m_call.begin(), fse = func->m_call.end();
				for (; fsi != fse; ++fsi)
				{
					Path fselector = fsi->selector();
					fselector.rewrite( rwtab);
					std::vector<Path> fparam = fsi->arg();
					std::vector<Path>::iterator fai = fparam.begin(), fae = fparam.end();
					for (; fai != fae; ++fai) fai->rewrite( rwtab);
					std::vector<std::string> rl;
					rl.push_back( resultname );
					FunctionCall cc( rl, fsi->name(), fselector, fparam, false, false, fsi->level() + 1);
					m_call.push_back( cc);
				}
				if (!di->outputs.empty() && !di->outputs[0].empty( ) )
				{
					m_resultstruct->addMark( ResultElement::OperationEnd, m_call.size());
				}
			}
		}
		catch (const std::runtime_error& e)
		{
			throw Error( eidx, e.what());
		}
	}
	if (!resultname.empty())
	{
		m_resultstruct->closeTag();
	}
}

TransactionFunction::Impl::Impl( const Impl& o)
	:m_resultstruct(o.m_resultstruct)
	,m_call(o.m_call)
	,m_tagmap(o.m_tagmap){}


TransactionFunction::TransactionFunction( const std::string& name_, const std::vector<TransactionDescription>& description, const std::string& resultname, const types::keymap<TransactionFunctionR>& functionmap, const langbind::Authorization& authorization_)
	:m_name(name_)
	,m_authorization(authorization_)
	,m_impl( new Impl( description, resultname, functionmap)){}

TransactionFunction::TransactionFunction( const TransactionFunction& o)
	:m_name(o.m_name)
	,m_authorization(o.m_authorization)
	,m_impl( new Impl( *o.m_impl)){}

TransactionFunction::~TransactionFunction()
{
	delete m_impl;
}

const TransactionFunction::TagTable* TransactionFunction::tagmap() const
{
	return &m_impl->m_tagmap;
}

const char* TransactionFunction::getErrorHint( const std::string& errorclass, int functionidx) const
{
	if (functionidx < 0 || functionidx >= (int)m_impl->m_call.size()) return 0;
	return m_impl->m_call.at( functionidx).getErrorHint( errorclass);
}

TransactionFunctionInput* TransactionFunction::getInput() const
{
	return new TransactionFunctionInput( this);
}

TransactionFunctionOutput* TransactionFunction::getOutput( const db::TransactionOutput& o) const
{
	return new TransactionFunctionOutput( m_impl->m_resultstruct, o);
}

TransactionFunction* _Wolframe::db::createTransactionFunction( const std::string& name, const std::vector<TransactionDescription>& description, const std::string& resultname, const types::keymap<TransactionFunctionR>& functionmap, const langbind::Authorization& auth)
{
	return new TransactionFunction( name, description, resultname, functionmap, auth);
}



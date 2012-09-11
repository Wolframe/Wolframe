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
///\brief Implementation of the processor of prepared statements in the database
///\file src/database_preparedStatementProcessor.cpp
#include "database/preparedStatement.hpp"
#include "database/preparedStatementProcessor.hpp"
#include "langbind/appGlobalContext.hpp"
#include "filter/token_filter.hpp"
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

int TagTable::find( const char* tag, std::size_t tagsize) const
{
	const std::string tagnam( tag, tagsize);
	return find( tagnam);
}

int TagTable::find( const std::string& tagnam) const
{
	std::map< std::string, int>::const_iterator ii = m_map.find( tagnam);
	if (ii == m_map.end()) return 0;
	return ii->second;
}

int TagTable::get( const std::string& tagnam)
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

int TagTable::get( const char* tag, std::size_t tagsize)
{
	const std::string tagstr( tag, tagsize);
	return get( tagstr);
}

int TagTable::unused() const
{
	return m_size +1;
}

bool Structure::Node::operator == (const Node& o) const
{
	if (m_parent != o.m_parent) return false;
	if (m_tag != o.m_tag) return false;
	if (m_element != o.m_element) return false;
	if (m_elementsize != o.m_elementsize) return false;
	return true;
}

Structure::Structure( const Structure& o)
	:m_nodemem(o.m_nodemem)
	,m_strmem(o.m_strmem)
	,m_tagmap(o.m_tagmap)
	,m_rootidx(o.m_rootidx)
	,m_rootsize(o.m_rootsize)
	{}


Structure::Structure( const TagTable* tagmap)
	:m_tagmap(tagmap)
	,m_rootidx(0)
	,m_rootsize(0)
{
	m_nodemem.alloc( 1);
	m_strmem.alloc( 1);
	m_data.push_back( std::vector<Node>());
}

void Structure::check() const
{
	if (m_data.size() != 1)
	{
		throw std::runtime_error( "tags not balanced in structure");
	}
}

void Structure::setParentLinks( std::size_t mi)
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

const std::string Structure::tostring() const
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

void Structure::openTag( const char* tag, std::size_t tagsize)
{
	const std::string tagstr( tag, tagsize);
	openTag( tagstr);
}

void Structure::openTag( const std::string& tagstr)
{
	int mi = (int)m_tagmap->find( tagstr);
	if (mi == 0) mi = (int)m_tagmap->unused();

	m_data.back().push_back( Node( 0, mi, 0, 0));
	m_data.push_back( std::vector<Node>());
}

void Structure::createRootNode()
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

void Structure::closeTag()
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

void Structure::pushValue( const char* val, std::size_t valsize)
{
	std::size_t mi = m_strmem.alloc( valsize+1);
	char* mem = m_strmem.base() + mi;
	std::memcpy( mem, val, (valsize+1)*sizeof(char));

	m_data.back().push_back( Node( 0, 0, valsize, Node::val_element( mi)));
}

void Structure::pushValue( const std::string& val)
{
	pushValue( val.c_str(), val.size());
}

void Structure::next( const Node& nd, int tag, std::vector<Node>& nextnd) const
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

void Structure::find( const Node& nd, int tag, std::vector<Node>& findnd) const
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

void Structure::up( const Node& nd, std::vector<Node>& rt) const
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

Structure::Node Structure::root() const
{
	Node rt( 0, 0, m_rootsize, Node::ref_element(m_rootidx));
	return rt;
}

const char* Structure::nodevalue( const Node& nd) const
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

Path::Path( const std::string& pt, TagTable* tagmap)
{
	Element elem;
	std::string::const_iterator ii = pt.begin(), ee = pt.end();
	while (ii != ee)
	{
		if (*ii == '$')
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

void Path::selectNodes( const Structure& st, const Structure::Node& nd, std::vector<Structure::Node>& ar) const
{
	std::vector<Structure::Node> ar1,ar2;
	ar1.push_back( nd);

	// [B.1] Find selected nodes:
	std::vector<Element>::const_iterator si = begin(), se = end();
	for (; si != se; ++si)
	{
		ar2.clear();
		std::vector<Structure::Node>::const_iterator ni = ar1.begin(), ne = ar1.end();
		for (; ni != ne; ++ni)
		{
			switch (si->m_type)
			{
				case Result:
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

FunctionCall::FunctionCall( const std::string& r, const std::string& n, const Path& s, const std::vector<Path>& a)
	:m_resultname(r)
	,m_name(n)
	,m_selector(s)
	,m_arg(a) {}

FunctionCall::FunctionCall( const FunctionCall& o)
	:m_resultname(o.m_resultname)
	,m_name(o.m_name)
	,m_selector(o.m_selector)
	,m_arg(o.m_arg) {}

bool FunctionCall::hasResultReference() const
{
	std::vector<Path>::const_iterator pi,pe;
	for (pi=m_arg.begin(),pe=m_arg.end(); pi!=pe; ++pi)
	{
		if (pi->resultReference()) return true;
	}
	return false;
}

PreparedStatementTransactionFunction::PreparedStatementTransactionFunction( const PreparedStatementTransactionFunction& o)
	:m_database(o.m_database)
	,m_resultname(o.m_resultname)
	,m_call(o.m_call)
	,m_tagmap(o.m_tagmap){}

static bool isAlphaNumeric( char ch)
{
	if (ch >= '0' && ch <= '9') return true;
	if (ch >= 'A' && ch <= 'Z') return true;
	if (ch >= 'a' && ch <= 'z') return true;
	if (ch == '_') return true;
	return false;
}

static void skipSpaces( std::string::const_iterator& ii, std::string::const_iterator& ee)
{
	while (ii < ee && *ii > 0 && *ii <= 32) ++ii;
}

static void nextToken( std::string::const_iterator& ii, std::string::const_iterator& ee)
{
	skipSpaces(ii,ee);
	if (ii == ee) throw std::runtime_error( "unexpected end of expression");
}

static std::string parseParameter( std::string::const_iterator& ii, std::string::const_iterator& ee)
{
	std::string rt;
	while (ii < ee && *ii != ',' && *ii != ')')
	{
		rt.push_back( *ii);
		++ii;
	}
	boost::trim( rt);
	if (rt.empty()) throw std::runtime_error( "empty element in parameter list");
	return rt;
}

PreparedStatementTransactionFunction::PreparedStatementTransactionFunction( const std::string& src)
{
	std::string::const_iterator ii = src.begin(), ee = src.end();
	while (ii != ee)
	{
		// Parse result name and function name "bla/bla=func(" or "bla=func(" of "func(":
		nextToken(ii,ee);
		std::string resname,functionname;
		while (ii < ee && (isAlphaNumeric( *ii) || *ii == '/'))
		{
			functionname.push_back( *ii);
			++ii;
		}
		nextToken(ii,ee);
		if (*ii == '=')
		{
			++ii; nextToken(ii,ee);
			resname = functionname;
			functionname.clear();
			for (; ii < ee && isAlphaNumeric( *ii); ++ii)
			{
				functionname.push_back( *ii);
			}
			if (functionname.empty()) throw std::runtime_error("expected identifier for function name after '='");
		}
		nextToken(ii,ee);
		if (*ii != '(') throw std::runtime_error( "syntax error in expression '(' expected");
		++ii; nextToken(ii,ee);

		// Parse selector "/doc/aa:":
		std::string selectorstr;
		while (ii < ee && *ii != ':' && *ii != ',' && *ii != ')' && *ii != '(' && *ii != ';')
		{
			selectorstr.push_back( *ii);
			++ii;
		}
		nextToken(ii,ee);
		if (*ii != ':') throw std::runtime_error( "expected ':' after selection");
		++ii; nextToken(ii,ee);

		// Parse parameter list:
		std::vector<std::string> paramstr;
		if (*ii == ')')
		{
			// ... empty parameter list
			++ii;
		}
		else
		{
			for (;;)
			{
				paramstr.push_back( parseParameter( ii, ee));
				nextToken(ii,ee);
				if (*ii == ')')
				{
					++ii;
					break;
				}
				else if (*ii == ',')
				{
					++ii; nextToken(ii,ee);
					continue;
				}
			}
		}

		// Build Function call object for parsed function:
		Path selector( selectorstr, &m_tagmap);
		std::vector<Path> param;
		std::vector<std::string>::const_iterator ai = paramstr.begin(), ae = paramstr.end();
		for (; ai != ae; ++ai)
		{
			Path pp( *ai, &m_tagmap);
			param.push_back( pp);
		}
		FunctionCall cc( resname, functionname, selector, param);
		m_call.push_back( cc);

		// Skip to end of next semicolon that starts a new function call definition:
		skipSpaces(ii,ee);
		if (ii == ee) break;
		if (*ii != ';') throw std::runtime_error( "missing semicolon as expression separator");
		++ii; skipSpaces(ii,ee);
		if (ii == ee) throw std::runtime_error( "superfluous semicolon at end of expression. ';' is a separator and not the terminator of a function call definition");
	}

	// calculating common result name prefix:
	std::vector<FunctionCall>::iterator ci = m_call.begin(), ce = m_call.end();
	m_resultname.clear();
	ci = m_call.begin(), ce = m_call.end();
	for (; ci != ce; ++ci)
	{
		std::string prefix;
		const char* pp = std::strchr( ci->resultname().c_str(), '/');
		if (pp)
		{
			prefix.append( ci->resultname().c_str(), pp-ci->resultname().c_str());
			if (std::strchr( pp+1, '/')) throw std::runtime_error( "Illegal result prefix. Only one '/' allowed");

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
				throw std::runtime_error( "no common result prefix");
			}
		}
	}

	// checking the program
	ci = m_call.begin(), ce = m_call.end();
	for (; ci != ce; ++ci)
	{
		if (ci->selector().resultReference())
		{
			throw std::runtime_error( "undefined: result variable reference as selector");
		}
	}
	ci = m_call.begin();
	if (ci != ce)
	{
		if (ci->hasResultReference())
		{
			throw std::runtime_error( "result variable reference in first command leads to an empty result");
		}
	}
}

TransactionInput::TransactionInput( const TagTable* tagmap)
	:Structure(tagmap)
	,m_lasttype(langbind::TypedInputFilter::Value){}

TransactionInput::TransactionInput( const TransactionInput& o)
	:langbind::TransactionFunction::Input(o)
	,Structure(o)
	,m_lasttype(o.m_lasttype){}

bool TransactionInput::print( ElementType type, const Element& element)
{
	switch (type)
	{
		case langbind::TypedInputFilter::OpenTag:
			openTag( element.tostring());
		break;
		case langbind::TypedInputFilter::CloseTag:
			closeTag();
		break;
		case langbind::TypedInputFilter::Attribute:
			openTag( element.tostring());
		break;
		case langbind::TypedInputFilter::Value:
			pushValue( element.tostring());
			if (m_lasttype == langbind::TypedInputFilter::Attribute)
			{
				closeTag();
			}
		break;
	}
	m_lasttype = type;
	return true;
}

TransactionResult::TransactionResult( const TransactionResult& o)
	:langbind::TransactionFunction::Result(o)
	,m_itemar(o.m_itemar)
	,m_itemitr(o.m_itemar.begin()+(o.m_itemitr-o.m_itemar.begin())){}

void TransactionResult::openTag( const std::string& tag)
{
	m_itemar.push_back( Item( langbind::InputFilter::OpenTag, tag));
}

void TransactionResult::openTag( const char* tag, std::size_t tagsize)
{
	m_itemar.push_back( Item( langbind::InputFilter::OpenTag, std::string( tag, tagsize)));
}

void TransactionResult::closeTag()
{
	m_itemar.push_back( Item( langbind::InputFilter::CloseTag, std::string()));
}

void TransactionResult::pushValue( const std::string& val)
{
	m_itemar.push_back( Item( langbind::InputFilter::Value, val));
}

void TransactionResult::pushValue( const char* val, std::size_t valsize)
{
	m_itemar.push_back( Item( langbind::InputFilter::Value, std::string( val, valsize)));
}

bool TransactionResult::getNext( ElementType& type, Element& element)
{
	if (m_itemitr == m_itemar.end()) return false;
	type = m_itemitr->first;
	element = langbind::TypedFilterBase::Element( m_itemitr->second);
	++m_itemitr;
	return true;
}

void TransactionResult::finalize()
{
	m_itemitr = m_itemar.begin();
}

langbind::TransactionFunction::InputR PreparedStatementTransactionFunction::getInput() const
{
	langbind::TransactionFunction::InputR rt( new TransactionInput( &m_tagmap));
	return rt;
}

typedef std::vector<std::size_t> ResultRow;

///\brief Execute one single command of the transaction
static void getCommandResults( PreparedStatementHandler* dbi, std::vector<ResultRow>& resultar, std::string& resultstr)
{
	ResultRow res;
	std::size_t ii, nn=dbi->nofColumns();
	if (nn) do
	{
		for (ii=1; ii<=nn; ++ii)
		{
			const char* resstr = dbi->get( ii);
			if (resstr)
			{
				res.push_back( resultstr.size());
				resultstr.append( resstr);
				resultstr.push_back( '\0');
			}
			else
			{
				res.push_back( 0);
			}
		}
		resultar.push_back( res);
		res.clear();
	}
	while (dbi->next());
	if (dbi->getLastError()) throw std::runtime_error( "fetch next result in transaction failed");
}

///\brief Print the result of a command if it is named
static void printCommandResult( PreparedStatementHandler* dbi, const FunctionCall& call, TransactionResult* result, const std::vector<ResultRow>& resultar, const std::string& resultstr)
{
	if (!call.resultname().empty())
	{
		// Get the result column names. They are printed as tag names
		std::vector<std::string> columnar;
		columnar.clear();
		std::size_t ii, nn=dbi->nofColumns();
		for (ii=1; ii<=nn; ++ii)
		{
			const char* colname = dbi->columnName( ii);
			if (!colname) throw std::runtime_error( "cannot retrieve a result column name");
			columnar.push_back( colname);
		}

		// Print the result rows
		std::vector<ResultRow>::const_iterator ri = resultar.begin(), re = resultar.end();
		for (; ri != re; ++ri)
		{
			result->openTag( call.resultname());
			for (ii=0; ii<nn; ++ii)
			{
				if ((*ri)[ii])
				{
					result->openTag( columnar[ ii]);
					result->pushValue( resultstr.c_str() + (*ri)[ii]);
					result->closeTag();
				}
			}
			result->closeTag();
		}
	}
}

// Bind the arguments that are input node references:
static void bindNodeReferenceArguments( PreparedStatementHandler* dbi, const FunctionCall& call, const TransactionInput* inputst, const Structure::Node& selectornode)
{
	std::vector<Path>::const_iterator pi=call.arg().begin(), pe=call.arg().end();
	for (std::size_t argidx=1; pi != pe; ++pi,++argidx)
	{
		if (!pi->resultReference())
		{
			std::vector<Structure::Node> param;
			pi->selectNodes( *inputst, selectornode, param);
			if (param.size() > 1)
			{
				std::vector<Structure::Node>::const_iterator gs = param.begin(), gi = param.begin()+1, ge = param.end();
				for (; gi != ge; ++gi)
				{
					if (*gs != *gi) throw std::runtime_error( "more than one node selected in db call argument");
				}
				const char* value = inputst->nodevalue( *gs);
				if (!dbi->bind( argidx, value)) throw std::runtime_error( "bind paramater in transaction failed");
			}
			else if (param.size() == 0)
			{
				if (!dbi->bind( argidx, 0)) throw std::runtime_error( "bind NULL parameter in transaction failed");
			}
			else
			{
				const char* value = inputst->nodevalue( param[0]);
				if (!dbi->bind( argidx, value)) throw std::runtime_error( "bind paramater in transaction failed");
			}
		}
	}
}

// Bind the arguments that are result references (one row):
static void bindResultRowReferenceArguments( PreparedStatementHandler* dbi, const FunctionCall& call, const ResultRow& row, const std::string& resultstr)
{
	std::vector<Path>::const_iterator pi=call.arg().begin(), pe=call.arg().end();
	for (std::size_t argidx=1; pi != pe; ++pi,++argidx)
	{
		std::size_t residx = pi->resultReference();
		if (residx)
		{
			if (residx > row.size())
			{
				throw std::runtime_error( "accessing non existing index in result row when binding parameters");
			}
			std::size_t resstridx = row[ residx -1];
			if (resstridx)
			{
				if (!dbi->bind( argidx, resultstr.c_str() + resstridx)) throw std::runtime_error( "bind paramater in transaction failed");
			}
			else
			{
				if (!dbi->bind( argidx, 0)) throw std::runtime_error( "bind paramater in transaction failed");
			}
		}
	}
}


langbind::TransactionFunction::ResultR PreparedStatementTransactionFunction::execute( const langbind::TransactionFunction::Input* inputi) const
{
	PreparedStatementHandler* dbi = 0;
	try
	{
		PreparedStatementHandlerR dbiref;
		langbind::GlobalContext* gct = langbind::getGlobalContext();
		if (!gct->getPreparedStatementHandler( m_database, dbiref))
		{
			if (m_database.empty())
			{
				throw std::runtime_error( "default database prepared statement handler is not defined");
			}
			else
			{
				throw std::runtime_error( std::string("prepared statement handler for database '") + m_database + "' is not defined");
			}
		}
		dbi = dbiref.get();

		const TransactionInput* inputst = dynamic_cast<const TransactionInput*>( inputi);
		if (!inputst) throw std::logic_error( "function called with unknown input type");

		TransactionResult* result = new TransactionResult();
		langbind::TransactionFunction::ResultR rt( result);

		std::string resultstr,resultstr2;
		std::vector<ResultRow> resultar,resultar2;

		if (!dbi->begin()) throw std::runtime_error( "transaction begin failed");
		if (!m_resultname.empty())
		{
			result->openTag( m_resultname);
		}
		std::vector<FunctionCall>::const_iterator ci = m_call.begin(), ce = m_call.end();
		for (; ci != ce; ++ci)
		{
			resultar = resultar2;
			resultstr = resultstr2;
			resultar2.clear();
			resultstr2.clear();
			resultstr2.push_back( '\0');

			// Start transaction:
			if (!dbi->start( ci->name())) throw std::runtime_error( "start transaction failed");

			// Select the nodes to execute the command with:
			std::vector<Structure::Node> nodearray;
			Structure::Node root = inputst->root();
			ci->selector().selectNodes( *inputst, root, nodearray);

			// For each selected node do expand the function call arguments:
			std::vector<Structure::Node>::const_iterator vi=nodearray.begin(), ve=nodearray.end();
			for (; vi != ve; ++vi)
			{
				bindNodeReferenceArguments( dbi, *ci, inputst, *vi);

				// Expand the arguments that are result references, and execute the command for each result row:
				if (ci->hasResultReference())
				{
					std::vector<ResultRow>::const_iterator ri = resultar.begin(), re = resultar.end();
					for (; ri != re; ++ri)
					{
						bindResultRowReferenceArguments( dbi, *ci, *ri, resultstr);
						if (!dbi->execute())
						{
							throw std::runtime_error( "execute database function in transaction failed");
						}
						std::vector<FunctionCall>::const_iterator nextcall = ci + 1;
						std::vector<ResultRow> resultar2_singlecmd;
						if (!ci->resultname().empty() || (nextcall != ce && nextcall->hasResultReference()))
						{
							getCommandResults( dbi, resultar2_singlecmd, resultstr2);
							resultar2.insert( resultar2.end(), resultar2_singlecmd.begin(), resultar2_singlecmd.end());
						}
						printCommandResult( dbi, *ci, result, resultar2_singlecmd, resultstr2);
					}
				}
				else
				{
					// If we do not have result references then the command is complete and executed once
					if (!dbi->execute())
					{
						throw std::runtime_error( "execute database function in transaction failed");
					}
					std::vector<FunctionCall>::const_iterator nextcall = ci + 1;
					std::vector<ResultRow> resultar2_singlecmd;
					if (!ci->resultname().empty() || (nextcall != ce && nextcall->hasResultReference()))
					{
						getCommandResults( dbi, resultar2_singlecmd, resultstr2);
						resultar2.insert( resultar2.end(), resultar2_singlecmd.begin(), resultar2_singlecmd.end());
					}
					printCommandResult( dbi, *ci, result, resultar2_singlecmd, resultstr2);
				}
			}
		}
		if (!dbi->commit()) throw std::runtime_error( "commit transaction failed");
		if (!m_resultname.empty())
		{
			result->closeTag();
		}
		result->finalize();
		return rt;
	}
	catch (const std::exception& e)
	{
		if (!dbi) throw std::runtime_error( e.what());

		const char* dberr = dbi->getLastError();
		std::string dberrstr( dberr?dberr:"unspecified database error");
		dbi->rollback();
		throw std::runtime_error( std::string("error in database transaction: ") + e.what() + " (" + dberrstr + ")");
	}
}

langbind::TransactionFunction* createPreparedStatementTransactionFunction( const std::string& description)
{
	return new PreparedStatementTransactionFunction( description);
}



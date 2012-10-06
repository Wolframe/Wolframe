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
///\file src/libwolframe_langbind/databaseTransactionFunction.cpp
#include "langbind/databaseTransactionFunction.hpp"
#include "database/transaction.hpp"
#include "processor/procProvider.hpp"
#include "types/countedReference.hpp"
#include "utils/miscUtils.hpp"
#include "textwolf/xmlscanner.hpp"
#include "textwolf/cstringiterator.hpp"
#include "textwolf/charset.hpp"
#include <cstring>
#include <sstream>
#include <iostream>
#include <locale>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

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

DatabaseTransactionFunction::DatabaseTransactionFunction( const DatabaseTransactionFunction& o)
	:TransactionFunction(o)
	,m_resultname(o.m_resultname)
	,m_call(o.m_call)
	,m_tagmap(o.m_tagmap)
	,m_provider(o.m_provider){}

static bool isAlphaNumeric( char ch)
{
	if (ch >= '0' && ch <= '9') return true;
	if (ch >= 'A' && ch <= 'Z') return true;
	if (ch >= 'a' && ch <= 'z') return true;
	if (ch == '_') return true;
	return false;
}

bool checkIdentifier( const std::string& id)
{
	std::string::const_iterator ii = id.begin(), ie = id.end();
	while (ii != ie && isAlphaNumeric( *ii)) ++ii;
	return (ii != ie);
}

DatabaseTransactionFunction::DatabaseTransactionFunction( const proc::ProcessorProvider* provider_, const std::vector<TransactionDescription>& description)
	:m_provider(provider_)
{
	typedef TransactionDescription::Error Error;
	TransactionDescription::ElementName elementName = TransactionDescription::Call;

	std::vector<TransactionDescription>::const_iterator di = description.begin(), de = description.end();
	for (; di != de; ++di)
	{
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
			Path selector( di->selector, &m_tagmap);
			elementName = TransactionDescription::Call;
			std::vector<Path> param;
			std::vector<std::string>::const_iterator ai = paramstr.begin(), ae = paramstr.end();
			for (; ai != ae; ++ai)
			{
				Path pp( *ai, &m_tagmap);
				param.push_back( pp);
			}
			FunctionCall cc( di->output, functionname, selector, param);
			m_call.push_back( cc);
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
		std::string prefix;
		const char* pp = std::strchr( ci->resultname().c_str(), '/');
		if (pp)
		{
			prefix.append( ci->resultname().c_str(), pp-ci->resultname().c_str());
			if (std::strchr( pp+1, '/'))
			{
				throw Error( elementName, ci - m_call.begin(), "illegal result prefix. Only one '/' allowed");
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
				throw Error( elementName, ci - m_call.begin(), "no common result prefix");
			}
		}
		if (!checkIdentifier( ci->resultname()) || !checkIdentifier( m_resultname))
		{
			throw Error( elementName, ci - m_call.begin(), "identifier or two identifiers separated by a '/' expected for output");
		}
	}

	// checking the program
	elementName = TransactionDescription::Selector;
	ci = m_call.begin(), ce = m_call.end();
	for (; ci != ce; ++ci)
	{
		if (ci->selector().resultReference())
		{
			throw Error( elementName, ci - m_call.begin(), "undefined: result variable reference in selector");
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
}

TransactionFunctionInput::TransactionFunctionInput( const TagTable* tagmap)
	:Structure(tagmap)
	,m_lasttype(langbind::TypedInputFilter::Value){}

TransactionFunctionInput::TransactionFunctionInput( const TransactionFunctionInput& o)
	:langbind::TransactionFunction::Input(o)
	,Structure(o)
	,m_lasttype(o.m_lasttype){}

bool TransactionFunctionInput::print( ElementType type, const Element& element)
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


langbind::TransactionFunction::InputR DatabaseTransactionFunction::getInput() const
{
	langbind::TransactionFunction::InputR rt( new TransactionFunctionInput( &m_tagmap));
	return rt;
}

static void bindArguments( db::TransactionInput& ti, const FunctionCall& call, const TransactionFunctionInput* inputst, const Structure::Node& selectornode)
{
	std::vector<Path>::const_iterator pi=call.arg().begin(), pe=call.arg().end();
	for (std::size_t argidx=1; pi != pe; ++pi,++argidx)
	{
		std::size_t residx = pi->resultReference();
		if (residx)
		{
			ti.bindCommandArgAsResultReference( residx);
		}
		else
		{
			std::vector<Structure::Node> param;
			pi->selectNodes( *inputst, selectornode, param);
			if (param.size() == 0)
			{
				ti.bindCommandArgAsNull();
			}
			else
			{
				std::vector<Structure::Node>::const_iterator gs = param.begin(), gi = param.begin()+1, ge = param.end();
				for (; gi != ge; ++gi)
				{
					if (*gs != *gi) throw std::runtime_error( "more than one node selected in db call argument");
				}
				const char* value = inputst->nodevalue( *gs);
				ti.bindCommandArgAsValue( value);
			}
		}
	}
}

db::TransactionInput DatabaseTransactionFunction::databaseTransactionInput( const TransactionFunctionInput& inputst) const
{
	db::TransactionInput rt;
	std::vector<FunctionCall>::const_iterator ci = m_call.begin(), ce = m_call.end();
	for (; ci != ce; ++ci)
	{
		// Select the nodes to execute the command with:
		std::vector<Structure::Node> nodearray;
		Structure::Node root = inputst.root();
		ci->selector().selectNodes( inputst, root, nodearray);

		// For each selected node do expand the function call arguments:
		std::vector<Structure::Node>::const_iterator vi=nodearray.begin(), ve=nodearray.end();
		for (; vi != ve; ++vi)
		{
			rt.startCommand( ci - m_call.begin(), ci->name());
			bindArguments( rt, *ci, &inputst, *vi);
		}
	}
	return rt;
}

TransactionFunctionResult::TransactionFunctionResult( const std::string& rootname_, const std::vector<std::string>& resname_, const db::TransactionOutput& o)
	:db::TransactionOutput(o)
	,m_state(0)
	,m_rowidx(0)
	,m_colidx(0)
	,m_colend(0)
	,m_rootname(rootname_)
	,m_resname(resname_){}

bool TransactionFunctionResult::getNext( ElementType& type, TypedFilterBase::Element& element)
{
	for (;;) switch (m_state)
	{
		case 0:
			m_resitr = db::TransactionOutput::begin();
			m_resend = db::TransactionOutput::end();
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
				continue;
			}
			m_rowitr = m_resitr->begin();
			m_rowend = m_resitr->end();
			m_rowidx = 0;
			m_colidx = 0;
			m_colend = m_resitr->nofColumns();
			m_state = 2;
			/* no break here ! */
		case 2:
			m_colidx = 0;
			if (m_rowitr == m_rowend)
			{
				++m_resitr;
				m_state = 1;
				continue;
			}
			m_state = 3;
			type = TypedInputFilter::OpenTag;
			element = m_resname[ m_resitr->functionidx()];
			return true;
		case 3:
			if (m_colidx == m_colend)
			{
				++m_rowitr;
				++m_rowidx;
				m_state = 2;
				continue;
			}
			m_state = 4;
			type = TypedInputFilter::OpenTag;
			element = m_resitr->columnName( m_colidx);
			return true;
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
				type = TypedInputFilter::CloseTag;
				element = TypedInputFilter::Element();
				return true;
			}
			continue;

	}
}

langbind::TransactionFunction::ResultR DatabaseTransactionFunction::execute( const langbind::TransactionFunction::Input* inputi) const
{
	db::Transaction* trs = 0;
	types::CountedReference<db::Transaction> trsr;
	try
	{
		if (!m_provider)
		{
			throw std::runtime_error( "no provider defined for getting database access");
		}
		trsr.reset( trs = m_provider->transaction( name()));
		if (!trs)
		{
			throw std::runtime_error( std::string("could not allocate transaction object for '") + name() + "'");
		}
		const TransactionFunctionInput* inputst = dynamic_cast<const TransactionFunctionInput*>( inputi);
		if (!inputst) throw std::logic_error( "function called with unknown input type");

		std::vector<std::string> elemNames;
		std::vector<FunctionCall>::const_iterator ci = m_call.begin(), ce = m_call.end();
		for (; ci != ce; ++ci)
		{
			elemNames.push_back( ci->resultname());
		}
		trs->putInput( databaseTransactionInput( *inputst));
		trs->execute();
		langbind::TransactionFunction::ResultR rt( new TransactionFunctionResult( m_resultname, elemNames, trs->getResult()));
		return rt;
	}
	catch (const std::exception& e)
	{
		throw std::runtime_error( std::string("error in database transaction '") + TransactionFunction::name() + "': " + e.what());
	}
}

langbind::TransactionFunction* _Wolframe::langbind::createDatabaseTransactionFunction( const proc::ProcessorProvider* provider_, const std::vector<TransactionDescription>& description)
{
	return new DatabaseTransactionFunction( provider_, description);
}



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
///\brief Implementation of the processor of database commands
///\file src/database_processor.cpp
#include "database/processor.hpp"
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

Structure::Structure( const Structure& o)
	:m_nodemem(o.m_nodemem)
	,m_strmem(o.m_strmem)
	,m_tagmap(o.m_tagmap)
	,m_rootidx(o.m_rootidx)
	{}


Structure::Structure( const TagTable* tagmap)
	:m_tagmap(tagmap)
	,m_rootidx(0)
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
		cd->m_parent = mi;
		setParentLinks( ci + ii);
	}
}

const std::string Structure::tostring() const
{
	std::vector <std::pair< std::size_t, std::size_t> > stk;
	std::ostringstream rt;
	stk.push_back( std::pair< std::size_t, std::size_t>( 1, m_rootidx));
	while (stk.size())
	{
		std::size_t ii = stk.back().first;
		std::size_t mi = stk.back().second;
		--stk.back().first;
		if (ii>0)
		{
			Node* cd = &m_nodemem[ mi + ii - 1];
			if (cd->valueidx())
			{
				std::size_t indent = stk.size() -1;
				const char* val = nodevalue( cd->valueidx());
				while (indent--) rt << '\t';
				rt << cd->m_tag << ": '" << val << "'" << std::endl;
			}
			else
			{
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
	int mi = -(int)m_tagmap->find( tagstr);
	if (mi == 0) mi = -(int)m_tagmap->unused();

	m_data.back().push_back( Node( 0, mi, 0, 0));
	m_data.push_back( std::vector<Node>());
}

void Structure::closeTag()
{
	if (m_data.back().size())
	{
		std::size_t ni = m_data.back().size();
		std::size_t mi = m_nodemem.alloc( ni);
		Node* nd = &m_nodemem[mi];
		std::vector<Node>::const_iterator itr = m_data.back().begin(), end = m_data.back().end();
		for (; itr != end; ++itr, ++nd)
		{
			*nd = *itr;
		}
		m_data.pop_back();
		m_data.back().back().m_elementsize = ni;
		m_data.back().back().m_element = Node::ref_element( mi);
		if (m_data.size() == 1)
		{
			m_rootidx = mi;
			setParentLinks( m_rootidx);
		}
	}
	else
	{
		m_data.pop_back();
		if (m_data.back().size() && m_data.back().back().m_element)
		{
			std::size_t mi = m_nodemem.alloc( 1);
			Node* nd = &m_nodemem[mi];
			std::vector<Node>::const_iterator itr = m_data.back().begin();
			*nd = *itr;
			if (m_data.size() == 1)
			{
				m_rootidx = mi;
				setParentLinks( m_rootidx);
			}
		}
	}
}

void Structure::pushValue( const char* val, std::size_t valsize)
{
	std::size_t mi = m_strmem.alloc( valsize+1);
	if (mi > (std::size_t)std::numeric_limits<int>::max()) throw std::bad_alloc();
	char* nd = m_strmem.base() + mi;
	std::memcpy( nd, val, (valsize+1)*sizeof(char));
	if (m_data.size() < 2)
	{
		throw std::logic_error( "pushed value as root element");
	}
	if (m_data.back().size() > 0)
	{
		throw std::logic_error( "pushed value without tag context");
	}
	if (m_data[ m_data.size()-2].back().m_element != 0)
	{
		throw std::logic_error( "pushed more thatn one value in same tag context");
	}
	m_data[ m_data.size()-2].back().m_elementsize = valsize;
	m_data[ m_data.size()-2].back().m_element = Node::val_element( mi);
}

void Structure::pushValue( const std::string& val)
{
	pushValue( val.c_str(), val.size());
}

bool Structure::next( const Node& nd, int tag, std::vector<Node>& nextnd) const
{
	bool rt = false;
	std::size_t ii = 0, nn = nd.nofchild();
	Node* cd = &m_nodemem[ nd.childidx()];
	for (; ii<nn; ++ii)
	{
		if (tag == 0 || tag == cd[ ii].m_tag)
		{
			nextnd.push_back( cd[ ii]);
			rt = true;
		}
	}
	return rt;
}

bool Structure::find( const Node& nd, int tag, std::vector<Node>& findnd) const
{
	bool rt = false;
	std::size_t ii = 0, nn = nd.nofchild();
	Node* cd = &m_nodemem[ nd.childidx()];
	for (; ii<nn; ++ii)
	{
		if (tag == 0 || tag == cd[ ii].m_tag) findnd.push_back( cd[ ii]);
		if (cd[ ii].nofchild() > 0) rt |= find( cd[ ii], tag, findnd);
	}
	return rt;
}

bool Structure::up( const Node& nd, std::vector<Node>& rt) const
{
	if (nd.m_parent != 0)
	{
		rt.push_back( m_nodemem[ nd.m_parent]);
		return true;
	}
	return false;
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
			elem.m_tag = std::atoi( resno.c_str());
			if (elem.m_tag == 0) throw std::runtime_error( "referencing result with index 0");
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

bool Path::selectNodes( const Structure& st, const Structure::Node& nd, std::vector<Structure::Node>& ar) const
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
	ar = ar1;
	return (ar.size() > 0);
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

TransactionFunction::TransactionFunction( const TransactionFunction& o)
	:m_resultname(o.m_resultname)
	,m_call(o.m_call)
	,m_tagmap(o.m_tagmap){}

TransactionFunction::TransactionFunction( const std::string& src)
{
	std::string::const_iterator ii = src.begin(), ee = src.end();
	for (; ii != ee; ++ii)
	{
		while (ii < ee && *ii > 0 && *ii < 32) ++ii;
		if (ii == ee) throw std::runtime_error( "unexpected end of expression");
		std::string resname,functionname;
		while (ii < ee && (std::isalnum( *ii) || *ii == '/'))
		{
			functionname.push_back( *ii);
			++ii;
		}
		if (ii < ee && *ii == '=')
		{
			resname = functionname;
			functionname.clear();
			for (++ii; ii < ee && std::isalnum( *ii); ++ii)
			{
				functionname.push_back( *ii);
			}
		}
		if (ii == ee) throw std::runtime_error( "unexpected end of expression");
		while (ii < ee && *ii > 0 && *ii < 32) ++ii;
		if (ii == ee) throw std::runtime_error( "unexpected end of expression");
		if (*ii != '(') throw std::runtime_error( "syntax error in expression '(' expected");
		++ii;
		std::string selectorstr;
		while (ii < ee && *ii > 0 && *ii < 32) ++ii;
		if (ii == ee) throw std::runtime_error( "unexpected end of expression");
		while (ii < ee && *ii != ':')
		{
			selectorstr.push_back( *ii);
			++ii;
		}
		if (ii == ee) throw std::runtime_error( "unexpected end of expression");
		while (ii < ee && *ii > 0 && *ii < 32) ++ii;
		if (ii == ee) throw std::runtime_error( "unexpected end of expression");
		std::vector<std::string> paramstr;

		for (;;)
		{
			std::string pp;
			while (ii < ee && *ii > 0 && *ii < 32) ++ii;
			if (ii == ee) throw std::runtime_error( "unexpected end of expression");
			while (ii < ee && *ii != ',' && *ii != ')')
			{
				pp.push_back( *ii);
				++ii;
			}
			boost::trim( pp);
			if (ii == ee) throw std::runtime_error( "unexpected end of expression");
			paramstr.push_back( pp);
			if (*ii == ')')
			{
				++ii;
				while (ii < ee && *ii > 0 && *ii < 32) ++ii;
				break;
			}
			else if (*ii == ',')
			{
				++ii;
				continue;
			}
		}
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

		while (ii < ee && *ii > 0 && *ii < 32) ++ii;
		if (ii == ee) break;
		if (*ii != ';') throw std::runtime_error( "missing semicolon as expression separator");
	}

	// calculating common result name prefix
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
			if (std::strchr( pp+1, '/')) throw std::runtime_error( "illegal result prefix");

			if (m_resultname.empty())
			{
				m_resultname = prefix;
				ci->resultname( pp+1);
			}
			else if (m_resultname != prefix)
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

struct DatabaseError :public std::runtime_error
{
	DatabaseError( DatabaseInterface* dbi)
		:std::runtime_error( std::string("error in database transaction (") + dbi->getLastError())
	{
		dbi->rollback();
	}
};

TransactionInput::TransactionInput( const TagTable* tagmap)
	:Structure(tagmap)
	,m_lasttype(langbind::TypedInputFilter::Value){}

TransactionInput::TransactionInput( const TransactionInput& o)
	:Structure(o)
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
	:m_itemar(o.m_itemar)
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
	element = m_itemitr->second;
	++m_itemitr;
	return true;
}

void TransactionResult::finalize()
{
	m_itemitr = m_itemar.begin();
}

langbind::TransactionInputR TransactionFunction::getInput() const
{
	langbind::TransactionInputR rt( new TransactionInput( &m_tagmap));
	return rt;
}

langbind::TransactionResultR TransactionFunction::execute( DatabaseInterface* dbi, const langbind::TransactionInput* inputi) const
{
	const TransactionInput* inputst = dynamic_cast<const TransactionInput*>( inputi);
	if (!inputst) throw std::logic_error( "function called with unknown input type");

	std::string ststr = inputst->tostring();
/*[-]*/std::cout << "INPUT" << std::endl << ststr << std::endl << std::endl;

	TransactionResult* result = new TransactionResult();
	langbind::TransactionResultR rt( result);

	typedef std::vector<std::size_t> ResultRow;
	std::string resultstr,resultstr2;
	std::vector<ResultRow> resultar,resultar2;

	if (!dbi->begin()) throw DatabaseError( dbi);
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
		if (!dbi->start( ci->name())) throw DatabaseError( dbi);

		// Select the nodes to execute the command with:
		std::vector<Structure::Node> nodearray;
		if (!ci->selector().selectNodes( *inputst, inputst->root(), nodearray)) continue;

		// For each selected node do expand the function call arguments:
		std::vector<Structure::Node>::const_iterator vi=nodearray.begin(),ve=nodearray.end();
		for (; vi != ve; ++vi)
		{
			// Expand the arguments that are input node references:
			std::vector<Path>::const_iterator pi=ci->arg().begin(), pe=ci->arg().end();
			for (std::size_t argidx=0; pi != pe; ++pi,++argidx)
			{
				if (!pi->resultReference())
				{
					std::vector<Structure::Node> param;
					if (!pi->selectNodes( *inputst, *vi, param))
					{
						if (param.size() > 1)
						{
							throw std::runtime_error( "more than one node selected in db call argument");
						}
						else if (param.size() == 0)
						{
							if (!dbi->bind( argidx, 0)) throw DatabaseError( dbi);
						}
						else
						{
							std::size_t idx = param[0].valueidx();
							if (idx == 0) throw std::runtime_error( "non atomic value selected in db call argument");

							if (!dbi->bind( argidx, inputst->nodevalue( idx))) throw DatabaseError( dbi);
						}
					}
				}
			}

			// Expand the arguments that are result references:
			if (ci->hasResultReference())
			{
				std::vector<ResultRow>::const_iterator ri = resultar.begin(), re = resultar.end();
				for (; ri != re; ++ri)
				{
					for (std::size_t argidx=0; pi != pe; ++pi,++argidx)
					{
						std::size_t residx = pi->resultReference();
						if (residx)
						{
							std::size_t resstridx =  (*ri)[ residx -1];
							if (resstridx)
							{
								if (!dbi->bind( argidx, resultstr.c_str() + resstridx)) throw DatabaseError( dbi);
							}
							else
							{
								if (!dbi->bind( argidx, 0)) throw DatabaseError( dbi);
							}
						}
					}
					if (!dbi->execute()) throw DatabaseError( dbi);
					ResultRow res;
					std::size_t ii, nn=dbi->nofColumns();
					while (dbi->next())
					{
						for (ii=0; ii<nn; ++ii)
						{
							const char* resstr = dbi->get( ii);
							if (resstr)
							{
								res.push_back( resultstr2.size());
								resultstr2.append( resstr);
								resultstr2.push_back( '\0');
							}
							else
							{
								res.push_back( 0);
							}
						}
						resultar.push_back( res);
						res.clear();
					}
					if (dbi->getLastError()) throw DatabaseError( dbi);
				}
			}
			else
			{
				if (!dbi->execute()) throw DatabaseError( dbi);
				ResultRow res;
				std::size_t ii, nn=dbi->nofColumns();
				while (dbi->next())
				{
					for (ii=0; ii<nn; ++ii)
					{
						const char* resstr = dbi->get( ii);
						if (resstr)
						{
							res.push_back( resultstr2.size());
							resultstr2.append( resstr);
							resultstr2.push_back( '\0');
						}
						else
						{
							res.push_back( 0);
						}
					}
					resultar.push_back( res);
					res.clear();
				}
				if (dbi->getLastError()) throw DatabaseError( dbi);
			}

			// print the result if it is named:
			if (!ci->resultname().empty())
			{
				std::vector<std::string> columnar;
				result->openTag( ci->resultname());
				columnar.clear();
				std::size_t ii, nn=dbi->nofColumns();
				for (ii=0; ii<nn; ++ii)
				{
					const char* colname = dbi->columnName( ii);
					columnar.push_back( colname);
				}

				std::vector<ResultRow>::const_iterator ri = resultar.begin(), re = resultar.end();
				for (; ri != re; ++ri)
				{
					for (ii=0; ii<nn; ++ii)
					{
						if ((*ri)[ii])
						{
							result->openTag( columnar[ ii]);
							result->pushValue( resultstr2.c_str() + (*ri)[ii]);
							result->closeTag();
						}
					}
				}
				result->closeTag();
			}
		}
	}
	if (!dbi->commit()) throw DatabaseError( dbi);
	if (!m_resultname.empty())
	{
		result->closeTag();
	}
	return rt;
}




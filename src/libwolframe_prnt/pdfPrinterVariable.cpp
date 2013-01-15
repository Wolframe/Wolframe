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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file prnt_pdfPrinterVariable.cpp
#include "prnt/pdfPrinterVariable.hpp"
#include "types/bcdArithmetic.hpp"
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::prnt;

const char* _Wolframe::prnt::variableName( Variable::Id v)
{
	static const char* ar[] = {
		"R0","R1","R2","R3","R4","R5","R6","R7","R8","R9",
		"Text",
		"Align",
		"Index",
		"PositionX",
		"PositionY",
		"SizeX",
		"SizeY",
		"LineStyle",
		"Font",
		"FontSize",
		"Unit",
		0};
	return ar[ (int)v];
}

namespace {
struct VarnameMap :public std::map <std::string, std::size_t>
{
	static std::string unifyKey( const std::string& key)
	{
		return boost::algorithm::to_lower_copy(key);
	}

	VarnameMap()
	{
		for (std::size_t ii=0; variableName( (Variable::Id)ii); ++ii)
		{
			std::string key( unifyKey( variableName( (Variable::Id)ii)));
			(*this)[ key] = ii;
		}
	}
};
}//anonymous namespace

Variable::Id _Wolframe::prnt::variableId( const std::string& name)
{
	static VarnameMap map;
	std::string key( map.unifyKey( name));
	std::map <std::string, std::size_t>::const_iterator itr = map.find( key);
	if (itr == map.end()) throw std::runtime_error( std::string( "unknown variable '") + name + "'");
	return (Variable::Id)itr->second;
}

VariableScope::VariableScope()
{
	m_strings.push_back('\0');
	m_scopeid.push_back( 0);
	m_ar.push_back( Area());
}

// Find out if to tag scopes are adressing the same context.
// For example /a[0]/b and /a[1]/b are not sibling and though not adressing the same context
// But /a/b[0] and /a/b[1] on the other hand are siblings and therefore adressing the same context
static bool isSameTagScope( const std::vector<int>& scopeid1, const std::vector<int>& scopeid2)
{
	if (scopeid1.size() < 2 || scopeid2.size() < 2 || scopeid1.size() != scopeid2.size())
	{
		throw std::runtime_error( "internal: unexpected state in tag scope comparison");
	}
	std::vector<int>::const_iterator itr1 = scopeid1.begin(), end1 = scopeid1.begin() + scopeid1.size() - 2;
	std::vector<int>::const_iterator itr2 = scopeid2.begin(), end2 = scopeid2.begin() + scopeid2.size() - 2;

	for (; itr1 != end1 && itr2 != end2 && *itr1 == *itr2; ++itr1,++itr2);
	return (itr1 == end1 && itr2 == end2);
}

void VariableScope::push()
{
	m_ar.push_back( Area( m_ar.back(), m_tag.size()));
	++m_scopeid.back();
	m_scopeid.push_back( 0);
	m_ar.back().m_mrk.clear();
	if (m_ar.size() != m_scopeid.size()) throw std::logic_error( "internal: unexpected tag stack state");
}

void VariableScope::push( const std::string& tag)
{

	push();

	m_tag.push_back( '/');
	m_tag.append( tag);

	std::map< std::string, TagContext >::const_iterator ti = m_tagvarmap.find( m_tag), te = m_tagvarmap.end();
	if (ti != te)
	{
		// Check if the found match is in the same tag scope
		if (isSameTagScope( ti->second.m_scopeid, m_scopeid))
		{
			// If yes, then take all its variable definitions into the local definition scope
			std::map<Variable::Id,std::size_t>::const_iterator di = ti->second.m_var.begin(), de = ti->second.m_var.end();
			for (; di != de; ++di)
			{
				m_ar.back().m_map[ di->first] = di->second;
			}
		}
	}
}

void VariableScope::pop()
{
	m_tag.resize( m_ar.back().m_tagidx);
	m_ar.pop_back();
	m_scopeid.pop_back();
	if (m_ar.empty() || m_scopeid.empty()) throw std::logic_error( "non existing variable scope closed");
}

void VariableScope::pushDefinitionToTagContext( Variable::Id var, std::size_t val)
{
	std::map< std::string, TagContext >::iterator ti = m_tagvarmap.find( m_tag), te = m_tagvarmap.end();
	if (ti != te)
	{
		// Check if the new definition is in the same tag scope than the definitions before
		if (!isSameTagScope( ti->second.m_scopeid, m_scopeid))
		{
			// If not, then clear the old context, because it will not be
			// used anymore and create a new one for the current tag scope:
			ti->second.m_var.clear();
			ti->second.m_scopeid = m_scopeid;
		}
		// Take all variables pushed to this tag context to the local context:
		ti->second.m_var[ var] = val;
	}
	else
	{
		// Take all variables pushed to this tag context to the local context:
		m_tagvarmap[ m_tag].m_var[ var] = val;
		m_tagvarmap[ m_tag].m_scopeid = m_scopeid;
	}
}

void VariableScope::define( Variable::Id var, const std::string& value, bool passToSibling)
{
	std::size_t val = m_strings.size();
	m_ar.back().m_map[ var] = val;
	m_strings.append( value);
	m_strings.push_back('\0');
	if (passToSibling)
	{
		pushDefinitionToTagContext( var, val);
	}
}

void VariableScope::define( Variable::Id var, Variable::Id src, bool passToSibling)
{
	std::size_t val = getValueIdx( src);
	if (!val) throw std::runtime_error( std::string( "undefined variable in scope '") + variableName(src) + "'");
	m_ar.back().m_map[var] = val;
	if (passToSibling)
	{
		pushDefinitionToTagContext( var, val);
	}
}

std::size_t VariableScope::getValueIdx( Variable::Id var) const
{
	std::map<Variable::Id,std::size_t>::const_iterator vi = m_ar.back().m_map.find( var);
	return (vi == m_ar.back().m_map.end())?0:vi->second;
}

std::string VariableScope::getValue( std::size_t idx) const
{
	return std::string( m_strings.c_str() + idx);
}

ValueStack::ValueStack()
{
	m_strings.push_back( '\0');
}

void ValueStack::push( const std::string& value)
{
	m_valuear.push_back( m_strings.size());
	m_strings.append( value);
	m_strings.push_back( '\0');
}

void ValueStack::pop( std::size_t nof)
{
	while (nof--) m_valuear.pop_back();
}

std::string ValueStack::top( std::size_t idx) const
{
	if (idx >= m_valuear.size()) throw std::logic_error("internal: too few elements on stack for required operation");
	return std::string( m_strings.c_str() + m_valuear[ m_valuear.size()-1-idx]);
}

std::string ValueStack::dump() const
{
	std::ostringstream rt;
	int ii = 1;
	std::vector<std::size_t>::const_iterator vi = m_valuear.begin() + m_valuear.size(), vb = m_valuear.begin();
	while (vi > vb)
	{
		--vi;
		--ii;
		rt << (int)ii << ": '" << std::string( m_strings.c_str() + *vi) << "'" << std::endl;
	}
	return rt.str();
}



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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file prnt_pdfPrinterVariable.cpp
#include "prnt/pdfPrinterVariable.hpp"
#include <stdexcept>
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::prnt;

const char* _Wolframe::prnt::variableName( Variable v)
{
	static const char* ar[] = {
		"Index",
		"Value",
		"PositionX",
		"PositionY",
		"SizeX",
		"SizeY",
		0};
	return ar[ (int)v];
}

namespace {
struct VarnameMap :public std::map <std::string, std::size_t>
{
	VarnameMap()
	{
		for (std::size_t ii=0; variableName( (Variable)ii); ++ii)
		{
			(*this)[ variableName( (Variable)ii)] = ii;
		}
	}
};
}//anonymous namespace

std::map <std::string, std::size_t>* _Wolframe::prnt::getVariablenameMap()
{
	static VarnameMap rt;
	return &rt;
}

VariableValue VariableValue::string( std::size_t idx)
{
	VariableValue rt;
	rt.m_type = String;
	rt.value.m_string = idx;
	return rt;
}

VariableValue VariableValue::number( double value)
{
	VariableValue rt;
	rt.m_type = Number;
	rt.value.m_number = value;
	return rt;
}

std::string VariableValue::asString( const std::string strvalar) const
{
	switch (m_type)
	{
		case VariableValue::Number:
			return boost::lexical_cast<std::string>( value.m_number);

		case VariableValue::String:
			return std::string( strvalar.c_str() + value.m_string);
	}
	throw std::logic_error("illegal state");
}

double VariableValue::asNumber( const std::string strvalar) const
{
	switch (m_type)
	{
		case VariableValue::Number:
			return value.m_number;

		case VariableValue::String:
			std::string strval = strvalar.c_str() + value.m_string;
			return boost::lexical_cast<double>( strval);
	}
	throw std::logic_error("illegal state");
}

VariableScope::VariableScope()
{
	m_strings.push_back('\0');
	m_valuear.push_back( VariableValue::string(0));
	m_ar.push_back( Map());
}

void VariableScope::push()
{
	m_ar.push_back( Map());
}

void VariableScope::pop()
{
	m_ar.pop_back();
	if (m_ar.empty()) throw std::logic_error( "non existing variable scope closed");
}

void VariableScope::define( Variable var, const std::string& value)
{
	if (m_ar.back().find( var) != m_ar.back().end()) throw std::runtime_error( std::string( "duplicate definition of variable '") + variableName(var) + "'");
	m_valuear.push_back( VariableValue::string( m_strings.size()));
	m_strings.append( value);
	m_strings.push_back('\0');
	m_ar.back()[var] = m_valuear.size()-1;
}

void VariableScope::define( Variable var, double value)
{
	if (m_ar.back().find( var) != m_ar.back().end()) throw std::runtime_error( std::string( "duplicate definition of variable '") + variableName(var) + "'");
	m_valuear.push_back( VariableValue::number( value));
	m_ar.back()[var] = m_valuear.size()-1;
}

void VariableScope::define( Variable var, Variable src)
{
	std::size_t val = getValueIdx( src);
	if (m_ar.back().find( var) != m_ar.back().end()) throw std::runtime_error( std::string( "duplicate definition of variable '") + variableName(var) + "'");
	if (!val) throw std::runtime_error( std::string( "undefined variable in scope '") + variableName(src) + "'");
	m_ar.back()[var] = val;
}

std::size_t VariableScope::getValueIdx( Variable var)
{
	std::vector<Map>::const_iterator itr = m_ar.begin()+m_ar.size();
	while (itr != m_ar.begin())
	{
		--itr;
		Map::const_iterator vi = m_ar.back().find( var);
		if (vi != m_ar.back().end())
		{
			return vi->second;
		}
	}
	return 0;
}

VariableValue::Type VariableScope::getType( std::size_t idx) const
{
	return m_valuear[idx].type();
}

std::string VariableScope::getString( std::size_t idx)
{
	const VariableValue& val = m_valuear[ idx];
	return val.asString( m_strings);
}

double VariableScope::getNumber( std::size_t idx)
{
	const VariableValue& val = m_valuear[ idx];
	return val.asNumber( m_strings);
}

void ValueStack::push( const std::string& value)
{
	m_valuear.push_back( VariableValue::string( m_strings.size()));
	m_strings.append( value);
}

void ValueStack::push( double value)
{
	m_valuear.push_back( VariableValue::number( value));
}

ValueStack::ValueStack()
{
	m_strings.push_back( '\0');
}

void ValueStack::pop( std::size_t nof)
{
	while (nof--) m_valuear.pop_back();
}

VariableValue::Type ValueStack::type( std::size_t idx) const
{
	if (idx >= m_valuear.size()) throw std::logic_error("internal: too few elements on stack for required operation");
	return m_valuear.at( m_valuear.size()-1-idx).type();
}

double ValueStack::asNumber( std::size_t idx) const
{
	if (idx >= m_valuear.size()) throw std::logic_error("internal: too few elements on stack for required operation");
	return m_valuear.at( m_valuear.size()-1-idx).asNumber( m_strings);
}

std::string ValueStack::asString( std::size_t idx) const
{
	if (idx >= m_valuear.size()) throw std::logic_error("internal: too few elements on stack for required operation");
	return m_valuear.at( m_valuear.size()-1-idx).asString( m_strings);
}



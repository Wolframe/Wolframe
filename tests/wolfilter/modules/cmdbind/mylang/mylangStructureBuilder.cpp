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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file mylangStructureBuilder.cpp
///\brief Implementation of mylang data structure building
#include "mylangStructureBuilder.hpp"
#include "logger-v1.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;
using namespace _Wolframe::langbind::mylang;

StructureBuilder::StructureBuilder()
{
	m_stk.push_back( StructureR());
}

void StructureBuilder::openElement( const std::string& elemid_)
{
	LOG_DATA << "[mylang structure builder] open structure element '" << elemid_ << "'";
	if (!m_stk.back().get())
	{
		m_stk.back().reset( new Structure());
	}
	std::vector<Structure::KeyValuePair>::const_iterator si = m_stk.back()->m_struct.begin(), se = m_stk.back()->m_struct.end();
	for (; si != se; ++si)
	{
		if (si->first.type() != types::Variant::String) continue;
		if (si->first.tostring() == elemid_)
		{
			throw std::runtime_error( std::string("duplicate definition of element '") + elemid_ + "'");
		}
	}
	m_stk.back()->m_struct.push_back( Structure::KeyValuePair( elemid_, StructureR()));
	m_stk.push_back( StructureR());
}

void StructureBuilder::openArrayElement()
{
	if (!m_stk.back().get())
	{
		m_stk.back().reset( new Structure());
	}
	std::vector<Structure::KeyValuePair>::const_iterator si = m_stk.back()->m_struct.begin(), se = m_stk.back()->m_struct.end();
	if (!m_stk.back()->m_array)
	{
		for (; si != se; ++si)
		{
			if (si->first.type() == types::Variant::String) throw std::runtime_error( "mixing array with structure");
		}
		m_stk.back()->m_array = true;
	}
	// add element at end of array:
	unsigned int aridx = m_stk.back()->m_struct.size()+1;
	LOG_DATA << "[mylang structure builder] open array element " << aridx;

	m_stk.back()->m_struct.push_back( Structure::KeyValuePair( aridx, StructureR()));
	m_stk.push_back( StructureR());
}

void StructureBuilder::closeElement()
{
	LOG_DATA << "[mylang structure builder] close element";

	if (m_stk.size() <= 1) throw std::runtime_error("element tags not balanced (close)");
	StructureR elem = m_stk.back();
	m_stk.pop_back();
	if (elem.get())
	{
		m_stk.back()->m_struct.back().second = elem;
	}
	else
	{
		m_stk.back()->m_struct.pop_back();
	}
}

void StructureBuilder::setValue( const types::Variant& value_)
{
	LOG_DATA << "[mylang structure builder] set value '" << value_.tostring() << "'";
	if (!m_stk.back().get())
	{
		m_stk.back().reset( new Structure());
	}
	if (!value_.defined()) throw std::runtime_error("set undefined value");
	if (m_stk.back()->m_value.defined()) throw std::runtime_error("value already defined");
	if (!m_stk.back()->m_struct.empty()) throw std::runtime_error("setValue for atomic value called for a structure");
	m_stk.back()->m_value = value_;
}

unsigned int StructureBuilder::lastArrayIndex() const
{
	if (!m_stk.back().get()) return 0;
	if (!m_stk.back()->m_array) return 0;
	if (m_stk.back()->m_struct.empty()) return 0;
	const types::Variant& idxval = m_stk.back()->m_struct.back().first;
	if (idxval.type() == types::Variant::UInt || idxval.type() == types::Variant::Int)
	{
		return idxval.touint();
	}
	throw std::runtime_error( "mixing array with structure content");
}

StructureR StructureBuilder::get() const
{
	if (m_stk.size() != 1) throw std::runtime_error("element tags not balanced (final structure)");
	return m_stk.back();
}

int StructureBuilder::taglevel() const
{
	return (int)m_stk.size()-1;
}

bool StructureBuilder::defined() const
{
	return m_stk.back().get();
}

bool StructureBuilder::atomic() const
{
	return m_stk.back().get() && m_stk.back()->m_struct.size() == 0;
}

std::string StructureBuilder::currentElementPath() const
{
	std::string rt;
	std::vector<StructureR>::const_iterator ci = m_stk.begin(), ce = m_stk.end();
	for (; ci != ce; ++ci)
	{
		if (rt.size()) rt.append( "/");
		if ((*ci)->m_struct.size()) rt.append( (*ci)->m_struct.back().first.tostring());
	}
	return rt;
}

void StructureBuilder::clear()
{
	m_stk.clear();
	m_stk.push_back( StructureR());
}



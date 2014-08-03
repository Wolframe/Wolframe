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
///\file cppFormFunction.cpp
///\brief Implementation of form function calls written in C++
#include "serialize/cppFormFunction.hpp"
#include <iostream>
#include <sstream>

using namespace _Wolframe;
using namespace _Wolframe::serialize;

ApiFormData::ApiFormData( const serialize::StructDescriptionBase* descr_)
	:m_descr(descr_)
	,m_data(std::calloc( descr_->size(), 1), std::free)
{
	void* ptr = m_data.get();
	if (!ptr) throw std::bad_alloc();
	if (!m_descr->init( ptr))
	{
		m_descr->done( ptr);
		m_data.reset();
		throw std::bad_alloc();
	}
}

ApiFormData::~ApiFormData()
{
	void* ptr = m_data.get();
	if (ptr) m_descr->done( ptr);
}


CppFormFunctionClosure::CppFormFunctionClosure( const CppFormFunction& f)
	:m_func(&f)
	,m_state(0)
	,m_param_data(f.api_param())
	,m_result_data(f.api_result())
	,m_parser(m_param_data.data(),m_param_data.descr())
	,m_context(0)
{
	m_result.reset( new StructSerializer( m_result_data.data(), m_result_data.descr()));
}

CppFormFunctionClosure::CppFormFunctionClosure( const CppFormFunctionClosure& o)
	:m_func(o.m_func)
	,m_state(0)
	,m_param_data(o.m_param_data)
	,m_result_data(o.m_result_data)
	,m_result(o.m_result)
	,m_parser(o.m_parser)
	,m_context(o.m_context)
	{}

void CppFormFunctionClosure::init( proc::ExecContext* c, const langbind::TypedInputFilterR& i, serialize::Flags::Enum flags)
{
	m_context = c;
	m_parser.init(i,flags);
}

bool CppFormFunctionClosure::call()
{
	void* param_struct = m_param_data.get();
	void* result_struct = m_result_data.get();
	switch (m_state)
	{
		case 0:
			if (!m_parser.call()) return false;
			m_state = 1;
		case 1:
			int rt = m_func->call( m_context, result_struct, param_struct);
			if (rt != 0)
			{
				std::ostringstream msg;
				msg << "error in call of form function (return code " << rt << ")";
				throw std::runtime_error( msg.str());
			}
			m_state = 2;
	}
	return true;
}

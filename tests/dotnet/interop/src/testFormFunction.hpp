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
///\brief Form function replacement for tests
///\file testFormFunction.hpp

#ifndef _LANGBIND_TEST_FORM_FUNCTION_HPP_INCLUDED
#define _LANGBIND_TEST_FORM_FUNCTION_HPP_INCLUDED
#include "ddl_form.hpp"
#include "langbind/formFunction.hpp"
#include "processor/execContext.hpp"
#include <iostream>

namespace _Wolframe {
namespace test {

class FormFunctionClosure
	:public langbind::FormFunctionClosure
{
public:
	FormFunctionClosure( const std::string& name_, const test::Form* result_)
		:m_name(name_),m_result(result_){}

	virtual ~FormFunctionClosure(){}

	virtual bool call()
	{
		std::cout << "CALL FUNCTION '" << m_name << std::endl;
		return true;
	}

	virtual void init( const proc::ProcessorProviderInterface*, const langbind::TypedInputFilterR&, serialize::Context::Flags)
	{
	}

	///\brief Get the iterator for the function result
	virtual langbind::TypedInputFilterR result() const
	{
		return m_result->get();
	}

private:
	std::string m_name;
	const test::Form* m_result;
};

class FormFunction
	:public langbind::FormFunction
{
public:
	FormFunction( const std::string& name_, test::Form& result_)
		:m_name(name_),m_result(result_){}

	virtual ~FormFunction(){}
	virtual langbind::FormFunctionClosure* createClosure() const
	{
		return new test::FormFunctionClosure( m_name, &m_result);
	}

private:
	std::string m_name;
	test::Form m_result;
};

}}//namespace
#endif


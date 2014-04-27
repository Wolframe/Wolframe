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
///\brief Interface for executing functions with structure input/output in the language bindings
///\file langbind/formFunction.hpp

#ifndef _LANGBIND_FORM_FUNCTION_HPP_INCLUDED
#define _LANGBIND_FORM_FUNCTION_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "serialize/mapContext.hpp"
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace proc {
/// \brief Forward declaration
class ExecContext;
}
namespace langbind {

/// \class FormFunctionClosure
/// \brief Form function execution context
class FormFunctionClosure
{
public:
	virtual ~FormFunctionClosure(){}

	///\brief Calls the function with the input from the input filter specified
	///\return true when completed
	virtual bool call()=0;

	///\brief Initialization of call context for a new call
	///\param[in] c execution context reference
	///\param[in] i call input
	///\param[in] f serialization flags for validating form functions depending on caller context (directmap "strict",lua relaxed)
	virtual void init( proc::ExecContext* c, const TypedInputFilterR& i, serialize::Context::Flags f=serialize::Context::None)=0;

	///\brief Get the iterator for the function result
	///\remark MUST be standalone (alive after destruction of this 'FormFunctionClosure'!)
	virtual TypedInputFilterR result() const=0;
};

typedef boost::shared_ptr<FormFunctionClosure> FormFunctionClosureR;

struct FormFunction
{
	virtual ~FormFunction(){}
	virtual FormFunctionClosure* createClosure() const=0;
};

typedef boost::shared_ptr<FormFunction> FormFunctionR;

}}//namespace
#endif


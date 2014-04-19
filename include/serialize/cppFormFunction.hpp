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
///\file serialize/cppFormFunction.hpp
///\brief Interface to form functions written in C++
#ifndef _Wolframe_serialize_CPP_FORM_FUNCTION_HPP_INCLUDED
#define _Wolframe_serialize_CPP_FORM_FUNCTION_HPP_INCLUDED
#include "serialize/struct/structDescriptionBase.hpp"
#include "serialize/struct/structSerializer.hpp"
#include "serialize/struct/structParser.hpp"
#include "processor/procProviderInterface.hpp"
#include "processor/execContext.hpp"
#include "filter/typedfilter.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace serialize {

///\class CppFormFunction
///\brief Function of language bindings implemented in C++ with a form as argument and as result. The forms are defined by a serialization description.
class CppFormFunction
{
public:
	typedef int (*Function)( const proc::ProcessorProviderInterface* provider, void* res, const void* param);

	///\brief Default constructor
	CppFormFunction()
		:m_function(0)
		,m_api_param(0)
		,m_api_result(0){}

	///\brief Copy constructor
	///\param[in] o copied item
	CppFormFunction( const CppFormFunction& o)
		:m_function(o.m_function)
		,m_api_param(o.m_api_param)
		,m_api_result(o.m_api_result){}

	///\brief Constructor
	///\param[in] f function to call
	///\param[in] p part of the api describing the input
	///\param[in] r part of the api describing the function result
	CppFormFunction( Function f, const serialize::StructDescriptionBase* p, const serialize::StructDescriptionBase* r)
		:m_function(f)
		,m_api_param(p)
		,m_api_result(r){}

	///\brief Get the form function parameter description
	///\return The description
	const serialize::StructDescriptionBase* api_param() const
		{return m_api_param;}

	///\brief Get the form function result description
	///\return The description
	const serialize::StructDescriptionBase* api_result() const
		{return m_api_result;}

	///\brief Call the form function
	///\param[in] res pointer to structure as defined with 'api_result()' to hold the form function result
	///\param[in] param pointer to structure as defined with 'api_param()' to hold the form function parameter
	///\return 0 on success, error code else
	int call( const proc::ProcessorProviderInterface* provider, void* res, const void* param) const
		{return (*m_function)( provider, res, param);}

private:
	Function m_function;						//< form function implementation
	const serialize::StructDescriptionBase* m_api_param;		//< api parameter description
	const serialize::StructDescriptionBase* m_api_result;		//< api result description
};

typedef boost::shared_ptr<CppFormFunction> CppFormFunctionR;

///\class ApiFormData
class ApiFormData
	:public virtual utils::TypeSignature
{
public:
	ApiFormData( const serialize::StructDescriptionBase* descr);
	~ApiFormData();

	void* get() const					{return m_data.get();}
	const serialize::StructDescriptionBase* descr() const	{return m_descr;}
	const boost::shared_ptr<void>& data() const		{return m_data;}
private:
	const serialize::StructDescriptionBase* m_descr;
	boost::shared_ptr<void> m_data;
};

///\class CppFormFunctionClosure
///\brief Closure with calling state of called CppFormFunction
class CppFormFunctionClosure
	:public virtual utils::TypeSignature
{
public:
	///\brief Constructor
	///\param[in] f function called
	CppFormFunctionClosure( const CppFormFunction& f);

	///\brief Copy constructor
	///\param[in] o copied item
	CppFormFunctionClosure( const CppFormFunctionClosure& o);

	///\brief Calls the form function with the input from the input filter specified
	///\return true when completed
	bool call();

	///\brief Initialization of call context for a new call
	///\param[in] i call input
	///\param[in] flags serialization flags depending on context (directmap "strict",lua relaxed)
	void init( const proc::ProcessorProviderInterface* provider, const langbind::TypedInputFilterR& i, serialize::Context::Flags flags);

	const serialize::StructSerializer& result() const	{return m_result;}

private:
	CppFormFunction m_func;
	int m_state;
	ApiFormData m_param_data;
	ApiFormData m_result_data;
	serialize::StructSerializer m_result;
	serialize::StructParser m_parser;
	const proc::ProcessorProviderInterface* m_provider;
};

}}//namespace
#endif


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
///\file langbind/appObjects.hpp
///\brief Interface to system objects for processor language bindings
#ifndef _Wolframe_langbind_APP_OBJECTS_HPP_INCLUDED
#define _Wolframe_langbind_APP_OBJECTS_HPP_INCLUDED
#include "filter/filter.hpp"
#include "langbind/builtInFunction.hpp"
#include "database/transactionFunction.hpp"
#include "prnt/printFunction.hpp"
#include "processor/procProvider.hpp"
#include "types/typeSignature.hpp"
#include "ddl/structType.hpp"
#include "ddl/compilerInterface.hpp"
#include "serialize/struct/filtermapBase.hpp"
#include "serialize/ddl/filtermapDDLSerialize.hpp"
#include "serialize/ddl/filtermapDDLParse.hpp"
#include <stack>
#include <string>
#include <algorithm>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

///\class Logger
///\brief Logger as seen from scripting language binding
///\remark Empty object because it is represented as singleton in the system
struct Logger
{
	int _;			//< dummy element because some bindings (Lua) do not like empty structures (objects of size 1)
	Logger() :_(0){}
};

///\class Output
///\brief Output as seen from scripting language binding
class Output
	:public virtual types::TypeSignature
{
public:
	///\brief Constructor
	Output()
		:types::TypeSignature("langbind::Output", __LINE__)
		,m_state(0)
		,m_called(false){}
	///\brief Copy constructor
	///\param[in] o copied item
	Output( const Output& o)
		:types::TypeSignature(o)
		,m_outputfilter(o.m_outputfilter)
		,m_state(o.m_state)
		,m_called(o.m_called){}
	///\brief Constructor by output filter
	///\param[in] flt output filter reference
	Output( const OutputFilterR& flt)
		:types::TypeSignature("langbind::Output", __LINE__)
		,m_outputfilter(flt)
		,m_state(0)
		,m_called(false){}
	///\brief Destructor
	~Output(){}

	///\brief Print the next element
	///\param[in] e1 first element
	///\param[in] e1size first element size
	///\param[in] e2 second element
	///\param[in] e2size second element size
	///\return true, on success, false if we have to yield processing
	bool print( const char* e1, unsigned int e1size, const char* e2, unsigned int e2size);

	const OutputFilterR& outputfilter() const		{return m_outputfilter;}
	OutputFilterR& outputfilter()				{return m_outputfilter;}

	bool called() const					{return m_called;}
	void called( bool yes)					{m_called=yes;}

private:
	OutputFilterR m_outputfilter;				//< output filter reference
	unsigned int m_state;					//< current state for outputs with more than one elements
	bool m_called;						//< has already been called
};

///\class Input
///\brief input as seen from the application processor program
class Input
	:public virtual types::TypeSignature
{
public:
	///\brief Constructor
	Input()
		:types::TypeSignature("langbind::Input", __LINE__)
		,m_used(false){}

	///\brief Copy constructor
	///\param[in] o copied item
	Input( const Input& o)
		:types::TypeSignature(o)
		,m_used(o.m_used)
		,m_inputfilter(o.m_inputfilter){}

	///\brief Constructor by input filter
	///\param[in] flt input filter reference
	explicit Input( const InputFilterR& flt)
		:types::TypeSignature("langbind::Input", __LINE__)
		,m_used(false)
		,m_inputfilter(flt){}

	///\brief Destructor
	~Input(){}

	const InputFilterR& inputfilter() const		{return m_inputfilter;}
	InputFilterR& inputfilter()			{return m_inputfilter;}

	InputFilterR& getIterator();

private:
	bool m_used;					//< only one iterator can be created from input. This is the guard for checking this.
	InputFilterR m_inputfilter;			//< input is defined by the associated input filter
};

class DDLFormParser
	:public virtual types::TypeSignature
	,public serialize::DDLStructParser
{
public:
	explicit DDLFormParser( const ddl::FormR& form_)
		:types::TypeSignature("langbind::DDLFormParser", __LINE__)
		,DDLStructParser(form_.get())
		,m_form(form_){}

	DDLFormParser( const DDLFormParser& o)
		:types::TypeSignature(o)
		,DDLStructParser(o)
		,m_form(o.m_form){}

	virtual ~DDLFormParser(){}

	DDLFormParser& operator=( const DDLFormParser& o)
	{
		types::TypeSignature::operator=( o);
		DDLStructParser::operator=( o);
		m_form = o.m_form;
		return *this;
	}

	const ddl::FormR& form() const	{return m_form;}

private:
	ddl::FormR m_form;
};

class DDLFormSerializer
	:public virtual types::TypeSignature
	,public serialize::DDLStructSerializer
{
public:
	DDLFormSerializer()
		:types::TypeSignature("langbind::DDLFormSerializer", __LINE__){}

	explicit DDLFormSerializer( const ddl::FormR& form_)
		:types::TypeSignature("langbind::DDLFormSerializer", __LINE__)
		,DDLStructSerializer(form_.get())
		,m_form(form_){}

	DDLFormSerializer( const DDLFormSerializer& o)
		:types::TypeSignature(o)
		,DDLStructSerializer(o)
		,m_form(o.m_form){}
	virtual ~DDLFormSerializer(){}

	DDLFormSerializer& operator =( const DDLFormSerializer& o)
	{
		types::TypeSignature::operator=( o);
		DDLStructSerializer::operator=( o);
		m_form = o.m_form;
		return *this;
	}

	const ddl::FormR& form() const	{return m_form;}

private:
	ddl::FormR m_form;
};

///\class RedirectFilterClosure
class RedirectFilterClosure
	:public virtual types::TypeSignature
{
public:
	RedirectFilterClosure();
	RedirectFilterClosure( const TypedInputFilterR& i, const TypedOutputFilterR& o);
	RedirectFilterClosure( const RedirectFilterClosure& o);
	~RedirectFilterClosure(){}

	///\brief Calls the fetching of input and printing it to output until end or interruption
	///\return true when completed
	bool call();

	///\brief Initialization of call context for a new call
	///\param[in] i call input
	///\param[in] o call output
	void init( const TypedInputFilterR& i, const TypedOutputFilterR& o);

	const TypedInputFilterR& inputfilter() const		{return m_inputfilter;}
	const TypedOutputFilterR& outputfilter() const		{return m_outputfilter;}

private:
	int m_state;				//< current state of call
	int m_taglevel;				//< current balance of open/close tags
	TypedInputFilterR m_inputfilter;	//< input filter
	TypedOutputFilterR m_outputfilter;	//< output filter
	InputFilter::ElementType m_elemtype;	//< type of last element read from command result
	TypedInputFilter::Element m_elem;	//< last element read from command result
};

class ApiFormData
	:public virtual types::TypeSignature
{
public:
	ApiFormData( const serialize::StructDescriptionBase* descr);
	~ApiFormData();

	void* get() const						{return m_data.get();}
	const serialize::StructDescriptionBase* descr() const	{return m_descr;}
	const boost::shared_ptr<void>& data() const			{return m_data;}
private:
	const serialize::StructDescriptionBase* m_descr;
	boost::shared_ptr<void> m_data;
};

///\class BuiltInFunctionClosure
///\brief Closure with calling state of called BuiltInFunction
class BuiltInFunctionClosure
	:public virtual types::TypeSignature
{
public:
	///\brief Constructor
	///\param[in] f function called
	BuiltInFunctionClosure( const BuiltInFunction& f);

	///\brief Copy constructor
	///\param[in] o copied item
	BuiltInFunctionClosure( const BuiltInFunctionClosure& o);

	///\brief Calls the form function with the input from the input filter specified
	///\return true when completed
	bool call();

	///\brief Initialization of call context for a new call
	///\param[in] i call input
	///\param[in] flags serialization flags depending on context (directmap "strict",lua relaxed)
	void init( const TypedInputFilterR& i, serialize::Context::Flags flags);

	const serialize::StructSerializer& result() const		{return m_result;}

private:
	BuiltInFunction m_func;
	int m_state;
	ApiFormData m_param_data;
	ApiFormData m_result_data;
	serialize::StructSerializer m_result;
	serialize::StructParser m_parser;
};

///\class TransactionFunctionClosure
///\brief Closure with calling state of called TransactionFunction
class TransactionFunctionClosure
	:public virtual types::TypeSignature
{
public:
	///\brief Constructor
	///\param[in] provider_ processor provider to allocate transaction object
	///\param[in] f function called
	///\param[in] t transaction context, if provided by caller
	TransactionFunctionClosure( const db::TransactionFunction* f, const db::TransactionR& t=db::TransactionR());

	///\brief Copy constructor
	///\param[in] o copied item
	TransactionFunctionClosure( const TransactionFunctionClosure& o);

	///\brief Calls the transaction function with the input from the input filter specified
	///\return true when completed
	bool call();

	///\brief Initialization of call context for a new call
	///\param[in] i call input
	void init( const proc::ProcessorProvider* provider_, const TypedInputFilterR& i);

	const TypedInputFilterR& result() const
	{
		return m_result;
	}

private:
	const proc::ProcessorProvider* m_provider;	//< processor provider to get transaction object
	const db::TransactionFunction* m_func;		//< function to execute
	int m_state;					//< current state of call
	RedirectFilterClosure m_input;			//< builder of structure from input
	db::TransactionFunctionInput* m_inputstructptr;	//< input structure implementation interface
	TypedOutputFilterR m_inputstruct;		//< input structure
	TypedInputFilterR m_result;			//< function call result
	db::TransactionR m_transaction;			//< transaction context, created in the transaction (autocommit) if not defined
};

///\class PrintFunctionClosure
///\brief Closure with calling state of called PrintFunction
class PrintFunctionClosure
	:public virtual types::TypeSignature
{
public:
	///\brief Constructor
	///\param[in] f function called
	PrintFunctionClosure( const prnt::PrintFunction* f);

	///\brief Copy constructor
	///\param[in] o copied item
	PrintFunctionClosure( const PrintFunctionClosure& o);

	///\brief Calls the transaction function with the input from the input filter specified
	///\return true when completed
	bool call();

	///\brief Initialization of call context for a new call
	///\param[in] i call input
	void init( const TypedInputFilterR& i);

	const std::string& result() const		{return m_result;}

private:
	const prnt::PrintFunction* m_func;		//< function to execute
	int m_state;					//< current state of call
	RedirectFilterClosure m_input;			//< builder of structure from input
	prnt::PrintFunction::InputR m_inputstruct;	//< input structure
	std::string m_result;				//< function call result
};

}} //namespace
#endif


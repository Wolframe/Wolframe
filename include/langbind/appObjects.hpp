/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
///\brief interface to system objects for processor language bindings
#ifndef _Wolframe_langbind_APP_OBJECTS_HPP_INCLUDED
#define _Wolframe_langbind_APP_OBJECTS_HPP_INCLUDED
#include "filter/filter.hpp"
#include "ddl/structType.hpp"
#include "ddl/compilerInterface.hpp"
#include "serialize/struct/filtermapBase.hpp"
#include "serialize/ddl/filtermapDDLSerialize.hpp"
#include "serialize/ddl/filtermapDDLParse.hpp"
#include "cmdbind/commandHandler.hpp"
#include <stack>
#include <string>
#include <algorithm>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

///\class Logger
///\brief Logger as seen from scripting language binding
///\remark Empty object because it is represented as singleton in the system
class Logger
{
	int _;			//< dummy element because some bindings (Lua) do not like empty structures (objects of size 1)
};

///\class Output
///\brief Output as seen from scripting language binding
class Output
{
public:
	///\brief Constructor
	Output() :m_state(0){}
	///\brief Copy constructor
	///\param[in] o copied item
	Output( const Output& o) :m_outputfilter(o.m_outputfilter),m_state(o.m_state){}
	///\brief Constructor by output filter
	///\param[in] flt output filter reference
	Output( const OutputFilterR& flt) :m_outputfilter(flt),m_state(0){}
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

private:
	OutputFilterR m_outputfilter;				//< output filter reference
	unsigned int m_state;					//< current state for outputs with more than one elements
};

///\class Input
///\brief input as seen from the application processor program
class Input
{
public:
	///\brief Constructor
	Input(){}
	///\brief Copy constructor
	///\param[in] o copied item
	Input( const Input& o) :m_inputfilter(o.m_inputfilter){}
	///\brief Constructor by input filter
	///\param[in] flt input filter reference
	Input( const InputFilterR& flt) :m_inputfilter(flt){}
	///\brief Destructor
	~Input(){}

	const InputFilterR& inputfilter() const		{return m_inputfilter;}
	InputFilterR& inputfilter()			{return m_inputfilter;}

private:
	InputFilterR m_inputfilter;			//< input is defined by the associated input filter
};



///\class FilterMap
///\brief Map of available filters seen from scripting language binding
class FilterMap
{
public:
	FilterMap();
	~FilterMap(){}
	FilterMap( const FilterMap& o)
		:m_map(o.m_map){}

	void defineFilter( const std::string& name, const FilterFactoryR& f);
	bool getFilter( const std::string& arg, Filter& rt);
private:
	std::map<std::string,FilterFactoryR> m_map;
};


///\class RedirectFilterClosure
class RedirectFilterClosure
{
public:
	RedirectFilterClosure();
	RedirectFilterClosure( const TypedInputFilterR& i, const TypedOutputFilterR& o);
	RedirectFilterClosure( const RedirectFilterClosure& o);

	///\brief Calls the fetching of input and printing it to output until end or interruption
	///\return true when completed
	bool call();

	///\brief Initialization of call context for a new call
	///\param[in] i call input
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


///\class DDLForm
class DDLForm
{
public:
	///\brief Default constructor
	DDLForm(){}

	///\brief Copy constructor
	///\param[in] o copied item
	DDLForm( const DDLForm& o)
		:m_structure(o.m_structure){}

	///\brief Constructor
	///\param[in] st form data
	DDLForm( const ddl::StructTypeR& st)
		:m_structure(st){}

	///\brief Destructor
	~DDLForm(){}

	const ddl::StructTypeR& structure() const	{return m_structure;}

	std::string tostring() const;
	void clone();
private:
	friend class DDLFormFill;
	ddl::StructTypeR m_structure;
};

///\class DDLFormMap
///\brief Map of available forms seen from scripting language binding
class DDLFormMap
{
public:
	DDLFormMap(){}
	~DDLFormMap(){}

	void defineForm( const std::string& name, const DDLForm& f);
	bool getForm( const std::string& name, DDLForm& rt) const;
private:
	std::map<std::string,DDLForm> m_map;
};


class ApiFormData
{
public:
	ApiFormData( const serialize::FiltermapDescriptionBase* descr);
	~ApiFormData();

	void* get() const						{return m_data.get();}
	const serialize::FiltermapDescriptionBase* descr() const	{return m_descr;}
	const boost::shared_ptr<void>& data() const			{return m_data;}
private:
	const serialize::FiltermapDescriptionBase* m_descr;
	boost::shared_ptr<void> m_data;
};


///\class FormFunction
class FormFunction
{
public:
	typedef int (Function)( void* res, const void* param);

	///\brief Default constructor
	FormFunction();

	///\brief Copy constructor
	///\param[in] o copied item
	FormFunction( const FormFunction& o);

	///\brief Constructor
	///\param[in] f function to call
	///\param[in] p part of the api describing the input
	///\param[in] r part of the api describing the function result
	FormFunction( Function f, const serialize::FiltermapDescriptionBase* p, const serialize::FiltermapDescriptionBase* r);

	const serialize::FiltermapDescriptionBase* api_param() const	{return m_api_param;}
	const serialize::FiltermapDescriptionBase* api_result() const	{return m_api_result;}
	int call( void* res, const void* param) const			{return (*m_function)( res, param);}

private:
	Function* m_function;
	const serialize::FiltermapDescriptionBase* m_api_param;
	const serialize::FiltermapDescriptionBase* m_api_result;
};

///\class FormFunctionClosure
///\brief Closure with calling state of called FormFunction
class FormFunctionClosure
{
public:
	///\brief Constructor
	///\param[in] f function called
	FormFunctionClosure( const FormFunction& f);

	///\brief Copy constructor
	///\param[in] o copied item
	FormFunctionClosure( const FormFunctionClosure& o);

	///\brief Calls the form function with the input from the input filter specified
	///\return true when completed
	bool call();

	///\brief Initialization of call context for a new call
	///\param[in] i call input
	void init( const TypedInputFilterR& i, serialize::Context::Flags flags);

	const serialize::StructSerializer& result() const		{return m_result;}

private:
	FormFunction m_func;
	int m_state;
	ApiFormData m_param_data;
	ApiFormData m_result_data;
	serialize::StructSerializer m_result;
	serialize::StructParser m_parser;
};

///\class FormFunctionMap
///\brief Map of available transaction functions seen from scripting language binding
class FormFunctionMap
{
public:
	FormFunctionMap(){}
	~FormFunctionMap(){}

	void defineFormFunction( const std::string& name, const FormFunction& f);
	bool getFormFunction( const std::string& name, FormFunction& rt) const;
private:
	std::map<std::string,FormFunction> m_map;
};



typedef cmdbind::CommandHandlerR (*CreateCommandHandler)( const std::string& name);
class TransactionFunction
{
public:
	TransactionFunction(){}
	TransactionFunction( const TransactionFunction& o)
		:m_cmdwriter(o.m_cmdwriter)
		,m_resultreader(o.m_resultreader)
		,m_cmdconstructor(o.m_cmdconstructor){}

	TransactionFunction( const OutputFilterR& w, const InputFilterR& r, CreateCommandHandler c)
		:m_cmdwriter(w)
		,m_resultreader(r)
		,m_cmdconstructor(c){}

	const OutputFilterR& cmdwriter() const		{return m_cmdwriter;}
	const InputFilterR& resultreader() const	{return m_resultreader;}
	CreateCommandHandler cmdconstructor() const	{return m_cmdconstructor;}
private:
	OutputFilterR m_cmdwriter;			//< command input writer
	InputFilterR m_resultreader;			//< command result reader
	CreateCommandHandler m_cmdconstructor;
};


///\class TransactionFunctionResult
///\brief Result of a TransactionFunction call
class TransactionFunctionResult
{
public:
	///\brief Constructor
	///\param[in] f function called
	explicit TransactionFunctionResult( const TransactionFunction& f);

	///\brief Copy constructor
	///\param[in] o copied item
	TransactionFunctionResult( const TransactionFunctionResult& o);

	///\brief Destructor
	~TransactionFunctionResult(){}

	///\brief fetches results and writes them to the output filter specified
	///\return true when completed
	bool call();

	///\brief Initialization of call context for a new fetch result
	///\param[in] o fetch output
	void init( const TypedOutputFilterR& o);

	///\brief Append a chunk to the result buffer
	///\param[in] buf pointer to chunk
	///\param[in] size size of chunk in bytes
	void appendCmdOutput( const void* buf, std::size_t size);

	///\brief Clear the result buffer and error message buffer
	void reset();

private:
	TransactionFunction m_func;				//< transaction function executed
	int m_state;						//< execution state
	InputFilter::ElementType m_elemtype;			//< type of last element read from command result
	TypedInputFilter::Element m_elem;			//< last element read from command result
	boost::shared_ptr<std::string> m_resultbuf;		//< buffer for result
	InputFilterR m_resultreader;				//< result reader instance
	TypedOutputFilterR m_outputfilter;			//< output of the transaction function
};

///\class TransactionFunctionClosure
///\brief Closure with calling state of called TransactionFunction
class TransactionFunctionClosure
{
public:
	enum {
		InputBufSize=(1<<12),
		OutputBufSize=(1<<12)
	};

	///\brief Constructor
	///\param[in] nam name of the function called
	///\param[in] f function called
	TransactionFunctionClosure( const std::string& nam, const TransactionFunction& f);

	///\brief Copy constructor
	///\param[in] o copied item
	TransactionFunctionClosure( const TransactionFunctionClosure& o);

	///\brief Destructor
	~TransactionFunctionClosure(){}

	///\brief Executes the transaction function with the input from the input filter specified as far as possible
	///\return true when completed
	bool call();

	///\brief Initialization of call context for a new call
	///\param[in] i call input
	void init( const TypedInputFilterR& i);

	///\brief Get the result of the command
	const TransactionFunctionResult& result() const		{return m_result;}

private:
	TransactionFunction m_func;				//< transaction function executed
	std::string m_name;					//< name of the transaction function executed
	cmdbind::CommandHandlerR m_cmd;				//< command execute handler
	cmdbind::CommandHandler::Operation m_cmdop;		//< last operation fetched from command handler
	int m_state;						//< execution state
	InputFilter::ElementType m_elemtype;			//< type of last element read from command result
	TypedInputFilter::Element m_elem;			//< last element read from command result
	boost::shared_ptr<void> m_cmdinputbuf;			//< buffer for the command input
	boost::shared_ptr<void> m_cmdoutputbuf;			//< buffer for the command output
	OutputFilterR m_cmdwriter;				//< writer of the command
	TransactionFunctionResult m_result;			//< function result
	TypedInputFilterR m_inputfilter;			//< transaction command input
};

///\class TransactionFunctionMap
///\brief Map of available transaction functions seen from scripting language binding
class TransactionFunctionMap
{
public:
	TransactionFunctionMap(){}
	~TransactionFunctionMap(){}

	void defineTransactionFunction( const std::string& name, const TransactionFunction& f);
	bool getTransactionFunction( const std::string& name, TransactionFunction& rt) const;
private:
	std::map<std::string,TransactionFunction> m_map;
};


///\class DDLCompilerMap
///\brief Map of available DDL compilers seen from scripting language binding
class DDLCompilerMap
{
public:
	DDLCompilerMap();
	~DDLCompilerMap(){}

	void defineDDLCompiler( const std::string& name, const ddl::CompilerInterfaceR& ci);
	bool getDDLCompiler( const std::string& name, ddl::CompilerInterfaceR& rt) const;
private:
	std::map<std::string,ddl::CompilerInterfaceR> m_map;
};

}} //namespace
#endif


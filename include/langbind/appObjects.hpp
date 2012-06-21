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
#ifndef _Wolframe_langbind_APPOBJECTS_HPP_INCLUDED
#define _Wolframe_langbind_APPOBJECTS_HPP_INCLUDED
#include "filter/filter.hpp"
#include "ddl/structType.hpp"
#include "ddl/compilerInterface.hpp"
#include "serialize/struct/filtermapBase.hpp"
#include "serialize/ddl/filtermapDDLPrint.hpp"
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
	///\enum ItemType
	///\brief Output state
	enum ItemType
	{
		Data,		//< normal processing
		DoYield,	//< yield because rest of buffer not sufficient to complete operation
		Error		//< logic error in output. Operation is not possible
	};
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
	///\return state returned
	ItemType print( const char* e1, unsigned int e1size, const char* e2, unsigned int e2size);

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

///\class InputFilterClosure
///\brief Closure for the input iterator (in Lua returned by 'input.get()')
class InputFilterClosure
{
public:
	///\enum ItemType
	///\brief Input loop state
	enum ItemType
	{
		EndOfData,	//< End of processed content reached
		Data,		//< normal processing, loop can continue
		DoYield,	//< have to yield and request more network input
		Error		//< have to stop processing because of an error
	};

	///\brief Constructor
	///\param[in] ig input filter reference from input
	InputFilterClosure( const InputFilterR& ig)
		:m_inputfilter(ig)
		,m_type(InputFilter::Value)
		,m_gotattr(false)
		,m_taglevel(0){}

	InputFilterClosure( const InputFilterClosure& o)
		:m_inputfilter(o.m_inputfilter)
		,m_type(o.m_type)
		,m_attrbuf(o.m_attrbuf)
		,m_gotattr(o.m_gotattr)
		,m_taglevel(o.m_taglevel){}

	///\brief Get the next pair of elements
	///\param[out] e1 first element
	///\param[out] e1size first element size
	///\param[out] e2 second element
	///\param[out] e2size second element size
	///\return state returned
	ItemType fetch( const char*& e1, unsigned int& e1size, const char*& e2, unsigned int& e2size);

	const InputFilterR& inputfilter() const	{return m_inputfilter;}
private:
	InputFilterR m_inputfilter;			//< rerefence to input with filter
	InputFilter::ElementType m_type;		//< current state (last value type parsed)
	std::string m_attrbuf;				//< buffer for attribute name
	bool m_gotattr;					//< true, if the following value belongs to an attribute
	std::size_t m_taglevel;				//< current level in tag hierarchy
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

	///\enum CallResult
	///\brief Enumeration of call states of the processing
	enum CallResult
	{
		Ok,		//< successful termination of call
		Error,		//< termination of call with error (not completed)
		Yield		//< call interrupted with request for a network operation
	};
	///\brief Calls the fetching of input and printing it to output until end or interruption
	///\return Call state
	CallResult call();

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
	DDLForm( const ddl::StructType& st)
		:m_structure(st){}

	///\brief Destructor
	~DDLForm(){}

	const ddl::StructType& structure() const	{return m_structure;}

	std::string tostring() const;
private:
	friend class DDLFormFill;
	ddl::StructType m_structure;
};

typedef CountedReference<DDLForm> DDLFormR;


///\class DDLFormFill
///\brief State of filling a form
class DDLFormFill
{
public:
	///\brief Constructor
	DDLFormFill( const DDLFormR& f, serialize::Context::Flags flags);

	///\brief Constructor
	DDLFormFill( const DDLFormR& f, const TypedInputFilterR& inp, serialize::Context::Flags flags);

	///\brief Copy constructor
	DDLFormFill( const DDLFormFill& o);

	///\brief Destructor
	~DDLFormFill(){}

	///\brief Get the last error of 'call' as string
	const char* getLastError() const			{return m_ctx.getLastError();}

	///\brief Get the last error position of 'call' as string
	const char* getLastErrorPos() const			{return m_ctx.getLastErrorPos();}

	///\brief Initialization before call
	///\param[in] i input filter
	void init( const TypedInputFilterR& i);

	///\enum CallResult
	///\brief Enumeration of call states
	enum CallResult
	{
		Ok,		//< successful termination
		Error,		//< termination with error (not completed)
		Yield		//< call interrupted with request for a system operation
	};

	///\brief Call of the form fill
	///\remark finished with 'Error' or 'Ok'
	CallResult call();

private:
	DDLFormR m_form;
	int m_state;
	TypedInputFilterR m_inputfilter;
	serialize::Context m_ctx;
	serialize::FiltermapDDLParseStateStack m_parsestk;
};


///\class DDLFormPrint
///\brief State of serialization of a DDLForm
class DDLFormPrint
{
public:
	///\brief Constructor
	DDLFormPrint( const DDLFormR& f, serialize::Context::Flags flags);

	///\brief Constructor
	DDLFormPrint( const DDLFormR& f, const TypedOutputFilterR& outp, serialize::Context::Flags flags);

	///\brief Copy constructor
	DDLFormPrint( const DDLFormPrint& o);

	///\brief Destructor
	~DDLFormPrint(){}

	///\brief Get the last error as string
	const char* getLastError() const			{return m_ser.getLastError();}

	///\brief Get the last error position as string
	const char* getLastErrorPos() const			{return m_ser.getLastErrorPos();}

	///\enum CallResult
	///\brief Enumeration of call states of the fetch processing
	enum CallResult
	{
		Ok,		//< successful termination of call
		Error,		//< termination of call with error (not completed)
		Yield		//< call interrupted with request for a network operation
	};
	///\brief fetches results and writes them to the output filter specified
	///\return Call state
	CallResult fetch();

	///\brief Initialization of call context for a new fetch result
	///\param[in] o fetch output
	void init( const TypedOutputFilterR& o);

private:
	DDLFormR m_form;
	int m_state;
	TypedOutputFilterR m_outputfilter;
	serialize::DDLStructSerializer m_ser;
};

///\class DDLFormMap
///\brief Map of available forms seen from scripting language binding
class DDLFormMap
{
public:
	DDLFormMap(){}
	~DDLFormMap(){}

	void defineForm( const std::string& name, const DDLForm& f);
	bool getForm( const std::string& name, DDLFormR& rt) const;
private:
	std::map<std::string,DDLFormR> m_map;
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
	Function* function() const					{return m_function;}
private:
	Function* m_function;
	const serialize::FiltermapDescriptionBase* m_api_param;
	const serialize::FiltermapDescriptionBase* m_api_result;
};

///\class FormFunctionResult
///\brief Result of a FormFunction call
class FormFunctionResult
{
public:
	///\brief Constructor
	///\param[in] f function called
	FormFunctionResult( const FormFunction& f);

	///\brief Copy constructor
	///\param[in] o copied item
	FormFunctionResult( const FormFunctionResult& o);

	///\brief Destructor
	~FormFunctionResult();

	///\brief Get the last error as string
	const char* getLastError() const			{return m_ser.getLastError();}

	///\brief Get the last error position as string
	const char* getLastErrorPos() const			{return m_ser.getLastErrorPos();}

	///\enum CallResult
	///\brief Enumeration of call states of the fetch processing
	enum CallResult
	{
		Ok,		//< successful termination of call
		Error,		//< termination of call with error (not completed)
		Yield		//< call interrupted with request for a network operation
	};
	///\brief fetches results and writes them to the output filter specified
	///\return Call state
	CallResult fetch();

	///\brief Initialization of call context for a new fetch result
	///\param[in] o fetch output
	void init( const TypedOutputFilterR& o);

	void* data() const
	{
		return m_data.get();
	}
private:
	const serialize::FiltermapDescriptionBase* m_description;
	int m_state;
	boost::shared_ptr<void> m_data;
	TypedOutputFilterR m_outputfilter;
	serialize::StructSerializer m_ser;
};

///\class FormFunctionClosure
///\brief Closure with calling state of called FormFunction
class FormFunctionClosure :public FormFunction
{
public:
	///\brief Constructor
	///\param[in] f function called
	FormFunctionClosure( const FormFunction& f);

	///\brief Copy constructor
	///\param[in] o copied item
	FormFunctionClosure( const FormFunctionClosure& o);

	///\brief Destructor
	~FormFunctionClosure();

	///\brief Get the last error as string
	const char* getLastError() const			{return m_ctx.getLastError();}

	///\brief Get the last error position as string
	const char* getLastErrorPos() const			{return m_ctx.getLastErrorPos();}

	///\enum CallResult
	///\brief Enumeration of call states of the call processing
	enum CallResult
	{
		Ok,		//< successful termination of call
		Error,		//< termination of call with error (not completed)
		Yield		//< call interrupted with request for a network operation
	};
	///\brief Calls the form function with the input from the input filter specified
	///\return Call state
	CallResult call();

	///\brief Initialization of call context for a new call
	///\param[in] i call input
	void init( const TypedInputFilterR& i);

	const TypedInputFilterR& inputfilter() const	{return m_inputfilter;}
	const FormFunctionResult& result() const	{return m_result;}

private:
	int m_state;
	FormFunctionResult m_result;
	boost::shared_ptr<void> m_data;
	serialize::FiltermapParseStateStack m_parsestk;
	serialize::Context m_ctx;
	TypedInputFilterR m_inputfilter;
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

	///\brief Get the last error as string
	const char* getLastError() const
	{
		return m_lasterror.size()?m_lasterror.c_str():0;
	}

	///\enum CallResult
	///\brief Enumeration of call states of the fetch processing
	enum CallResult
	{
		Ok,		//< successful termination of call
		Error,		//< termination of call with error (not completed)
		Yield		//< call interrupted with request for a network operation
	};

	///\brief fetches results and writes them to the output filter specified
	///\return Call state
	CallResult fetch();

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
	std::string m_lasterror;				//< last error string
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

	///\brief Get the last error as string
	const char* getLastError() const
	{
		return m_lasterror.size()?m_lasterror.c_str():0;
	}

	///\enum CallResult
	///\brief Enumeration of call states of the call processing
	enum CallResult
	{
		Ok,		//< successful termination of call
		Error,		//< termination of call with error (not completed)
		Yield		//< call interrupted with request for a network operation
	};
	///\brief Executes the transaction function with the input from the input filter specified as far as possible
	///\return Call state
	CallResult call();

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
	std::string m_lasterror;				//< last error string
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


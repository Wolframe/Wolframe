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
	~RedirectFilterClosure(){}

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
	DDLForm copy() const;
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


class PeerFunction
{
public:
	typedef cmdbind::CommandHandlerR (*CreateCommandHandler)();

	PeerFunction(){}
	PeerFunction( const PeerFunction& o);
	PeerFunction( const OutputFilterR& w, const InputFilterR& r, CreateCommandHandler c);

	const OutputFilterR& cmdwriter() const		{return m_cmdwriter;}
	const InputFilterR& resultreader() const	{return m_resultreader;}
	CreateCommandHandler cmdconstructor() const	{return m_cmdconstructor;}

private:
	OutputFilterR m_cmdwriter;			//< command input writer
	InputFilterR m_resultreader;			//< command result reader
	CreateCommandHandler m_cmdconstructor;		//< constructor of a command handler instance for that command
};


class PeerFormFunction :public PeerFunction
{
public:
	PeerFormFunction(){}
	PeerFormFunction( const PeerFormFunction& o);
	PeerFormFunction( const PeerFunction& f, const DDLForm& i, const DDLForm& o);

	const DDLForm& inputform() const		{return m_inputform;}
	const DDLForm& outputform() const		{return m_outputform;}

private:
	DDLForm m_inputform;				//< transaction function parameter description
	DDLForm m_outputform;				//< transaction function result description
};


///\class PeerFormFunctionClosure
///\brief Closure with calling state of called PeerFormFunction
class PeerFormFunctionClosure
{
public:
	enum {
		InputBufSize=(1<<12),
		OutputBufSize=(1<<12)
	};

	///\brief Constructor
	///\param[in] nam name of the function called
	///\param[in] f function called
	PeerFormFunctionClosure( const PeerFormFunction& f);

	///\brief Copy constructor
	///\param[in] o copied item
	PeerFormFunctionClosure( const PeerFormFunctionClosure& o);

	///\brief Initialization of call context for a new call
	///\param[in] i call input
	void init( const TypedInputFilterR& i);

	///\brief Destructor
	~PeerFormFunctionClosure(){}

	///\brief Executes the transaction function with the input from the input filter specified as far as possible
	///\return true when completed, false if it needs more input (yield execution)
	bool call();

	///\brief Get the result of the command
	DDLForm result() const;

private:
	PeerFormFunction m_func;				//< transaction function executed
	cmdbind::CommandHandlerR m_cmd;				//< command execute handler
	cmdbind::CommandHandler::Operation m_cmdop;		//< last operation fetched from command handler
	int m_state;						//< execution state
	boost::shared_ptr<void> m_cmdinputbuf;			//< buffer for the command input
	boost::shared_ptr<void> m_cmdoutputbuf;			//< buffer for the command output
	OutputFilterR m_cmdwriter;				//< writer of the command
	InputFilterR m_resultreader;				//< reader (parser) of the result
	serialize::DDLStructParser m_param;			//< function parameter form
	serialize::DDLStructParser m_result;			//< function result form
	serialize::DDLStructSerializer m_cmdserialize;		//< serializer of the command
	TypedInputFilterR m_inputfilter;			//< transaction command input
};


///\class PeerFunctionMap
///\brief Map of available transaction functions seen from scripting language binding
class PeerFunctionMap
{
public:
	PeerFunctionMap(){}
	~PeerFunctionMap(){}

	void definePeerFunction( const std::string& name, const PeerFunction& f);
	bool getPeerFunction( const std::string& name, PeerFunction& rt) const;
private:
	std::map<std::string,PeerFunction> m_map;
};


///\class PeerFormFunctionMap
///\brief Map of available transaction functions seen from scripting language binding
class PeerFormFunctionMap
{
public:
	PeerFormFunctionMap(){}
	~PeerFormFunctionMap(){}

	void definePeerFormFunction( const std::string& name, const PeerFormFunction& f);
	bool getPeerFormFunction( const std::string& name, PeerFormFunction& rt) const;
private:
	std::map<std::string,PeerFormFunction> m_map;
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


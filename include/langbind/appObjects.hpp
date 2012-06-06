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
#include "serialize/ddl/filtermapDDLPrintStack.hpp"
#include "serialize/ddl/filtermapDDLParseStack.hpp"
#include "cmdbind/commandHandler.hpp"
#include <stack>
#include <string>
#include <algorithm>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

///\class Output
///\brief Output as seen from scripting language binding
struct Output
{
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
struct Input
{
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


///\class DDLForm
struct DDLForm
{
	ddl::StructType m_struct;

	///\brief Default constructor
	DDLForm() {}

	///\brief Copy constructor
	///\param[in] o copied item
	DDLForm( const DDLForm& o)
		:m_struct(o.m_struct){}

	///\brief Constructor
	///\param[in] st form data
	DDLForm( const ddl::StructType& st)
		:m_struct(st){}

	///\brief Destructor
	~DDLForm(){}
};

typedef CountedReference<DDLForm> DDLFormR;

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
	std::map<std::string,DDLForm> m_map;
};

///\class PluginFunction
class PluginFunction
{
public:
	typedef int (Call)( void* res, const void* param);

	///\brief Default constructor
	PluginFunction()
		:m_state(-1)
		,m_lastres(Error)
		,m_call(0)
		,m_api_param(0)
		,m_api_result(0){}

	///\brief Copy constructor
	///\param[in] o copied item
	PluginFunction( const PluginFunction& o)
		:m_state(o.m_state)
		,m_lastres(o.m_lastres)
		,m_data(o.m_data)
		,m_call(o.m_call)
		,m_api_param(o.m_api_param)
		,m_api_result(o.m_api_result)
		,m_parsestk(o.m_parsestk)
		,m_printstk(o.m_printstk)
		,m_ctx(o.m_ctx)
	{
		if (m_state > 0) throw std::runtime_error( "illegal copy of plugin function not in initial state");
	}

	///\brief Constructor
	///\param[in] c function to call
	///\param[in] p part of the api describing the input
	///\param[in] r part of the api describing the function result
	PluginFunction( Call c, const serialize::FiltermapDescriptionBase* p, const serialize::FiltermapDescriptionBase* r)
		:m_state(0),m_lastres(Ok),m_data(std::calloc( p->size() + r->size(), 1), std::free),m_call(c),m_api_param(p),m_api_result(r){}

	///\brief Destructor
	~PluginFunction();

	///\enum CallResult
	///\brief Enumeration of call states of a function call processing
	enum CallResult
	{
		Ok,		//< successful termination of call
		Error,		//< termination of call with error (not completed)
		Yield		//< call interrupted with request for a network operation
	};
	CallResult call( InputFilter& ifl, OutputFilter& ofl);

	const char* getLastError() const		{return m_ctx.getLastError();}

private:
	int m_state;
	CallResult m_lastres;
	boost::shared_ptr<void> m_data;
	Call* m_call;
	const serialize::FiltermapDescriptionBase* m_api_param;
	const serialize::FiltermapDescriptionBase* m_api_result;
	serialize::FiltermapParseStateStack m_parsestk;
	serialize::FiltermapPrintStateStack m_printstk;
	serialize::Context m_ctx;
};

///\class PluginFunctionMap
///\brief Map of available transaction functions seen from scripting language binding
class PluginFunctionMap
{
public:
	PluginFunctionMap(){}
	~PluginFunctionMap(){}

	void definePluginFunction( const std::string& name, const PluginFunction& f);
	bool getPluginFunction( const std::string& name, PluginFunction& rt) const;
private:
	std::map<std::string,PluginFunction> m_map;
};


///\class TransactionFunction
class TransactionFunction
{
public:
	///\brief Default constructor
	TransactionFunction() {}

	///\brief Copy constructor
	///\param[in] o copied item
	TransactionFunction( const TransactionFunction& o)
		:m_cmdwriter(o.m_cmdwriter)
		,m_resultreader(o.m_resultreader)
		,m_cmd(o.m_cmd){}

	///\brief Constructor
	///\param[in] w command input writer
	///\param[in] r command output reader
	///\param[in] c command execute handler
	TransactionFunction( const OutputFilterR& w, const InputFilterR& r, const cmdbind::CommandHandlerR& c)
		:m_cmdwriter(w),m_resultreader(r),m_cmd(c){}

	///\brief Destructor
	~TransactionFunction(){}

	///\brief Transaction function call
	///\param[in] param function call arguments
	///\param[in,out] result function call result
	///\return true on success, false else
	bool call( const DDLForm& param, DDLForm& result);

	typedef cmdbind::CommandHandlerR (*CreateCommandHandler)( const std::string& name);
	struct Definition
	{
		Definition(){}
		Definition( const Definition& o)
			:m_cmdwriter(o.m_cmdwriter)
			,m_resultreader(o.m_resultreader)
			,m_cmdconstructor(o.m_cmdconstructor){}

		Definition( const OutputFilterR& w, const InputFilterR& r, CreateCommandHandler c)
			:m_cmdwriter(w)
			,m_resultreader(r)
			,m_cmdconstructor(c){}

		OutputFilterR m_cmdwriter;			//< command input writer
		InputFilterR m_resultreader;			//< command result reader
		CreateCommandHandler m_cmdconstructor;

		TransactionFunction create( const std::string& name) const
		{
			TransactionFunction rt;
			rt.m_cmdwriter.reset( m_cmdwriter->copy());
			rt.m_resultreader.reset( m_resultreader->copy());
			rt.m_cmd = m_cmdconstructor( name);
			return rt;
		}
	};
private:
	friend class Defintion;
	OutputFilterR m_cmdwriter;				//< command input writer
	InputFilterR m_resultreader;				//< command result reader
	cmdbind::CommandHandlerR m_cmd;				//< command execute handler
	serialize::FiltermapDDLParseStateStack m_parsestk;	//< STM for result reader
	serialize::FiltermapDDLPrintStateStack m_printstk;	//< STM for command writer
	serialize::Context m_ctx;				//< serialization context
};

///\class TransactionFunctionMap
///\brief Map of available transaction functions seen from scripting language binding
class TransactionFunctionMap
{
public:
	TransactionFunctionMap(){}
	~TransactionFunctionMap(){}

	void defineTransactionFunction( const std::string& name, const TransactionFunction::Definition& f);
	bool hasTransactionFunction( const std::string& name) const;
	bool getTransactionFunction( const std::string& name, TransactionFunction& rt) const;
private:
	std::map<std::string,TransactionFunction::Definition> m_map;
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


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
#include "filter.hpp"
#include "ddl/structType.hpp"
#include "ddl/compilerInterface.hpp"
#include "serialize/struct/filtermapBase.hpp"
#include "protocol/commandHandler.hpp"
#include <stack>
#include <string>
#include <algorithm>
#if WITH_LUA
extern "C" {
	#include "serialize/struct/luamapBase.hpp"
	#include "lua.h"
}
#endif

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
		Data,		///< normal processing
		DoYield,	///< yield because rest of buffer not sufficient to complete operation
		Error		///< logic error in output. Operation is not possible
	};
	///\brief Constructor
	Output() :m_state(0){}
	///\brief Copy constructor
	///\param[in] o copied item
	Output( const Output& o) :m_outputfilter(o.m_outputfilter),m_state(0){}
	///\brief Destructor
	~Output(){}

	///\brief Print the next element
	///\param[in] e1 first element
	///\param[in] e1size first element size
	///\param[in] e2 second element
	///\param[in] e2size second element size
	///\return state returned
	ItemType print( const char* e1, unsigned int e1size, const char* e2, unsigned int e2size);

	const protocol::OutputFilterR& outputfilter() const		{return m_outputfilter;}
	protocol::OutputFilterR& outputfilter()				{return m_outputfilter;}

protected:
	protocol::OutputFilterR m_outputfilter;	///< output filter reference

private:
	unsigned int m_state;						///< current state for outputs with more than one elements
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
	///\brief Destructor
	~Input(){}

	const protocol::InputFilterR& inputfilter() const		{return m_inputfilter;}
	protocol::InputFilterR& inputfilter()				{return m_inputfilter;}
protected:
	protocol::InputFilterR m_inputfilter;			///< input is defined by the associated input filter
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
		EndOfData,	///< End of processed content reached
		Data,		///< normal processing, loop can continue
		DoYield,	///< have to yield and request more network input
		Error		///< have to stop processing because of an error
	};

	///\brief Constructor
	///\param[in] ig input filter reference from input
	InputFilterClosure( const protocol::InputFilterR& ig)
		:m_inputfilter(ig)
		,m_type(protocol::InputFilter::Value)
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

private:
	protocol::InputFilterR m_inputfilter;			//< rerefence to input with filter
	protocol::InputFilter::ElementType m_type;		//< current state (last value type parsed)
	std::string m_attrbuf;					//< buffer for attribute name
	bool m_gotattr;						//< true, if the following value belongs to an attribute
	std::size_t m_taglevel;					//< current level in tag hierarchy
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

	void defineFilter( const char* name, const FilterFactoryR& f);
	bool getFilter( const char* arg, Filter& rt);
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

///\class DDLFormMap
///\brief Map of available forms seen from scripting language binding
class DDLFormMap
{
public:
	DDLFormMap(){}
	~DDLFormMap(){}

	void defineForm( const char* name, const DDLForm& f);
	bool getForm( const char* name, DDLForm& rt) const;
private:
	std::map<std::string,DDLForm> m_map;
};

///\class PluginFunction
class PluginFunction
{
public:
	typedef int (Call)( const void* in, void* out);

	///\brief Default constructor
	PluginFunction() {}

	///\brief Copy constructor
	///\param[in] o copied item
	PluginFunction( const PluginFunction& o)
		:m_call(o.m_call),m_api_param(o.m_api_param),m_api_result(o.m_api_result){}

	///\brief Constructor
	///\param[in] c function to call
	///\param[in] p part of the api describing the input
	///\param[in] r part of the api describing the function result
	PluginFunction( const Call c, const serialize::FiltermapDescriptionBase* p, const serialize::FiltermapDescriptionBase* r)
		:m_call(c),m_api_param(p),m_api_result(r){}

private:
	Call* m_call;
	const serialize::FiltermapDescriptionBase* m_api_param;
	const serialize::FiltermapDescriptionBase* m_api_result;
};

///\class PluginFunctionMap
///\brief Map of available transaction functions seen from scripting language binding
class PluginFunctionMap
{
public:
	PluginFunctionMap(){}
	~PluginFunctionMap(){}

	void definePluginFunction( const char* name, const PluginFunction& f);
	bool getPluginFunction( const char* name, PluginFunction& rt) const;
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
		:m_cmdwriter(o.m_cmdwriter),m_resultreader(o.m_resultreader),m_cmd(o.m_cmd){}

	///\brief Constructor
	///\param[in] w command input writer
	///\param[in] r command output reader
	///\param[in] c command execute handler
	TransactionFunction( const protocol::OutputFilterR& w, const protocol::InputFilterR& r, const protocol::CommandBaseR& c)
		:m_cmdwriter(w),m_resultreader(r),m_cmd(c){}

	///\brief Destructor
	~TransactionFunction(){}

	const protocol::OutputFilterR& cmdwriter() const		{return m_cmdwriter;}
	protocol::OutputFilterR& cmdwriter()				{return m_cmdwriter;}

	const protocol::InputFilterR& resultreader() const		{return m_resultreader;}
	protocol::InputFilterR& resultreader()				{return m_resultreader;}

	const protocol::CommandBaseR& cmd() const			{return m_cmd;}
	protocol::CommandBaseR& cmd()					{return m_cmd;}

private:
	protocol::OutputFilterR m_cmdwriter;				//< command input writer
	protocol::InputFilterR m_resultreader;				//< command result reader
	protocol::CommandBaseR m_cmd;					//< command execute handler
};

///\class TransactionFunctionMap
///\brief Map of available transaction functions seen from scripting language binding
class TransactionFunctionMap
{
public:
	TransactionFunctionMap(){}
	~TransactionFunctionMap(){}

	void defineTransactionFunction( const char* name, const TransactionFunction& f);
	bool getTransactionFunction( const char* name, TransactionFunction& rt) const;
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

	void defineDDLCompiler( const char* name, const ddl::CompilerInterfaceR& ci);
	bool getDDLCompiler( const char* name, ddl::CompilerInterfaceR& rt) const;
private:
	std::map<std::string,ddl::CompilerInterfaceR> m_map;
};


#if WITH_LUA
class LuaScript
{
public:
	struct Module
	{
		std::string m_name;
		lua_CFunction m_initializer;

		Module( const Module& o)				:m_name(o.m_name),m_initializer(o.m_initializer){}
		Module( const std::string& n, const lua_CFunction f)	:m_name(n),m_initializer(f){}
	};

public:
	LuaScript( const char* path_);
	LuaScript( const LuaScript& o)
		:m_modules(o.m_modules),m_path(o.m_path),m_content(o.m_content){}
	~LuaScript(){}

	void addModule( const std::string& n, lua_CFunction f)		{m_modules.push_back( Module( n, f));}

	const std::vector<Module>& modules() const			{return m_modules;}
	const std::string& path() const					{return m_path;}
	const std::string& content() const				{return m_content;}

private:
	std::vector<Module> m_modules;
	std::string m_path;
	std::string m_content;
};

class LuaScriptInstance
{
public:
	explicit LuaScriptInstance( const LuaScript* script);
	~LuaScriptInstance();

	lua_State* ls()				{return m_ls;}
	lua_State* thread()			{return m_thread;}
private:
	lua_State* m_ls;
	lua_State* m_thread;
	int m_threadref;
	const LuaScript* m_script;

private:
	LuaScriptInstance( const LuaScriptInstance&){}
};

typedef CountedReference<LuaScriptInstance> LuaScriptInstanceR;


///\class LuaFunctionMap
///\brief Map of available Lua functions
class LuaFunctionMap
{
public:
	LuaFunctionMap(){}
	~LuaFunctionMap();

	void defineLuaFunction( const char* procname, const LuaScript& script);
	bool getLuaScriptInstance( const char* procname, LuaScriptInstanceR& rt) const;
private:
	LuaFunctionMap( const LuaFunctionMap&){}

private:
	std::vector<LuaScript*> m_ar;
	std::map<std::string,std::size_t> m_pathmap;
	std::map<std::string,std::size_t> m_procmap;
};

///\class LuaPluginFunction
class LuaPluginFunction
{
public:
	typedef int (Call)( const void* in, void* out);

	///\brief Default constructor
	LuaPluginFunction() {}

	///\brief Copy constructor
	///\param[in] o copied item
	LuaPluginFunction( const LuaPluginFunction& o)
		:m_call(o.m_call),m_api_param(o.m_api_param),m_api_result(o.m_api_result){}

	///\brief Constructor
	///\param[in] c function to call
	///\param[in] p part of the api describing the input
	///\param[in] r part of the api describing the function result
	LuaPluginFunction( const Call c, const serialize::LuamapDescriptionBase* p, const serialize::LuamapDescriptionBase* r)
		:m_call(c),m_api_param(p),m_api_result(r){}

	int call( lua_State* ls) const;
private:
	Call* m_call;
	const serialize::LuamapDescriptionBase* m_api_param;
	const serialize::LuamapDescriptionBase* m_api_result;
};

///\class LuaPluginFunctionMap
///\brief Map of available plugin functions seen from scripting language binding
class LuaPluginFunctionMap
{
public:
	LuaPluginFunctionMap(){}
	~LuaPluginFunctionMap(){}

	void defineLuaPluginFunction( const char* name, const LuaPluginFunction& f);
	bool getLuaPluginFunction( const char* name, LuaPluginFunction& rt) const;
private:
	std::map<std::string,LuaPluginFunction> m_map;
};

#else

struct LuaFunctionMap {};
struct LuaPluginFunctionMap {};

#endif
}} //namespace
#endif


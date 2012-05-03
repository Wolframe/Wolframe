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
///\file langbind_appObjects.cpp
///\brief implementation of scripting language objects
#include "langbind/appObjects.hpp"
#include "serialize/ddl/filtermapSerialize.hpp"
#include "ddl/compiler/simpleFormCompiler.hpp"
#include "logger-v1.hpp"
#include "protocol/inputfilter.hpp"
#include "protocol/outputfilter.hpp"
#include "filter/char_filter.hpp"
#include "filter/line_filter.hpp"
#include "filter/token_filter.hpp"
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>

#ifdef WITH_LIBXML2
#include "filter/libxml2_filter.hpp"
#endif
#include "filter/textwolf_filter.hpp"
#ifdef WITH_XMLLITE
#include "filter/xmllite_filter.hpp"
#endif
#if WITH_MSXML
#include "filter/msxml_filter.hpp"
#endif
#if WITH_LUA
#include "langbind/luaDebug.hpp"
extern "C" {
	#include <lualib.h>
	#include <lauxlib.h>
	#include <lua.h>
}
#endif

using namespace _Wolframe;
using namespace langbind;

namespace //anonymous
{
template <class Object>
void defineObject( std::map<std::string,Object>& m_map, const std::string& name, const Object& obj)
{
	std::string nam( name);
	std::transform( nam.begin(), nam.end(), nam.begin(), ::tolower);
	m_map[ nam] = obj;
}

template <class Object>
bool getObject( const std::map<std::string,Object>& m_map, const std::string& name, Object& obj)
{
	std::string nam( name);
	std::transform( nam.begin(), nam.end(), nam.begin(), ::tolower);
	typename std::map<std::string,Object>::const_iterator ii=m_map.find( nam),ee=m_map.end();
	if (ii == ee)
	{
		return false;
	}
	else
	{
		obj = ii->second;
		return true;
	}
}
}//anonymous namespace

void FilterMap::defineFilter( const std::string& name, const FilterFactoryR& f)
{
	defineObject( m_map, name, f);
}

bool FilterMap::getFilter( const std::string& arg, Filter& rt)
{
	std::size_t nn = arg.size();
	std::size_t ii = nn;
	std::string nam( arg);
	std::transform( nam.begin(), nam.end(), nam.begin(), ::tolower);
	do
	{
		nam.resize(ii);
		std::map<std::string,FilterFactoryR>::const_iterator itr=m_map.find( nam),end=m_map.end();
		if (itr != end)
		{
			rt = itr->second->create( (ii==nn)?0:(arg.c_str()+ii+1));
			return true;
		}
		for (ii=nn; ii>0 && arg[ii] != ':'; --ii);
	}
	while (ii>0);
	return false;
}

FilterMap::FilterMap()
{
	defineFilter( "char", FilterFactoryR( new CharFilterFactory()));
	defineFilter( "line", FilterFactoryR( new LineFilterFactory()));
	defineFilter( "xml:textwolf", FilterFactoryR( new TextwolfXmlFilterFactory()));
#ifdef WITH_LIBXML2
	defineFilter( "xml:libxml2", FilterFactoryR( new Libxml2FilterFactory()));
#endif
#ifdef WITH_XMLLITE
	defineFilter( "xml:xmllite", FilterFactoryR( new Libxml2FilterFactory()));
#endif
#ifdef WITH_MSXML
	defineFilter( "xml:msxml", FilterFactoryR( new Libxml2FilterFactory()));
#endif
}

void DDLFormMap::defineForm( const std::string& name, const DDLForm& f)
{
	defineObject( m_map, name, f);
}

bool DDLFormMap::getForm( const std::string& name, DDLFormR& rt) const
{
	rt = DDLFormR( new DDLForm());
	return getObject( m_map, name, *rt.get());
}

PluginFunction::CallResult PluginFunction::call( protocol::InputFilter& ifl, protocol::OutputFilter& ofl)
{
	if (m_state) return Error;

	serialize::Context ctx;
	void* input_struct = m_data.get();
	void* result_struct = (void*)((char*)m_data.get() + m_api_param->size());

	switch (m_state)
	{
		case 0:
			if (!m_api_param->init( input_struct))
			{
				LOG_ERROR << "Could not initialize api input object for plugin function";
				return Error;
			}
			m_state = 1;
		case 1:
			if (!m_api_result->init( result_struct))
			{
				LOG_ERROR << "Could not initialize api result object for plugin function";
				return Error;
			}
			m_state = 2;
		case 2:
			if (!m_api_param->parse( "", input_struct, ifl, ctx))
			{
				switch (ifl.state())
				{
					case protocol::InputFilter::Open:
						ctx.setError( 0, "Unknown error");
						return Error;

					case protocol::InputFilter::EndOfMessage:
						return Yield;

					case protocol::InputFilter::Error:
						ctx.setError( 0, ifl.getError());
						return Error;
				}
			}
			m_state = 3;
		case 3:
			try
			{
				int rt = m_call( input_struct, result_struct);
				if (rt != 0)
				{
					LOG_ERROR << "error in call of plugin function. error code = " << rt;
					return Error;
				}
			}
			catch (const std::exception& e)
			{
				LOG_ERROR << "exception in plugin function call: " << e.what();
				return Error;
			}
			m_state = 4;
		case 4:
			if (!m_api_param->print( "", result_struct, ofl, ctx))
			{
				switch (ofl.state())
				{
					case protocol::OutputFilter::Open:
						ctx.setError( 0, "Unknown error");
						return Error;

					case protocol::OutputFilter::EndOfBuffer:
						return Yield;

					case protocol::OutputFilter::Error:
						ctx.setError( 0, ofl.getError());
						return Error;
				}
			}
			m_state = 5;
		case 5:
			m_api_param->done( input_struct);
			m_api_result->done( result_struct);
			std::memset( m_data.get(), 0, m_api_param->size() + m_api_result->size());
			m_state = 0;
	}
	return Ok;
}

PluginFunction::~PluginFunction()
{
	if (m_state >= 0)
	{
		void* input_struct = m_data.get();
		void* result_struct = (void*)((char*)m_data.get() + m_api_param->size());

		switch (m_state)
		{
			case 0:
				break;
			case 5:
			case 4:
			case 3:
			case 2:
				m_api_result->done( result_struct);
			case 1:
				m_api_param->done( input_struct);
		}
	}
}

void PluginFunctionMap::definePluginFunction( const std::string& name, const PluginFunction& f)
{
	defineObject( m_map, name, f);
}

bool PluginFunctionMap::getPluginFunction( const std::string& name, PluginFunction& rt) const
{
	return getObject( m_map, name, rt);
}

bool TransactionFunction::call( const DDLForm& param, DDLForm& result)
{
	cmdbind::CommandHandler* cmd = m_cmd.get();
	protocol::OutputFilter* wr = m_cmdwriter.get();
	protocol::InputFilter* rd = m_resultreader.get();
	if (!cmd || !wr || !rd)
	{
		LOG_ERROR << "incomplete transaction function call definition";
		return false;
	}
	protocol::OutputFilterR wrs( wr = wr->copy());
	protocol::InputFilterR rds( rd = rd->copy());

	serialize::Context pctx;
	if (!serialize::print( param.m_struct, wr, pctx))
	{
		LOG_ERROR << "error writing transaction command: " << pctx.getLastError();
		return false;
	}
	enum {inbufsize=1024, outbufsize=1024};
	char inbuf[ inbufsize];
	char outbuf[ outbufsize];
	std::size_t ii = 0;

	cmd->setInputBuffer( inbuf, inbufsize);
	cmd->setOutputBuffer( outbuf, outbufsize, 0);

	std::string resultstr;
	cmdbind::CommandHandler::Operation op = cmdbind::CommandHandler::READ;

	for(;;)
	{
		switch (op)
		{
			case cmdbind::CommandHandler::READ:
			{
				std::size_t rr = pctx.content().size()-ii;
				if (rr > inbufsize) rr = inbufsize;
				memcpy( inbuf, pctx.content().c_str()+ii, rr);
				cmd->putInput( inbuf, rr);
				op = cmd->nextOperation();
				continue;
			}
			case cmdbind::CommandHandler::WRITE:
			{
				const void* oo;
				std::size_t nn;
				cmd->getOutput( oo, nn);
				resultstr.append( (const char*)oo, nn);
				op = cmd->nextOperation();
				continue;
			}
			case cmdbind::CommandHandler::CLOSED:
			{
				serialize::Context rctx;
				rd->protocolInput( resultstr.c_str(), resultstr.size(), true);
				if (!serialize::parse( result.m_struct, *rd, rctx))
				{
					LOG_ERROR << "error reading transaction result: " << rctx.getLastError();
					return false;
				}
				return true;
			}
		}
	}
}

bool TransactionFunctionMap::hasTransactionFunction( const std::string& name) const
{
	const char* ee = std::strchr( name.c_str(), ':');
	if (ee)
	{
		std::string nam( name.c_str(), ee-name.c_str());
		std::transform( nam.begin(), nam.end(), nam.begin(), ::tolower);
		return m_map.find( nam) != m_map.end();
	}
	else
	{
		std::string nam( name);
		std::transform( nam.begin(), nam.end(), nam.begin(), ::tolower);
		return m_map.find( nam) != m_map.end();
	}
}

void TransactionFunctionMap::defineTransactionFunction( const std::string& name, const TransactionFunction::Definition& f)
{
	defineObject( m_map, name, f);
}

bool TransactionFunctionMap::getTransactionFunction( const std::string& name, TransactionFunction& rt) const
{
	TransactionFunction::Definition def;
	if (!getObject( m_map, name, def)) return false;
	rt = def.create( name);
	return true;
}

DDLCompilerMap::DDLCompilerMap()
{
	ddl::CompilerInterfaceR simpleformCompiler( new ddl::SimpleFormCompiler());
	m_map[ simpleformCompiler->ddlname()] = simpleformCompiler;
}

void DDLCompilerMap::defineDDLCompiler( const std::string& name, const ddl::CompilerInterfaceR& f)
{
	defineObject( m_map, name, f);
}

bool DDLCompilerMap::getDDLCompiler( const std::string& name, ddl::CompilerInterfaceR& rt) const
{
	return getObject( m_map, name, rt);
}

static InputFilterClosure::ItemType fetchFailureResult( const protocol::InputFilter& ff)
{
	const char* msg;
	switch (ff.state())
	{
		case protocol::InputFilter::EndOfMessage:
			return InputFilterClosure::DoYield;

		case protocol::InputFilter::Error:
			msg = ff.getError();
			LOG_ERROR << "error in input filter (" << (msg?msg:"unknown") << ")";
			return InputFilterClosure::Error;

		case protocol::InputFilter::Open:
			LOG_DATA << "end of input";
			return InputFilterClosure::EndOfData;
	}
	LOG_ERROR << "illegal state in iterator";
	return InputFilterClosure::Error;
}

InputFilterClosure::ItemType InputFilterClosure::fetch( const char*& tag, unsigned int& tagsize, const char*& val, unsigned int& valsize)
{
	const void* element;
	std::size_t elementsize;

AGAIN:
	if (!m_inputfilter.get())
	{
		return EndOfData;
	}
	if (!m_inputfilter->getNext( m_type, element, elementsize))
	{
		if (m_inputfilter->state() == protocol::InputFilter::Open)
		{
			// at end of data check if there is a follow filter (transformed filter) to continue with:
			protocol::InputFilter* follow = m_inputfilter->createFollow();
			if (follow)
			{
				m_inputfilter.reset( follow);
				goto AGAIN;
			}
		}
		return fetchFailureResult( *m_inputfilter);
	}
	else
	{
		switch (m_type)
		{
			case protocol::InputFilter::OpenTag:
				m_taglevel += 1;
				tag = (const char*)element;
				tagsize = elementsize;
				val = 0;
				valsize = 0;
				m_gotattr = false;
				return Data;

			case protocol::InputFilter::Value:
				if (m_gotattr)
				{
					tag = m_attrbuf.c_str();
					tagsize = m_attrbuf.size();
					m_gotattr = false;
				}
				else
				{
					tag = 0;
					tagsize = 0;
				}
				val = (const char*)element;
				valsize = elementsize;
				return Data;

			 case protocol::InputFilter::Attribute:
				m_attrbuf.clear();
				m_attrbuf.append( (const char*)element, elementsize);
				m_gotattr = true;
				goto AGAIN;

			 case protocol::InputFilter::CloseTag:
				tag = 0;
				tagsize = 0;
				val = 0;
				valsize = 0;
				m_gotattr = false;
				if (m_taglevel == 0)
				{
					return EndOfData;
				}
				else
				{
					m_taglevel -= 1;
					return Data;
				}
		}
	}
	LOG_ERROR << "illegal state in iterator";
	return Error;
}

Output::ItemType Output::print( const char* tag, unsigned int tagsize, const char* val, unsigned int valsize)
{
	if (!m_outputfilter.get())
	{
		LOG_ERROR << "no output sink defined (output ignored)";
		return Error;
	}
	try
	{
		if (tag)
		{
			if (val)
			{
				switch (m_state)
				{
					case 0:
						if (!m_outputfilter->print( protocol::OutputFilter::Attribute, tag, tagsize)) break;
						m_state ++;
					case 1:
						if (!m_outputfilter->print( protocol::OutputFilter::Value, val, valsize)) break;
						m_state ++;
					case 2:
						m_state = 0;
						return Data;
				}
				const char* err = m_outputfilter->getError();
				if (err)
				{
					LOG_ERROR << "error in output filter (" << err << ")";
					return Error;
				}
				else if (m_outputfilter->state() != protocol::OutputFilter::EndOfBuffer)
				{
					// in case of return false and no error check if there is a follow format ouptut filter to continue with:
					protocol::OutputFilter* follow = m_outputfilter->createFollow();
					if (follow)
					{
						m_outputfilter.reset( follow);
						return print( tag, tagsize, val, valsize);
					}

				}
				return DoYield;
			}
			else
			{
				if (!m_outputfilter->print( protocol::OutputFilter::OpenTag, tag, tagsize))
				{
					const char* err = m_outputfilter->getError();
					if (err)
					{
						LOG_ERROR << "error in output filter open tag (" << err << ")";
						return Error;
					}
					else  if (m_outputfilter->state() != protocol::OutputFilter::EndOfBuffer)
					{
						// in case of return false and no error check if there is a follow format ouptut filter to continue with:
						protocol::OutputFilter* follow = m_outputfilter->createFollow();
						if (follow)
						{
							m_outputfilter.reset( follow);
							return print( tag, tagsize, val, valsize);
						}
					}
					return DoYield;
				}
				return Data;
			}
		}
		else if (val)
		{
			if (!m_outputfilter->print( protocol::OutputFilter::Value, val, valsize))
			{
				const char* err = m_outputfilter->getError();
				if (err)
				{
					LOG_ERROR << "error in output filter value (" << err << ")";
					return Error;
				}
				else if (m_outputfilter->state() != protocol::OutputFilter::EndOfBuffer)
				{
					// in case of return false and no error check if there is a follow format ouptut filter to continue with:
					protocol::OutputFilter* follow = m_outputfilter->createFollow();
					if (follow)
					{
						m_outputfilter.reset( follow);
						return print( tag, tagsize, val, valsize);
					}
				}
				return DoYield;
			}
			return Data;
		}
		else
		{
			if (!m_outputfilter->print( protocol::OutputFilter::CloseTag, 0, 0))
			{
				const char* err = m_outputfilter->getError();
				if (err)
				{
					LOG_ERROR << "error in output filter close tag (" << err << ")";
					return Error;
				}
				else if (m_outputfilter->state() != protocol::OutputFilter::EndOfBuffer)
				{
					// in case of return false and no error check if there is a follow format ouptut filter to continue with:
					protocol::OutputFilter* follow = m_outputfilter->createFollow();
					if (follow)
					{
						m_outputfilter.reset( follow);
						return print( tag, tagsize, val, valsize);
					}
				}
				return DoYield;
			}
			return Data;
		}
	}
	catch (std::bad_alloc)
	{
		LOG_ERROR << "out of memory in output filter";
		return Error;
	}
}

#if WITH_LUA
static int function_printlog( lua_State *ls)
{
	/* first parameter maps to a log level, rest gets printed depending on
	 * whether it's a string or a number
	 */
	int ii,nn = lua_gettop(ls);
	if (nn <= 0)
	{
		LOG_ERROR << "no arguments passed to 'printlog'";
		return 0;
	}
	const char *logLevel = luaL_checkstring( ls, 1);
	std::string logmsg;

	for (ii=2; ii<=nn; ii++)
	{
		if (!getDescription( ls, ii, logmsg))
		{
			LOG_ERROR << "failed to map 'printLog' arguments to a string";
		}
	}
	_Wolframe::log::LogLevel::Level lv = _Wolframe::log::LogLevel::strToLogLevel( logLevel);
	if (lv == _Wolframe::log::LogLevel::LOGLEVEL_UNDEFINED)
	{
		LOG_ERROR << "'printLog' called with undefined loglevel '" << logLevel << "' as first argument";
	}
	else
	{
		_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( lv )
			<< _Wolframe::log::LogComponent::LogLua
			<< logmsg;
	}
	return 0;
}

LuaScript::LuaScript( const std::string& path_)
	:m_path(path_)
{
	char buf;
	std::fstream ff;
	ff.open( path_.c_str(), std::ios::in);
	while (ff.read( &buf, sizeof(buf)))
	{
		m_content.push_back( buf);
	}
	if ((ff.rdstate() & std::ifstream::eofbit) == 0)
	{
		LOG_ERROR << "failed to read lua script from file: '" << path_ << "'";
		throw std::runtime_error( "read lua script from file");
	}
	ff.close();
}

LuaScriptInstance::LuaScriptInstance( const LuaScript* script_)
	:m_ls(0),m_thread(0),m_threadref(0),m_script(script_)
{
	m_ls = luaL_newstate();
	if (!m_ls) throw std::runtime_error( "failed to create lua state");

	// create thread and prevent garbage collecting of it (http://permalink.gmane.org/gmane.comp.lang.lua.general/22680)
	m_thread = lua_newthread( m_ls);
	lua_pushvalue( m_ls, -1);
	m_threadref = luaL_ref( m_ls, LUA_REGISTRYINDEX);

	if (luaL_loadbuffer( m_ls, m_script->content().c_str(), m_script->content().size(), m_script->path().c_str()))
	{
		std::ostringstream buf;
		buf << "Failed to load script '" << m_script->path() << "':" << lua_tostring( m_ls, -1);
		throw std::runtime_error( buf.str());
	}
	// open standard lua libraries
	luaL_openlibs( m_ls);

	// register logging function already here because then it can be used in the script initilization part
	lua_pushcfunction( m_ls, &function_printlog);
	lua_setglobal( m_ls, "printlog");

	// open additional libraries defined for this script
	std::vector<LuaScript::Module>::const_iterator ii=m_script->modules().begin(), ee=m_script->modules().end();
	for (;ii!=ee; ++ii)
	{
		if (ii->m_initializer( m_ls))
		{
			std::ostringstream buf;
			buf << "module '" << ii->m_name << "' initialization failed: " << lua_tostring( m_ls, -1);
			throw std::runtime_error( buf.str());
		}
	}

	// call main, we may have to initialize LUA modules there
	if (lua_pcall( m_ls, 0, LUA_MULTRET, 0) != 0)
	{
		std::ostringstream buf;
		buf << "Unable to call main entry of script: " << lua_tostring( m_ls, -1 );
		throw std::runtime_error( buf.str());
	}
}

LuaScriptInstance::~LuaScriptInstance()
{
	if (m_ls)
	{
		luaL_unref( m_ls, LUA_REGISTRYINDEX, m_threadref);
		lua_close( m_ls);
	}
}

LuaFunctionMap::~LuaFunctionMap()
{
	std::vector<LuaScript*>::iterator ii=m_ar.begin(),ee=m_ar.end();
	while (ii != ee)
	{
		delete *ii;
		++ii;
	}
}

void LuaFunctionMap::defineLuaFunction( const std::string& name, const LuaScript& script)
{
	std::string nam( name);
	std::transform( nam.begin(), nam.end(), nam.begin(), ::tolower);
	{
		std::map<std::string,std::size_t>::const_iterator ii=m_procmap.find( nam),ee=m_procmap.end();
		if (ii != ee)
		{
			std::ostringstream buf;
			buf << "Duplicate definition of function '" << nam << "'";
			throw std::runtime_error( buf.str());
		}
	}
	std::size_t scriptId;
	std::map<std::string,std::size_t>::const_iterator ii=m_pathmap.find( script.path()),ee=m_pathmap.end();
	if (ii != ee)
	{
		scriptId = ii->second;
	}
	else
	{
		scriptId = m_ar.size();
		m_ar.push_back( new LuaScript( script));	//< load its content from file
		LuaScriptInstance( m_ar.back());		//< check, if it can be compiled
		m_pathmap[ script.path()] = scriptId;
	}
	m_procmap[ nam] = scriptId;
}

bool LuaFunctionMap::getLuaScriptInstance( const std::string& procname, LuaScriptInstanceR& rt) const
{
	std::string nam( procname);
	std::transform( nam.begin(), nam.end(), nam.begin(), ::tolower);

	std::map<std::string,std::size_t>::const_iterator ii=m_procmap.find( nam),ee=m_procmap.end();
	if (ii == ee) return false;
	rt = LuaScriptInstanceR( new LuaScriptInstance( m_ar[ ii->second]));
	return true;
}

int LuaPluginFunction::call( lua_State* ls) const
{
	bool ok = true;
	serialize::Context ctx;
	int rt = 0;

	void* dt = lua_newuserdata( ls, m_api_param->size() + m_api_result->size());
	std::memset( dt, 0, m_api_param->size() + m_api_result->size());
	void* input_struct = (void*)dt;
	void* result_struct = (void*)((char*)dt + m_api_param->size());

	if (!m_api_param->init( input_struct))
	{
		ctx.setError( 0, "Could not initialize api input object");
		ok = false;
		goto EXIT_FUNCTION1;
	}
	if (!m_api_result->init( result_struct))
	{
		m_api_param->done( input_struct);
		ctx.setError( 0, "Could not initialize api result object");
		ok = false;
		goto EXIT_FUNCTION2;
	}
	if (!m_api_param->parse( input_struct, ls, &ctx))
	{
		ok = false;
		goto EXIT_FUNCTION3;
	}
	try
	{
		rt = m_call( input_struct, result_struct);
		if (rt != 0)
		{
			m_api_param->done( input_struct);
			m_api_result->done( result_struct);
			lua_pushnumber( ls, rt);
			return 1;
		}
	}
	catch (const std::exception& e)
	{
		ctx.setError( 0, e.what());
		ok = false;
		goto EXIT_FUNCTION3;
	}
	if (!m_api_param->print( result_struct, ls, &ctx))
	{
		ok = false;
		goto EXIT_FUNCTION3;
	}
EXIT_FUNCTION3:
	m_api_param->done( input_struct);
EXIT_FUNCTION2:
	m_api_result->done( result_struct);
EXIT_FUNCTION1:
	if (!ok)
	{
		luaL_error( ls, ctx.getLastError());
	}
	return 1;
}

void LuaPluginFunctionMap::defineLuaPluginFunction( const std::string& name, const LuaPluginFunction& f)
{
	defineObject( m_map, name, f);
}

bool LuaPluginFunctionMap::getLuaPluginFunction( const std::string& name, LuaPluginFunction& rt) const
{
	return getObject( m_map, name, rt);
}

#endif





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
#include "serialize/ddl/filtermapDDLParse.hpp"
#include "serialize/ddl/filtermapDDLPrint.hpp"
#include "ddl/compiler/simpleFormCompiler.hpp"
#include "logger-v1.hpp"
#include "filter/filter.hpp"
#include "filter/serializefilter.hpp"
#include "filter/char_filter.hpp"
#include "filter/line_filter.hpp"
#include "filter/token_filter.hpp"
#include "filter/expression_filter.hpp"
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
	std::size_t aa = nn;
	std::string nam( arg);
	std::transform( nam.begin(), nam.end(), nam.begin(), ::tolower);
	do
	{
		nam.resize(ii);
		std::map<std::string,FilterFactoryR>::const_iterator itr=m_map.find( nam),end=m_map.end();
		if (itr != end)
		{
			rt = itr->second->create( (ii==nn)?0:(arg.c_str()+aa));
			return true;
		}
		for (nn=ii; ii>0 && arg[ii] != ':'; --ii);
		aa = ii + 1;
		for (++ii; ii>0 && arg[ii-1] == ':'; --ii);
	}
	while (ii>0 && ii<nn);
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

bool DDLForm::getValue( const char* name, std::string& val) const
{
	serialize::Context::Flags flt;
	if (serialize::Context::getFlag( name, flt))
	{
		if ((int)flt & (int)m_flags)
		{
			val = "true";
		}
		else
		{
			val = "false";
		}
		return true;
	}
	return false;
}

bool DDLForm::setValue( const char* name, const std::string& value)
{
	serialize::Context::Flags flt;
	if (serialize::Context::getFlag( name, flt))
	{
		if (value == "true")
		{
			m_flags = (serialize::Context::Flags)((int)flt | (int)m_flags);
		}
		else if (value == "false")
		{
			if ((int)flt & (int)m_flags)
			{
				m_flags = (serialize::Context::Flags)((int)flt ^ (int)m_flags);
			}
		}
		else
		{
			throw std::runtime_error( "illegal value for boolean");
		}
		return true;
	}
	return false;
}

std::string DDLForm::tostring() const
{
	std::ostringstream out;
	m_structure.print( out);
	return out.str();
}

DDLFormFill::DDLFormFill( const DDLFormR& f)
	:m_form(f)
	,m_state(0)
	,m_ctx(f->flags()){}

DDLFormFill::DDLFormFill( const DDLFormR& f, const TypedInputFilterR& inp)
	:m_form(f)
	,m_state(0)
	,m_inputfilter(inp)
	,m_ctx(f->flags()){}

DDLFormFill::DDLFormFill( const DDLFormFill& o)
	:m_form(o.m_form)
	,m_state(o.m_state)
	,m_inputfilter(o.m_inputfilter)
	,m_ctx(o.m_ctx)
	,m_parsestk(o.m_parsestk){}

void DDLFormFill::init( const TypedInputFilterR& i)
{
	m_inputfilter = i;
	m_ctx.clear();
	m_parsestk.clear();
	m_state = 0;
}

DDLFormFill::CallResult DDLFormFill::call()
{
	if (m_state > 0) return Ok;
	switch (m_state)
	{
		case 0:
			if (!m_inputfilter.get())
			{
				m_ctx.setError( "no input filter defined for form fill");
				return Error;
			}
			if (!m_form.get())
			{
				m_ctx.setError( "no form defined for form fill");
				return Error;
			}
			if (!serialize::parse( m_form->m_structure, *m_inputfilter, m_ctx, m_parsestk))
			{
				if (m_ctx.getLastError()) return Error;
				switch (m_inputfilter->state())
				{
					case InputFilter::Open:
						m_ctx.setError( "unknown error in form fill");
						return Error;

					case InputFilter::EndOfMessage:
						return Yield;

					case InputFilter::Error:
						m_ctx.setError( m_inputfilter->getError());
						return Error;
				}
			}
			m_state = 1;
	}
	return Ok;
}

DDLFormPrint::DDLFormPrint( const DDLFormR& f)
	:m_form(f)
	,m_state(0)
	,m_ctx(f->flags()){}

DDLFormPrint::DDLFormPrint( const DDLFormR& f, const TypedOutputFilterR& outp)
	:m_form(f)
	,m_state(1)
	,m_outputfilter(outp)
	,m_ctx(f->flags()){}

DDLFormPrint::DDLFormPrint( const DDLFormPrint& o)
	:m_form(o.m_form)
	,m_state(o.m_state)
	,m_outputfilter(o.m_outputfilter)
	,m_ctx(o.m_ctx)
	,m_printstk(o.m_printstk){}

void DDLFormPrint::init( const TypedOutputFilterR& o)
{
	m_outputfilter = o;
	m_ctx.clear();
	m_printstk.clear();
	m_state = 1;
}

DDLFormPrint::CallResult DDLFormPrint::fetch()
{
	if (m_state == 0) return Error;
	if (m_state == 2) return Ok;
	if (!m_outputfilter.get())
	{
		m_ctx.setError( "no output filter defined for fetching the form iterator");
		return Error;
	}
	if (!m_form.get())
	{
		m_ctx.setError( "no form defined for fetching the form iterator");
		return Error;
	}
	if (!serialize::print( m_form->structure(), *m_outputfilter, m_ctx, m_printstk))
	{
		if (m_ctx.getLastError()) return Error;
		switch (m_outputfilter->state())
		{
			case OutputFilter::Open:
				m_ctx.setError( "unknown error in form print");
				return Error;

			case OutputFilter::EndOfBuffer:
				return Yield;

			case OutputFilter::Error:
				m_ctx.setError( m_outputfilter->getError());
				return Error;
		}
	}
	m_state = 2;
	return Ok;
}

void DDLFormMap::defineForm( const std::string& name, const DDLForm& f)
{
	defineObject( m_map, name, DDLFormR( new DDLForm(f)));
}

bool DDLFormMap::getForm( const std::string& name, DDLFormR& rt) const
{
	DDLFormR obj;
	if (!getObject( m_map, name, obj) || !obj.get()) return false;
	rt = DDLFormR( new DDLForm( *obj));
	return true;
}

FormFunction::FormFunction()
	:m_function(0)
	,m_api_param(0)
	,m_api_result(0){}

FormFunction::FormFunction( const FormFunction& o)
	:m_function(o.m_function)
	,m_api_param(o.m_api_param)
	,m_api_result(o.m_api_result){}

FormFunction::FormFunction( Function f, const serialize::FiltermapDescriptionBase* p, const serialize::FiltermapDescriptionBase* r)
	:m_function(f)
	,m_api_param(p)
	,m_api_result(r){}

FormFunctionResult::FormFunctionResult( const FormFunction& f)
	:m_description(f.api_result())
	,m_state(0)
	,m_data(std::calloc( f.api_result()->size(), 1), std::free)
{
	if (!m_data.get()) throw std::bad_alloc();
	void* result_struct = m_data.get();
	if (!result_struct || !m_description->init( result_struct))
	{
		m_ctx.setError( "could not initialize api result object for form function");
	}
	else
	{
		m_state = 1;
	}
}

FormFunctionResult::FormFunctionResult( const FormFunctionResult& o)
	:m_description(o.m_description)
	,m_state(o.m_state)
	,m_data(o.m_data)
	,m_outputfilter(o.m_outputfilter)
	,m_ctx(o.m_ctx)
	,m_printstk(o.m_printstk){}

FormFunctionResult::~FormFunctionResult()
{
	if (m_state)
	{
		m_description->done( m_data.get());
		std::memset( m_data.get(), 0, m_description->size());
	}
}

void FormFunctionResult::init( const TypedOutputFilterR& o)
{
	m_outputfilter = o;
	m_ctx.clear();
	m_printstk.clear();
	m_state = 1;
}

FormFunctionResult::CallResult FormFunctionResult::fetch()
{
	if (m_state == 0) return Error;
	if (m_state == 2) return Ok;
	const void* result_struct = m_data.get();
	if (!m_outputfilter.get())
	{
		m_ctx.setError( "no output filter defined for fetching the form function result");
		return Error;
	}
	if (!m_description->print( result_struct, *m_outputfilter, m_ctx, m_printstk))
	{
		switch (m_outputfilter->state())
		{
			case OutputFilter::Open:
				return Error;

			case OutputFilter::EndOfBuffer:
				return Yield;

			case OutputFilter::Error:
				m_ctx.setError( m_outputfilter->getError());
				return Error;
		}
	}
	m_state = 2;
	return Ok;
}

FormFunctionClosure::FormFunctionClosure( const FormFunction& f)
	:FormFunction(f)
	,m_state(0)
	,m_result(f)
	,m_data(std::calloc( f.api_param()->size(), 1), std::free)
{
	if (!m_data.get()) throw std::bad_alloc();
	if (!f.api_param()->init( m_data.get()))
	{
		m_ctx.setError( "could not initialize objects for form function");
	}
	else
	{
		m_state = 1;
	}
}

FormFunctionClosure::FormFunctionClosure( const FormFunctionClosure& o)
	:FormFunction(o)
	,m_state(o.m_state)
	,m_result(o.m_result)
	,m_data(o.m_data)
	,m_parsestk(o.m_parsestk)
	,m_ctx(o.m_ctx)
	,m_inputfilter(o.m_inputfilter)
	{}

FormFunctionClosure::~FormFunctionClosure()
{
	if (m_state)
	{
		api_param()->done( m_data.get());
		std::memset( m_data.get(), 0, api_param()->size());
	}
}

void FormFunctionClosure::init( const TypedInputFilterR& i)
{
	m_inputfilter = i;
	m_ctx.clear();
	m_parsestk.clear();
	m_state = 1;
}

FormFunctionClosure::CallResult FormFunctionClosure::call()
{
	if (m_state < 1) return Error;
	void* param_struct = m_data.get();

	switch (m_state)
	{
		case 1:
			if (!m_inputfilter.get())
			{
				m_ctx.setError( "no input filter defined for form function");
				return Error;
			}
			if (!api_param()->parse( param_struct, *m_inputfilter, m_ctx, m_parsestk))
			{
				switch (m_inputfilter->state())
				{
					case InputFilter::Open:
						if (m_ctx.getLastError())
						{
							return Error;
						}
						break;

					case InputFilter::EndOfMessage:
						return Yield;

					case InputFilter::Error:
						m_ctx.setError( m_inputfilter->getError());
						return Error;
				}
			}
			m_state = 2;
		case 2:
			try
			{
				int rt = FormFunction::function()( m_result.data(), param_struct);
				if (rt != 0)
				{
					m_ctx.setError( "error in call of form function");
					return Error;
				}
			}
			catch (const std::exception& e)
			{
				m_ctx.setError( e.what());
				return Error;
			}
			m_state = 3;
	}
	return Ok;
}

void FormFunctionMap::defineFormFunction( const std::string& name, const FormFunction& f)
{
	defineObject( m_map, name, f);
}

bool FormFunctionMap::getFormFunction( const std::string& name, FormFunction& rt) const
{
	return getObject( m_map, name, rt);
}

TransactionFunctionResult::TransactionFunctionResult( const TransactionFunction& f)
	:m_func(f)
	,m_state(0)
	{}

TransactionFunctionResult::TransactionFunctionResult( const TransactionFunctionResult& o)
	:m_func(o.m_func)
	,m_state(o.m_state)
	,m_lasterror(o.m_lasterror)
	,m_elemtype(o.m_elemtype)
	,m_elem(o.m_elem)
	,m_resultbuf(o.m_resultbuf)
	,m_resultreader(o.m_resultreader)
	,m_outputfilter(o.m_outputfilter){}


TransactionFunctionResult::CallResult TransactionFunctionResult::fetch()
{
	if (!m_outputfilter.get())
	{
		m_lasterror = "no output filter specified for transaction function result";
		m_state = 0;
		return Error;
	}
	for (;;) switch (m_state)
	{
		case 0: return Error;
		case 1:
		{
			SerializeInputFilter si( m_resultreader.get());
			if (!si.getNext( m_elemtype, m_elem))
			{
				switch (si.state())
				{
					case InputFilter::Open:
						return Ok;
					case InputFilter::EndOfMessage:
						m_lasterror = "unexpected end of message in result reader";
						break;
					case InputFilter::Error:
						if (si.getError())
						{
							m_lasterror.append( si.getError());
						}
						break;
				}
				return Error;
			}
			m_state = 2;
		}
		case 2:
		{
			if (!m_outputfilter->print( m_elemtype, m_elem))
			{
				switch (m_outputfilter->state())
				{
					case OutputFilter::Open:
						m_lasterror = "unknown error in transaction function output";
						break;
					case OutputFilter::EndOfBuffer:
						return Yield;
					case OutputFilter::Error:
						if (m_outputfilter->getError())
						{
							m_lasterror.append( m_outputfilter->getError());
						}
						break;
				}
				return Error;
			}
			m_state = 1;
			break;
		}
		default:
			m_lasterror = "illegal state";
			return Error;
	}
}

void TransactionFunctionResult::init( const TypedOutputFilterR& o)
{
	m_state = 1;
	m_resultreader.reset( m_func.resultreader()->copy());
	if (m_resultbuf.get())
	{
		m_resultreader->putInput( m_resultbuf->c_str(), m_resultbuf->size(), true);
	}
	else
	{
		m_resultreader->putInput( "", 0, true);
	}
	m_lasterror.clear();
	m_outputfilter = o;
}

void TransactionFunctionResult::appendCmdOutput( const void* buf, std::size_t size)
{
	if (!m_resultbuf.get()) m_resultbuf.reset( new std::string);
	m_resultbuf->append( (const char*)buf, size);
}

void TransactionFunctionResult::reset()
{
	m_resultbuf.reset();
	m_lasterror.clear();
}


TransactionFunctionClosure::TransactionFunctionClosure( const std::string& name_, const TransactionFunction& f)
	:m_func(f)
	,m_name(name_)
	,m_cmdop(cmdbind::CommandHandler::READ)
	,m_state(0)
	,m_cmdinputbuf(std::malloc( InputBufSize), std::free)
	,m_cmdoutputbuf(std::malloc( OutputBufSize), std::free)
	,m_result(f)
	{}

TransactionFunctionClosure::TransactionFunctionClosure( const TransactionFunctionClosure& o)
	:m_func(o.m_func)
	,m_name(o.m_name)
	,m_cmd(o.m_cmd)
	,m_cmdop(cmdbind::CommandHandler::READ)
	,m_state(o.m_state)
	,m_lasterror(o.m_lasterror)
	,m_cmdinputbuf(o.m_cmdinputbuf)
	,m_cmdoutputbuf(o.m_cmdoutputbuf)
	,m_cmdwriter(o.m_cmdwriter)
	,m_result(o.m_result)
	,m_inputfilter(o.m_inputfilter)
	{}

TransactionFunctionClosure::CallResult TransactionFunctionClosure::call()
{
	if (!m_inputfilter.get())
	{
		m_lasterror = "empty input for transaction function command";
		m_state = 0;
		return Error;
	}
	for (;;) switch (m_cmdop)
	{
		case cmdbind::CommandHandler::READ:
		{
			switch (m_state)
			{
				case 0:
					return Error;
				case 1:
					if (!m_inputfilter->getNext( m_elemtype, m_elem))
					{
						switch (m_inputfilter->state())
						{
							case InputFilter::Open:
								m_cmd->putInput( m_cmdinputbuf.get(), m_cmdwriter->getPosition());
								m_cmdwriter->setOutputBuffer( m_cmdinputbuf.get(), InputBufSize);
								m_cmd->nextOperation();
								m_state = 1;

							case InputFilter::EndOfMessage:
								m_cmd->putInput( m_cmdinputbuf.get(), m_cmdwriter->getPosition());
								m_cmdwriter->setOutputBuffer( m_cmdinputbuf.get(), InputBufSize);
								return Yield;

							case InputFilter::Error:
								if (m_inputfilter->getError())
								{
									m_lasterror.append( m_inputfilter->getError());
								}
								break;
						}
						return Error;
					}
					m_state = 2;
				case 2:
				{
					SerializeOutputFilter so( m_cmdwriter.get());
					if (!so.print( m_elemtype, m_elem))
					{
						switch (so.state())
						{
							case OutputFilter::Open:
								m_state = 1;
								continue;

							case OutputFilter::EndOfBuffer:
								m_cmd->putInput( m_cmdinputbuf.get(), m_cmdwriter->getPosition());
								m_cmdwriter->setOutputBuffer( m_cmdinputbuf.get(), InputBufSize);
								continue;

							case InputFilter::Error:
								if (so.getError())
								{
									m_lasterror.append( so.getError());
								}
								return Error;
						}
						m_lasterror = "illegal state";
						return Error;
					}
					m_state = 1;
					continue;
				}
				default:
					m_lasterror = "illegal state";
					return Error;
			}
		}
		case cmdbind::CommandHandler::WRITE:
		{
			const void* data;
			std::size_t datasize;
			m_cmd->getOutput( data, datasize);
			m_result.appendCmdOutput( data, datasize);
			m_cmd->nextOperation();
			m_state = 1;
		}
		case cmdbind::CommandHandler::CLOSED:
		{
			return Ok;
		}
	}
}

void TransactionFunctionClosure::init( const TypedInputFilterR& i)
{
	m_cmd = m_func.cmdconstructor()( m_name);
	m_cmdop = cmdbind::CommandHandler::READ;
	m_state = 1;
	m_lasterror.clear();
	m_cmdwriter.reset( m_func.cmdwriter()->copy());
	m_cmdwriter->setOutputBuffer( m_cmdinputbuf.get(), InputBufSize);
	m_result.reset();
	m_inputfilter = i;
	if (!m_cmd.get())
	{
		m_lasterror = "unknown transaction function command";
		m_state = 0;
	}
	else
	{
		m_cmd->setInputBuffer( m_cmdinputbuf.get(), InputBufSize);
		m_cmd->setOutputBuffer( m_cmdoutputbuf.get(), OutputBufSize, 0);
		m_state = 1;
	}
}

void TransactionFunctionMap::defineTransactionFunction( const std::string& name, const TransactionFunction& f)
{
	defineObject( m_map, name, f);
}

bool TransactionFunctionMap::getTransactionFunction( const std::string& name, TransactionFunction& rt) const
{
	return getObject( m_map, name, rt);
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

static InputFilterClosure::ItemType fetchFailureResult( const InputFilter& ff)
{
	const char* msg;
	switch (ff.state())
	{
		case InputFilter::EndOfMessage:
			return InputFilterClosure::DoYield;

		case InputFilter::Error:
			msg = ff.getError();
			LOG_ERROR << "error in input filter (" << (msg?msg:"unknown") << ")";
			return InputFilterClosure::Error;

		case InputFilter::Open:
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
		return fetchFailureResult( *m_inputfilter);
	}
	else
	{
		switch (m_type)
		{
			case InputFilter::OpenTag:
				m_taglevel += 1;
				tag = (const char*)element;
				tagsize = elementsize;
				val = 0;
				valsize = 0;
				m_gotattr = false;
				return Data;

			case InputFilter::Value:
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

			 case InputFilter::Attribute:
				m_attrbuf.clear();
				m_attrbuf.append( (const char*)element, elementsize);
				m_gotattr = true;
				goto AGAIN;

			 case InputFilter::CloseTag:
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
						if (!m_outputfilter->print( OutputFilter::Attribute, tag, tagsize)) break;
						m_state ++;
					case 1:
						if (!m_outputfilter->print( OutputFilter::Value, val, valsize)) break;
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
				return DoYield;
			}
			else
			{
				if (!m_outputfilter->print( OutputFilter::OpenTag, tag, tagsize))
				{
					const char* err = m_outputfilter->getError();
					if (err)
					{
						LOG_ERROR << "error in output filter open tag (" << err << ")";
						return Error;
					}
					return DoYield;
				}
				return Data;
			}
		}
		else if (val)
		{
			if (!m_outputfilter->print( OutputFilter::Value, val, valsize))
			{
				const char* err = m_outputfilter->getError();
				if (err)
				{
					LOG_ERROR << "error in output filter value (" << err << ")";
					return Error;
				}
				return DoYield;
			}
			return Data;
		}
		else
		{
			if (!m_outputfilter->print( OutputFilter::CloseTag, 0, 0))
			{
				const char* err = m_outputfilter->getError();
				if (err)
				{
					LOG_ERROR << "error in output filter close tag (" << err << ")";
					return Error;
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






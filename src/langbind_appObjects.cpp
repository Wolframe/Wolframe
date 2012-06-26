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
#include "serialize/ddl/filtermapDDLSerialize.hpp"
#include "ddl/compiler/simpleFormCompiler.hpp"
#include "filter/filter.hpp"
#include "filter/typingfilter.hpp"
#include "filter/tostringfilter.hpp"
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


RedirectFilterClosure::RedirectFilterClosure()
	:m_state(0)
	,m_taglevel(0)
	,m_elemtype(InputFilter::Value){}

RedirectFilterClosure::RedirectFilterClosure( const TypedInputFilterR& i, const TypedOutputFilterR& o)
	:m_state(0)
	,m_taglevel(0)
	,m_inputfilter(i)
	,m_outputfilter(o)
	,m_elemtype(InputFilter::Value)
	{}

RedirectFilterClosure::RedirectFilterClosure( const RedirectFilterClosure& o)
	:m_state(o.m_state)
	,m_taglevel(o.m_taglevel)
	,m_inputfilter(o.m_inputfilter)
	,m_outputfilter(o.m_outputfilter)
	,m_elemtype(o.m_elemtype)
	,m_elem(o.m_elem)
	{}

void RedirectFilterClosure::init( const TypedInputFilterR& i, const TypedOutputFilterR& o)
{
	m_inputfilter = i;
	m_outputfilter = o;
	m_state = 0;
	m_taglevel = 0;
	m_elemtype = InputFilter::Value;
}


bool RedirectFilterClosure::call()
{
	if (!m_inputfilter.get()) throw std::runtime_error( "no input defined for redirecting filter");
	if (!m_outputfilter.get()) throw std::runtime_error( "no output defined for redirecting filter");

	for (;;) switch (m_state)
	{
		case 0:
			if (!m_inputfilter->getNext( m_elemtype, m_elem))
			{
				switch (m_inputfilter->state())
				{
					case InputFilter::Open:
						m_state = 2;
						return true;

					case InputFilter::EndOfMessage:
						return false;

					case InputFilter::Error:
						throw std::runtime_error( m_inputfilter->getError());
				}
			}
			m_state = 1;
			if (m_elemtype == InputFilter::OpenTag)
			{
				++m_taglevel;
			}
			else if (m_elemtype == InputFilter::CloseTag)
			{
				--m_taglevel;
				if (m_taglevel < 0)
				{
					m_state = 2;
					return true;
				}
			}
			/*no break here!*/
		case 1:
			if (!m_outputfilter->print( m_elemtype, m_elem))
			{
				switch (m_outputfilter->state())
				{
					case OutputFilter::Open:
						throw std::runtime_error( "unknown error in output filter");

					case OutputFilter::EndOfBuffer:
						return false;

					case OutputFilter::Error:
						throw std::runtime_error( m_outputfilter->getError());
				}
			}
			m_state = 0;
			continue;
		default:
			return true;
	}
}

void DDLForm::clone()
{
	if (m_structure.get())
	{
		m_structure = ddl::StructTypeR( new ddl::StructType( *m_structure));
	}
}

std::string DDLForm::tostring() const
{
	ToStringFilter* flt = new ToStringFilter;
	TypedOutputFilterR out( flt);
	serialize::DDLStructSerializer ser( m_structure);
	ser.init( out, serialize::Context::SerializeWithIndices);
	if (!ser.call())
	{
		if (out->state() == OutputFilter::EndOfBuffer)
		{
			throw std::logic_error( "internal: tostring serialization with yield");
		}
		else
		{
			throw std::runtime_error( ser.getError());
		}
	}
	return flt->content();
}

void DDLFormMap::defineForm( const std::string& name, const DDLForm& f)
{
	DDLForm f_(f);
	f_.clone();
	defineObject( m_map, name, f_);
}

bool DDLFormMap::getForm( const std::string& name, DDLForm& rt) const
{
	if (!getObject( m_map, name, rt)) return false;
	rt.clone();
	return true;
}


ApiFormData::ApiFormData( const serialize::FiltermapDescriptionBase* descr_)
	:m_descr(descr_)
	,m_data(std::calloc( descr_->size(), 1), std::free)
{
	void* ptr = m_data.get();
	if (!ptr) throw std::bad_alloc();
	if (!m_descr->init( ptr))
	{
		m_descr->done( ptr);
		m_data.reset();
		throw std::bad_alloc();
	}
}

ApiFormData::~ApiFormData()
{
	void* ptr = m_data.get();
	if (ptr) m_descr->done( ptr);
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


FormFunctionClosure::FormFunctionClosure( const FormFunction& f)
	:m_func(f)
	,m_state(0)
	,m_param_data(f.api_param())
	,m_result_data(f.api_result())
	,m_result(m_result_data.data(),m_result_data.descr())
	,m_parser(m_param_data.data(),m_param_data.descr()){}

FormFunctionClosure::FormFunctionClosure( const FormFunctionClosure& o)
	:m_func(o.m_func)
	,m_state(0)
	,m_param_data(o.m_param_data)
	,m_result_data(o.m_result_data)
	,m_result(o.m_result)
	,m_parser(o.m_parser)
	{}

void FormFunctionClosure::init( const TypedInputFilterR& i, serialize::Context::Flags flags)
{
	m_parser.init(i,flags);
}

bool FormFunctionClosure::call()
{
	void* param_struct = m_param_data.get();
	void* result_struct = m_result_data.get();
	switch (m_state)
	{
		case 0:
			if (!m_parser.call()) return false;
			m_state = 1;
		case 1:
			int rt = m_func.call( result_struct, param_struct);
			if (rt != 0)
			{
				std::ostringstream msg;
				msg << "error in call of form function (return code " << rt << ")";
				throw std::runtime_error( msg.str());
			}
			m_state = 2;
	}
	return true;
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
	,m_elemtype(o.m_elemtype)
	,m_elem(o.m_elem)
	,m_resultbuf(o.m_resultbuf)
	,m_resultreader(o.m_resultreader)
	,m_outputfilter(o.m_outputfilter){}


bool TransactionFunctionResult::call()
{
	if (!m_outputfilter.get())
	{
		throw std::runtime_error( "no output filter specified for transaction function result");
	}
	for (;;) switch (m_state)
	{
		case 0: throw std::runtime_error( "transaction function result not initialized");
		case 1:
		{
			TypingInputFilter si( m_resultreader);
			if (!si.getNext( m_elemtype, m_elem))
			{
				switch (si.state())
				{
					case InputFilter::Open:
						return true;

					case InputFilter::EndOfMessage:
						throw std::runtime_error( "unexpected end of message in result reader");

					case InputFilter::Error:
						throw std::runtime_error( si.getError());
				}
				throw std::runtime_error( "in transaction function illegal input filter state");
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
						throw std::runtime_error( "unknown error in transaction function output");

					case OutputFilter::EndOfBuffer:
						return false;

					case OutputFilter::Error:
						throw std::runtime_error( m_outputfilter->getError());
				}
				throw std::runtime_error( "in transaction function illegal output filter state");
			}
			m_state = 1;
			break;
		}
		default:
			throw std::runtime_error( "in transaction function illegal state");
	}
}

void TransactionFunctionResult::init( const TypedOutputFilterR& o)
{
	m_state = 1;
	m_elemtype = InputFilter::Value;

	m_resultreader.reset( m_func.resultreader()->copy());
	if (m_resultbuf.get())
	{
		m_resultreader->putInput( m_resultbuf->c_str(), m_resultbuf->size(), true);
	}
	else
	{
		m_resultreader->putInput( "", 0, true);
	}
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
}


TransactionFunctionClosure::TransactionFunctionClosure( const std::string& name_, const TransactionFunction& f)
	:m_func(f)
	,m_name(name_)
	,m_cmdop(cmdbind::CommandHandler::READ)
	,m_state(0)
	,m_elemtype(InputFilter::Value)
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
	,m_elemtype(o.m_elemtype)
	,m_elem(o.m_elem)
	,m_cmdinputbuf(o.m_cmdinputbuf)
	,m_cmdoutputbuf(o.m_cmdoutputbuf)
	,m_cmdwriter(o.m_cmdwriter)
	,m_result(o.m_result)
	,m_inputfilter(o.m_inputfilter)
	{}


bool TransactionFunctionClosure::call()
{
	if (!m_inputfilter.get())
	{
		throw std::runtime_error( "empty input for transaction function command");
	}
	for (;;) switch (m_cmdop)
	{
		case cmdbind::CommandHandler::READ:
		{
			switch (m_state)
			{
				case 0:
					throw std::runtime_error( "invalid transaction object called");
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
								continue;

							case InputFilter::EndOfMessage:
								m_cmd->putInput( m_cmdinputbuf.get(), m_cmdwriter->getPosition());
								m_cmdwriter->setOutputBuffer( m_cmdinputbuf.get(), InputBufSize);
								return false;

							case InputFilter::Error:
								throw std::runtime_error( m_inputfilter->getError());
						}
						throw std::runtime_error( "in transaction function illegal input filter state");
					}
					m_state = 2;
				case 2:
				{
					TypingOutputFilter so( m_cmdwriter);
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
								throw std::runtime_error( so.getError());
						}
						throw std::runtime_error( "in transaction function illegal output filter state");
					}
					m_state = 1;
					continue;
				}
				default:
					throw std::runtime_error( "illegal state");
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
			continue;
		}
		case cmdbind::CommandHandler::CLOSED:
		{
			return true;
		}
	}
}

void TransactionFunctionClosure::init( const TypedInputFilterR& i)
{
	m_cmd = m_func.cmdconstructor()( m_name);
	m_cmdop = cmdbind::CommandHandler::READ;
	m_state = 1;
	m_cmdwriter.reset( m_func.cmdwriter()->copy());
	m_cmdwriter->setOutputBuffer( m_cmdinputbuf.get(), InputBufSize);
	m_result.reset();
	m_inputfilter = i;
	if (!m_cmd.get())
	{
		m_state = 0;
		throw std::runtime_error( "unknown transaction function command");
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


bool Output::print( const char* tag, unsigned int tagsize, const char* val, unsigned int valsize)
{
	if (!m_outputfilter.get())
	{
		throw std::runtime_error( "no output sink defined (output ignored)");
	}
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
					return true;
			}
			const char* err = m_outputfilter->getError();
			if (err) throw std::runtime_error( err);
			return false;
		}
		else
		{
			if (!m_outputfilter->print( OutputFilter::OpenTag, tag, tagsize))
			{
				const char* err = m_outputfilter->getError();
				if (err) throw std::runtime_error( err);
				return false;
			}
			return true;
		}
	}
	else if (val)
	{
		if (!m_outputfilter->print( OutputFilter::Value, val, valsize))
		{
			const char* err = m_outputfilter->getError();
			if (err) throw std::runtime_error( err);
			return false;
		}
		return true;
	}
	else
	{
		if (!m_outputfilter->print( OutputFilter::CloseTag, 0, 0))
		{
			const char* err = m_outputfilter->getError();
			if (err) throw std::runtime_error( err);
			return false;
		}
		return true;
	}
}






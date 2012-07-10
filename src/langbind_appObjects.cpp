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
///\brief Implementation of scripting language objects
#include "langbind/appObjects.hpp"
#include "serialize/ddl/filtermapDDLParse.hpp"
#include "serialize/ddl/filtermapDDLSerialize.hpp"
#include "ddl/compiler/simpleFormCompiler.hpp"
#include "filter/filter.hpp"
#include "miscUtils.hpp"
#include "filter/typingfilter.hpp"
#include "filter/tostringfilter.hpp"
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

InputFilterR& Input::getIterator()
{
	if (m_used) throw std::runtime_error( "try to read input twice");
	m_used = true;
	return m_inputfilter;
}


void FilterMap::defineFilter( const std::string& name, CreateFilterFunc f)
{
	defineObject( m_map, name, f);
}

bool FilterMap::getFilter( const std::string& name, Filter& rt)
{
	CreateFilterFunc f;
	if (!getObject( m_map, name, f)) return false;
	rt = f( name);
	return true;
}

FilterMap::FilterMap()
{
	defineFilter( "char", createCharFilter);
	defineFilter( "char:UTF-8", createCharFilter);
	defineFilter( "char:UTF-16BE", createCharFilter);
	defineFilter( "char:UTF-16LE", createCharFilter);
	defineFilter( "char:UCS-2LE", createCharFilter);
	defineFilter( "char:UCS-2BE", createCharFilter);
	defineFilter( "char:UCS-4LE", createCharFilter);
	defineFilter( "char:UCS-4BE", createCharFilter);
	defineFilter( "line", createLineFilter);
	defineFilter( "line:UTF-8", createLineFilter);
	defineFilter( "line:UTF-16BE", createLineFilter);
	defineFilter( "line:UTF-16LE", createLineFilter);
	defineFilter( "line:UCS-2LE", createLineFilter);
	defineFilter( "line:UCS-2BE", createLineFilter);
	defineFilter( "line:UCS-4LE", createLineFilter);
	defineFilter( "line:UCS-4BE", createLineFilter);
	defineFilter( "token", createTokenFilter);
	defineFilter( "token:UTF-8", createTokenFilter);
	defineFilter( "token:UTF-16BE", createTokenFilter);
	defineFilter( "token:UTF-16LE", createTokenFilter);
	defineFilter( "token:UCS-2LE", createTokenFilter);
	defineFilter( "token:UCS-2BE", createTokenFilter);
	defineFilter( "token:UCS-4LE", createTokenFilter);
	defineFilter( "token:UCS-4BE", createTokenFilter);
	defineFilter( "xml:textwolf", createTextwolfXmlFilter);
	defineFilter( "xml:textwolf:UTF-8", createTextwolfXmlFilter);
	defineFilter( "xml:textwolf:UTF-16BE", createTextwolfXmlFilter);
	defineFilter( "xml:textwolf:UTF-16LE", createTextwolfXmlFilter);
	defineFilter( "xml:textwolf:UCS-2LE", createTextwolfXmlFilter);
	defineFilter( "xml:textwolf:UCS-2BE", createTextwolfXmlFilter);
	defineFilter( "xml:textwolf:UCS-4LE", createTextwolfXmlFilter);
	defineFilter( "xml:textwolf:UCS-4BE", createTextwolfXmlFilter);
#ifdef WITH_LIBXML2
	defineFilter( "xml", createLibxml2Filter);
	defineFilter( "xml:libxml2", createLibxml2Filter);
	defineFilter( "xml:libxml2:UTF-8", createLibxml2Filter);
	defineFilter( "xml:libxml2:UTF-16BE", createLibxml2Filter);
	defineFilter( "xml:libxml2:UTF-16LE", createLibxml2Filter);
	defineFilter( "xml:libxml2:UCS-2LE", createLibxml2Filter);
	defineFilter( "xml:libxml2:UCS-2BE", createLibxml2Filter);
	defineFilter( "xml:libxml2:UCS-4LE", createLibxml2Filter);
	defineFilter( "xml:libxml2:UCS-4BE", createLibxml2Filter);
#else
	defineFilter( "xml", createTextwolfXmlFilter);
#endif
#ifdef WITH_XMLLITE
	defineFilter( "xml:xmllite", createXmlLiteFilter);
#endif
#ifdef WITH_MSXML
	defineFilter( "xml:msxml", createMSXMLFilter);
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

DDLForm DDLForm::copy() const
{
	DDLForm rt;
	if (m_structure.get())
	{
		rt.m_structure = ddl::StructTypeR( new ddl::StructType( *m_structure));
	}
	return rt;
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
	if (m_structure->doctype())
	{
		std::string rt = "!DOCTYPE \"";
		rt.append( m_structure->doctype());
		rt.append( "\" ");
		rt.append( flt->content());
		return rt;
	}
	return flt->content();
}

std::string DDLForm::getIdFromDoctype( const std::string& doctype)
{
	std::string rootid;
	std::string publicid;
	std::string systemid;
	std::string::const_iterator itr=doctype.begin(), end=doctype.end();

	if (utils::parseNextToken( rootid, itr, end))
	{
		if (utils::parseNextToken( publicid, itr, end))
		{
			if (publicid == "PUBLIC")
			{
				if (!utils::parseNextToken( publicid, itr, end)
				||  !utils::parseNextToken( systemid, itr, end))
				{
					throw std::runtime_error( "illegal doctype definition");
				}
				return utils::getFileStem( systemid);
			}
			else if (publicid == "SYSTEM")
			{
				if (!utils::parseNextToken( systemid, itr, end))
				{
					throw std::runtime_error( "illegal doctype definition");
				}
				return utils::getFileStem( systemid);
			}
			else if (utils::parseNextToken( systemid, itr, end))
			{
				return utils::getFileStem( systemid);
			}
			else
			{
				return utils::getFileStem( publicid);
			}
		}
		else
		{
			return rootid;
		}
	}
	else
	{
		return "";
	}
}

void DDLFormMap::defineForm( const std::string& name, const DDLForm& f)
{
	defineObject( m_map, name, f.copy());
}

bool DDLFormMap::getForm( const std::string& name, DDLForm& rt) const
{
	if (!getObject( m_map, name, rt)) return false;
	rt = rt.copy();
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

PeerFunction::PeerFunction( const PeerFunction& o)
	:m_cmdwriter(o.m_cmdwriter)
	,m_resultreader(o.m_resultreader)
	,m_cmdconstructor(o.m_cmdconstructor)
{}

PeerFunction::PeerFunction( const OutputFilterR& w, const InputFilterR& r, CreateCommandHandler c)
	:m_cmdwriter(w)
	,m_resultreader(r)
	,m_cmdconstructor(c)
{}

PeerFormFunction::PeerFormFunction( const PeerFormFunction& o)
	:PeerFunction(o)
	,m_inputform(o.m_inputform)
	,m_outputform(o.m_outputform)
{}

PeerFormFunction::PeerFormFunction( const PeerFunction& f, const DDLForm& i, const DDLForm& o)
	:PeerFunction(f)
	,m_inputform(i)
	,m_outputform(o)
{}


PeerFormFunctionClosure::PeerFormFunctionClosure( const PeerFormFunction& f)
	:m_func(f)
	,m_cmd(f.cmdconstructor()())
	,m_cmdop(cmdbind::CommandHandler::READ)
	,m_state(0)
	,m_cmdinputbuf(std::malloc( InputBufSize), std::free)
	,m_cmdoutputbuf(std::malloc( OutputBufSize), std::free)
{}

void PeerFormFunctionClosure::init( const TypedInputFilterR& i)
{
	m_inputfilter = i;
	m_param = serialize::DDLStructParser( m_func.inputform().copy().structure());
	m_result = serialize::DDLStructParser( m_func.outputform().copy().structure());
	m_cmdserialize = serialize::DDLStructSerializer( m_param.structure());

	if (!m_func.cmdwriter().get()) throw std::runtime_error( "called transaction function without command writer filter");
	if (!m_func.resultreader().get()) throw std::runtime_error( "called transaction function without result reader filter");
	if (!m_inputfilter.get()) throw std::runtime_error( "null input for transaction function");
	if (!m_cmdinputbuf.get()) throw std::bad_alloc();
	if (!m_cmdoutputbuf.get()) throw std::bad_alloc();

	m_cmdop = cmdbind::CommandHandler::READ;
	m_state = 0;
	m_cmdwriter.reset( m_func.cmdwriter()->copy());
	m_resultreader.reset( m_func.resultreader()->copy());
	m_param.init( m_inputfilter);
	m_result.init( TypedInputFilterR( new TypingInputFilter( m_resultreader)));
	m_cmdserialize.init( TypedOutputFilterR( new TypingOutputFilter( m_cmdwriter)));
	m_cmdwriter->setOutputBuffer( m_cmdinputbuf.get(), InputBufSize);
	m_cmd->setInputBuffer( m_cmdinputbuf.get(), InputBufSize);
	m_cmd->setOutputBuffer( m_cmdoutputbuf.get(), OutputBufSize, 0);
}


PeerFormFunctionClosure::PeerFormFunctionClosure( const PeerFormFunctionClosure& o)
	:m_func(o.m_func)
	,m_cmd(o.m_cmd)
	,m_cmdop(o.m_cmdop)
	,m_state(o.m_state)
	,m_cmdinputbuf(o.m_cmdinputbuf)
	,m_cmdoutputbuf(o.m_cmdoutputbuf)
	,m_cmdwriter(o.m_cmdwriter)
	,m_resultreader(o.m_resultreader)
	,m_param(o.m_param)
	,m_result(o.m_result)
	,m_cmdserialize(o.m_cmdserialize)
	,m_inputfilter(o.m_inputfilter)
	{}

DDLForm PeerFormFunctionClosure::result() const
{
	return DDLForm( m_result.structure());
}

bool PeerFormFunctionClosure::call()
{
	bool res;
	const void* data;
	std::size_t datasize;

	if (!m_inputfilter.get())
	{
		throw std::runtime_error( "null input for transaction function command");
	}
	// read input into parameter form 'm_param'
	if (m_state == 0 && !m_param.call()) return false;
	m_state = 1;

	for (;;) switch (m_cmdop)
	{
		case cmdbind::CommandHandler::READ:
			res = m_cmdserialize.call();
			m_cmd->putInput( m_cmdinputbuf.get(), m_cmdwriter->getPosition());
			m_cmdwriter->setOutputBuffer( m_cmdinputbuf.get(), InputBufSize);
			m_cmdop = m_cmd->nextOperation();
			if (res && m_cmdop == cmdbind::CommandHandler::READ)
			{
				throw std::runtime_error( "transaction function not terminated");
			}
			continue;

		case cmdbind::CommandHandler::WRITE:
			m_cmd->getOutput( data, datasize);
			m_resultreader->putInput( data, datasize, false);
			if (m_result.call())
			{
				throw std::runtime_error( "illegal state in result reader. end reading without getting end of data marker");
			}
			m_cmdop = m_cmd->nextOperation();
			continue;

		case cmdbind::CommandHandler::CLOSE:
			m_resultreader->putInput( "", 0, true);
			if (!m_result.call())
			{
				throw std::runtime_error( "transaction function result corrupt");
			}
			return true;
		default:
			throw std::runtime_error( "illegal state in transaction function in command handler");
	}
}


void PeerFunctionMap::definePeerFunction( const std::string& name, const PeerFunction& f)
{
	defineObject( m_map, name, f);
}

bool PeerFunctionMap::getPeerFunction( const std::string& name, PeerFunction& rt) const
{
	return getObject( m_map, name, rt);
}

void PeerFormFunctionMap::definePeerFormFunction( const std::string& name, const PeerFormFunction& f)
{
	defineObject( m_map, name, f);
}

bool PeerFormFunctionMap::getPeerFormFunction( const std::string& name, PeerFormFunction& rt) const
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






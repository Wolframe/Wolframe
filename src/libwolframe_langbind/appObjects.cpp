/************************************************************************
Copyright (C) 2011 - 2014 Project Wolframe.
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
#include "serialize/ddl/ddlStructParser.hpp"
#include "serialize/ddl/ddlStructSerializer.hpp"
#include "filter/filter.hpp"
#include "filter/typingfilter.hpp"
#include "filter/tostringfilter.hpp"
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace _Wolframe;
using namespace langbind;

Input::Input( const Input& o)
	:utils::TypeSignature(o)
	,m_used(o.m_used)
	,m_inputfilter(o.m_inputfilter)
	,m_docformat(o.m_docformat)
	,m_content(o.m_content)
	,m_contentsize(o.m_contentsize)
	,m_isProcessorInput(o.m_isProcessorInput)
{}

Input::Input( const InputFilterR& inputfilter_, const std::string& docformat_)
	:utils::TypeSignature("langbind::Input", __LINE__)
	,m_used(false)
	,m_inputfilter(inputfilter_)
	,m_docformat(docformat_)
	,m_contentsize(0)
	,m_isProcessorInput(true)
{}

Input::Input( const std::string& docformat_, const std::string& content_)
	:utils::TypeSignature("langbind::Input", __LINE__)
	,m_used(false)
	,m_docformat(docformat_)
	,m_contentsize(0)
	,m_isProcessorInput(false)
{
	char* mem = (char*)std::malloc( content_.size());
	if (!mem) throw std::bad_alloc();
	m_content = boost::shared_ptr<char>( mem, std::free);
	m_contentsize = content_.size();
	std::memcpy( mem, content_.c_str(), content_.size());
}

const void* Input::allocContentCopy( const void* ptr, std::size_t size)
{
	char* mem = (char*)std::malloc( size);
	if (!mem) throw std::bad_alloc();
	m_content = boost::shared_ptr<char>( mem, std::free);
	m_contentsize = size;
	std::memcpy( mem, ptr, size);
	return m_content.get();
}

InputFilterR& Input::getIterator()
{
	if (m_used) throw std::runtime_error( "try to use iterator for input/document twice");
	m_used = true;
	return m_inputfilter;
}

RedirectFilterClosure::RedirectFilterClosure()
	:utils::TypeSignature("langbind::RedirectFilterClosure", __LINE__)
	,m_state(0)
	,m_taglevel(0)
	,m_elemtype(InputFilter::Value){}

RedirectFilterClosure::RedirectFilterClosure( const TypedInputFilterR& i, const TypedOutputFilterR& o)
	:utils::TypeSignature("langbind::RedirectFilterClosure", __LINE__)
	,m_state(0)
	,m_taglevel(0)
	,m_inputfilter(i)
	,m_outputfilter(o)
	,m_elemtype(InputFilter::Value)
	{
		m_inputfilter->setFlags( m_outputfilter->flags());
	}

RedirectFilterClosure::RedirectFilterClosure( const RedirectFilterClosure& o)
	:utils::TypeSignature(o)
	,m_state(o.m_state)
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
	m_inputfilter->setFlags( m_outputfilter->flags());
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


ApiFormData::ApiFormData( const serialize::StructDescriptionBase* descr_)
	:utils::TypeSignature("langbind::ApiFormData", __LINE__)
	,m_descr(descr_)
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


CppFormFunctionClosure::CppFormFunctionClosure( const CppFormFunction& f)
	:utils::TypeSignature("langbind::CppFormFunctionClosure", __LINE__)
	,m_func(f)
	,m_state(0)
	,m_param_data(f.api_param())
	,m_result_data(f.api_result())
	,m_result(m_result_data.data(),m_result_data.descr())
	,m_parser(m_param_data.data(),m_param_data.descr())
	,m_provider(0){}

CppFormFunctionClosure::CppFormFunctionClosure( const CppFormFunctionClosure& o)
	:utils::TypeSignature(o)
	,m_func(o.m_func)
	,m_state(0)
	,m_param_data(o.m_param_data)
	,m_result_data(o.m_result_data)
	,m_result(o.m_result)
	,m_parser(o.m_parser)
	,m_provider(o.m_provider)
	{}

void CppFormFunctionClosure::init( const proc::ProcessorProviderInterface* provider, const TypedInputFilterR& i, serialize::Context::Flags flags)
{
	m_provider = provider;
	m_parser.init(i,flags);
}

bool CppFormFunctionClosure::call()
{
	void* param_struct = m_param_data.get();
	void* result_struct = m_result_data.get();
	switch (m_state)
	{
		case 0:
			if (!m_parser.call()) return false;
			m_state = 1;
		case 1:
			int rt = m_func.call( m_provider, result_struct, param_struct);
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






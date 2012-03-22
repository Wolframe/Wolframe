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
///\file appObjects.cpp
///\brief implementation of scripting language objects

#include "langbind/appObjects.hpp"
#include "logger-v1.hpp"
#include "protocol/inputfilter.hpp"
#include "protocol/outputfilter.hpp"
#include <boost/algorithm/string.hpp>
#include "filter/char_filter.hpp"
#include "filter/line_filter.hpp"
#include "filter/token_filter.hpp"
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
	if (!m_inputfilter.get() || m_bufsize==0)
	{
		return EndOfData;
	}
	if (m_value)
	{
		if (!m_inputfilter->getNext( &m_type, m_buf, m_bufsize-1, &m_bufpos))
		{
			if (m_inputfilter->state() == protocol::InputFilter::Open)
			{
				// at end of data check if there is a follow filter (transformed filter) to continue with:
				protocol::InputFilter* follow = m_inputfilter->createFollow();
				if (follow)
				{
					m_inputfilter.reset( follow);
					return fetch( tag, tagsize, val, valsize);
				}
			}
			return fetchFailureResult( *m_inputfilter); 
		}
		else if (m_type == protocol::InputFilter::Value)
		{
			tag = m_buf;
			tagsize = m_value-m_buf;
			val = m_value;
			valsize = m_bufpos-tagsize;
			init();
			return Data;
		}
		else
		{
			LOG_DATA << "error in XML: attribute value expected";
			init();
			return Error;
		}
	}
	if (!m_inputfilter->getNext( &m_type, m_buf, m_bufsize-1, &m_bufpos))
	{
		if (m_inputfilter->state() == protocol::InputFilter::Open)
		{
			// at end of data check if there is a follow filter (transformed filter) to continue with:
			protocol::InputFilter* follow = m_inputfilter->createFollow();
			if (follow)
			{
				m_inputfilter.reset( follow);
				return fetch( tag, tagsize, val, valsize);
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
				tag = m_buf;
				tagsize = m_bufpos;
				val = 0;
				valsize = 0;
				init();
				return Data;

			case protocol::InputFilter::Value:
				tag = 0;
				tagsize = 0;
				val = m_buf;
				valsize = m_bufpos;
				init();
				return Data;

			 case protocol::InputFilter::Attribute:
				m_value = m_buf+m_bufpos;
				return fetch( tag, tagsize, val, valsize);

			 case protocol::InputFilter::CloseTag:
				tag = 0;
				tagsize = 0;
				val = 0;
				valsize = 0;
				init();
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

static bool startsWith( const char* str, const char* prefix)
{
	std::size_t str_len = std::strlen( str);
	std::size_t prefix_len = std::strlen( prefix);
	if (str_len < prefix_len) return false;
	const std::string nm( str, prefix_len);
	return boost::algorithm::iequals( nm, prefix);
}

static bool equalIdent( const char* str, const char* prefix)
{
	return boost::algorithm::iequals( str, prefix);
}

Filter::Filter( const char* name)
{
	enum {DEFAULT_LINE_SIZE=2048,DEFAULT_INPUT_SIZE=4096,DEFAULT_OUTPUT_SIZE=4096};
	if (startsWith( name, "char:"))
	{
		filter::CharFilter flt( name+5);
		m_inputfilter = flt.inputFilter();
		m_outputfilter = flt.outputFilter();
	}
	else if (startsWith( name, "line:"))
	{
		filter::LineFilter flt( name+5, DEFAULT_LINE_SIZE);
		m_inputfilter = flt.inputFilter();
		m_outputfilter = flt.outputFilter();
	}
	else if (startsWith( name, "token:"))
	{
		filter::TokenFilter flt( name+6, DEFAULT_LINE_SIZE);
		m_inputfilter = flt.inputFilter();
		m_outputfilter = flt.outputFilter();
	}
	else if (equalIdent( name, "xml:textwolf"))
	{
		filter::TextwolfXmlFilter tw( DEFAULT_INPUT_SIZE, DEFAULT_OUTPUT_SIZE);
		m_inputfilter = tw.inputFilter();
		m_outputfilter = tw.outputFilter();
	}
	else if (startsWith( name, "xml:textwolf:"))
	{
		filter::TextwolfXmlFilter tw( DEFAULT_INPUT_SIZE, DEFAULT_OUTPUT_SIZE, name+strlen("xml:textwolf:"));
		m_inputfilter = tw.inputFilter();
		m_outputfilter = tw.outputFilter();
	}
#ifdef WITH_LIBXML2
	else if (equalIdent( name, "xml:libxml2"))
	{
		filter::Libxml2Filter tw( DEFAULT_INPUT_SIZE);
		m_inputfilter = tw.inputFilter();
		m_outputfilter = tw.outputFilter();
	}
#endif
#ifdef WITH_XMLLITE
	else if (equalIdent( name, "xml:xmllite"))
	{
		filter::XmlLiteFilter tw( DEFAULT_INPUT_SIZE);
		m_inputfilter = tw.inputFilter();
		m_outputfilter = tw.outputFilter();
	}
#endif
#ifdef WITH_MSXML
	else if (equalIdent( name, "xml:msxml"))
	{
		filter::MSXMLFilter tw( DEFAULT_INPUT_SIZE);
		m_inputfilter = tw.inputFilter();
		m_outputfilter = tw.outputFilter();
	}
#endif
}

bool Form::getValue( const char* name, std::string& val) const
{
	const ddl::StructType* st = m_struct.get();
	if (!st) return false;
	if (st->contentType() != ddl::StructType::Struct) return false;
	ddl::StructType::Map::const_iterator aa=st->find( name);
	if (aa == st->end() || aa->second.contentType() != ddl::StructType::Atomic) return false;
	if (!aa->second.value().get( val)) return false;
	return true;
}

bool Form::setValue( const char* name, const std::string& val)
{
	ddl::StructType* st = m_struct.get();
	if (!st) return false;
	if (st->contentType() != ddl::StructType::Struct) return false;
	ddl::StructType::Map::iterator aa=st->find( name);
	if (aa == st->end() || aa->second.contentType() != ddl::StructType::Atomic) return false;
	if (!aa->second.value().set( val)) return false;
	return true;
}

bool Form::getValue( const std::size_t idx, std::string& val) const
{
	const ddl::StructType* st = m_struct.get();
	if (!st) return false;
	if (st->contentType() != ddl::StructType::Vector || idx >st->size()) return false;
	ddl::StructType::Map::const_iterator aa = st->begin() + idx;
	if (!(aa < st->end()) || aa->second.contentType() != ddl::StructType::Atomic) return false;
	if (!aa->second.value().get( val)) return false;
	return true;
}

bool Form::setValue( const std::size_t idx, const std::string& val)
{
	ddl::StructType* st = m_struct.get();
	if (!st) return false;
	if (st->contentType() != ddl::StructType::Vector || idx >st->size()) return false;
	ddl::StructType::Map::iterator aa = st->begin() + idx;
	if (!(aa < st->end()) || aa->second.contentType() != ddl::StructType::Atomic) return false;
	if (!aa->second.value().set( val)) return false;
	return true;
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






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
///
/// \file appObjects.cpp
/// \brief interface implementation for application processor scripting language to system objects
///

#include "appObjects.hpp"
#include "logger.hpp"
#include "protocol/inputfilter.hpp"
#include "protocol/formatoutput.hpp"
#include <boost/algorithm/string.hpp>
#include "filters.hpp"

using namespace _Wolframe;
using namespace app;

static InputFilterClosure::ItemType fetchFailureResult( const protocol::InputFilter& ff)
{
	switch (ff.state())
	{
		case protocol::InputFilter::EndOfMessage:
			return InputFilterClosure::DoYield;

		case protocol::InputFilter::Error:
			LOG_ERROR << "error in iterator (" << ff.getError() << ")";
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

static std::size_t getName( char* buf, std::size_t bufsize, const char* name)
{
	std::size_t ii,kk;
	if (!bufsize) return 0;
	bufsize -= 1;
	for (kk=0,ii=0; ii<bufsize && name[kk]; kk++)
	{
		if (name[kk] == '-') continue;
		if (name[kk] == ' ') continue;
		buf[ii++] = name[kk];
	}
	buf[ii] = 0;
	return ii;
}

protocol::InputFilter* System::createDefaultInputFilter() const
{
	return new filter::CharFilter<textwolf::charset::IsoLatin1>::InputFilter();
}

static bool equalIdent( const char* a1, const char* a2)
{
	return boost::algorithm::iequals( a1, a2);
}

protocol::InputFilter* System::createInputFilter( const char* name, unsigned int bufsize) const
{
	protocol::InputFilter* rt = 0;
	char nm[32];
	if (!getName( nm, sizeof(nm), name))
	{
		LOG_ERROR << "unknown filter '" << name << "'";
		return 0;
	}
	if (equalIdent( nm, "char:isolatin1")) rt = (protocol::InputFilter*) new filter::CharFilter<textwolf::charset::IsoLatin1>::InputFilter();
	else if (equalIdent( nm, "char:iso88591")) rt = (protocol::InputFilter*) new filter::CharFilter<textwolf::charset::IsoLatin1>::InputFilter();
	else if (equalIdent( nm, "char:UTF8")) rt = (protocol::InputFilter*) new filter::CharFilter<textwolf::charset::UTF8>::InputFilter();
	else if (equalIdent( nm, "char:UTF16")) rt = (protocol::InputFilter*) new filter::CharFilter<textwolf::charset::UTF16BE>::InputFilter();
	else if (equalIdent( nm, "char:UTF16LE")) rt = (protocol::InputFilter*) new filter::CharFilter<textwolf::charset::UTF16LE>::InputFilter();
	else if (equalIdent( nm, "char:UTF16BE")) rt = (protocol::InputFilter*) new filter::CharFilter<textwolf::charset::UTF16BE>::InputFilter();
	else if (equalIdent( nm, "char:UCS2LE")) rt = (protocol::InputFilter*) new filter::CharFilter<textwolf::charset::UCS2LE>::InputFilter();
	else if (equalIdent( nm, "char:UCS2BE")) rt = (protocol::InputFilter*) new filter::CharFilter<textwolf::charset::UCS2BE>::InputFilter();
	else if (equalIdent( nm, "char:UCS2")) rt = (protocol::InputFilter*) new filter::CharFilter<textwolf::charset::UCS2BE>::InputFilter();
	else if (equalIdent( nm, "char:UCS4LE")) rt = (protocol::InputFilter*) new filter::CharFilter<textwolf::charset::UCS4LE>::InputFilter();
	else if (equalIdent( nm, "char:UCS4BE")) rt = (protocol::InputFilter*) new filter::CharFilter<textwolf::charset::UCS4BE>::InputFilter();
	else if (equalIdent( nm, "char:UCS4")) rt = (protocol::InputFilter*) new filter::CharFilter<textwolf::charset::UCS4BE>::InputFilter();

	else if (equalIdent( nm, "line:isolatin1")) rt = (protocol::InputFilter*) new filter::LineFilter<textwolf::charset::IsoLatin1>::InputFilter();
	else if (equalIdent( nm, "line:iso88591")) rt = (protocol::InputFilter*) new filter::LineFilter<textwolf::charset::IsoLatin1>::InputFilter();
	else if (equalIdent( nm, "line:UTF8")) rt = (protocol::InputFilter*) new filter::LineFilter<textwolf::charset::UTF8>::InputFilter( bufsize);
	else if (equalIdent( nm, "line:UTF16")) rt = (protocol::InputFilter*) new filter::LineFilter<textwolf::charset::UTF16BE>::InputFilter( bufsize);
	else if (equalIdent( nm, "line:UTF16LE")) rt = (protocol::InputFilter*) new filter::LineFilter<textwolf::charset::UTF16LE>::InputFilter( bufsize);
	else if (equalIdent( nm, "line:UTF16BE")) rt = (protocol::InputFilter*) new filter::LineFilter<textwolf::charset::UTF16BE>::InputFilter( bufsize);
	else if (equalIdent( nm, "line:UCS2LE")) rt = (protocol::InputFilter*) new filter::LineFilter<textwolf::charset::UCS2LE>::InputFilter( bufsize);
	else if (equalIdent( nm, "line:UCS2BE")) rt = (protocol::InputFilter*) new filter::LineFilter<textwolf::charset::UCS2BE>::InputFilter( bufsize);
	else if (equalIdent( nm, "line:UCS2")) rt = (protocol::InputFilter*) new filter::LineFilter<textwolf::charset::UCS2BE>::InputFilter( bufsize);
	else if (equalIdent( nm, "line:UCS4LE")) rt = (protocol::InputFilter*) new filter::LineFilter<textwolf::charset::UCS4LE>::InputFilter( bufsize);
	else if (equalIdent( nm, "line:UCS4BE")) rt = (protocol::InputFilter*) new filter::LineFilter<textwolf::charset::UCS4BE>::InputFilter( bufsize);
	else if (equalIdent( nm, "line:UCS4")) rt = (protocol::InputFilter*) new filter::LineFilter<textwolf::charset::UCS4BE>::InputFilter( bufsize);

	else if (equalIdent( nm, "xml:isolatin1")) rt = (protocol::InputFilter*) new filter::XmlFilter<textwolf::charset::IsoLatin1>::InputFilter( bufsize);
	else if (equalIdent( nm, "xml:iso88591")) rt = (protocol::InputFilter*) new filter::XmlFilter<textwolf::charset::IsoLatin1>::InputFilter( bufsize);
	else if (equalIdent( nm, "xml:UTF8"))	rt = (protocol::InputFilter*) new filter::XmlFilter<textwolf::charset::UTF8>::InputFilter( bufsize);
	else if (equalIdent( nm, "xml:UTF16")) rt = (protocol::InputFilter*) new filter::XmlFilter<textwolf::charset::UTF16BE>::InputFilter( bufsize);
	else if (equalIdent( nm, "xml:UTF16LE")) rt = (protocol::InputFilter*) new filter::XmlFilter<textwolf::charset::UTF16LE>::InputFilter( bufsize);
	else if (equalIdent( nm, "xml:UTF16BE")) rt = (protocol::InputFilter*) new filter::XmlFilter<textwolf::charset::UTF16BE>::InputFilter( bufsize);
	else if (equalIdent( nm, "xml:UCS2LE")) rt = (protocol::InputFilter*) new filter::XmlFilter<textwolf::charset::UCS2LE>::InputFilter( bufsize);
	else if (equalIdent( nm, "xml:UCS2BE")) rt = (protocol::InputFilter*) new filter::XmlFilter<textwolf::charset::UCS2BE>::InputFilter( bufsize);
	else if (equalIdent( nm, "xml:UCS2")) rt = (protocol::InputFilter*) new filter::XmlFilter<textwolf::charset::UCS2BE>::InputFilter( bufsize);
	else if (equalIdent( nm, "xml:UCS4LE")) rt = (protocol::InputFilter*) new filter::XmlFilter<textwolf::charset::UCS4LE>::InputFilter( bufsize);
	else if (equalIdent( nm, "xml:UCS4BE")) rt = (protocol::InputFilter*) new filter::XmlFilter<textwolf::charset::UCS4BE>::InputFilter( bufsize);
	else if (equalIdent( nm, "xml:UCS4")) rt = (protocol::InputFilter*) new filter::XmlFilter<textwolf::charset::UCS4BE>::InputFilter( bufsize);
	else if (equalIdent( nm, "xml:Header")) rt = (protocol::InputFilter*) new filter::XmlHeaderFilter::InputFilter();
#ifdef WITH_LIBXML2
	else if (equalIdent( nm, "xml:libxml2")) rt = (protocol::InputFilter*) new filter::libxml2::InputFilter();
#endif
	if (!rt)
	{
		LOG_ERROR << "unknown filter '" << name << "'";
	}
	return rt;
}

protocol::FormatOutput* System::createDefaultFormatOutput() const
{
	return new filter::CharFilter<textwolf::charset::IsoLatin1>::FormatOutput();
}

protocol::FormatOutput* System::createFormatOutput( const char* name, unsigned int buffersize) const
{
	protocol::FormatOutput* rt = 0;
	char nm[32];
	if (!getName( nm, sizeof(nm), name))
	{
		LOG_ERROR << "unknown filter '" << name << "'";
		return 0;
	}
	if (equalIdent( nm, "char:isolatin1")) rt = (protocol::FormatOutput*) new filter::CharFilter<textwolf::charset::IsoLatin1>::FormatOutput();
	else if (equalIdent( nm, "char:iso88591")) rt = (protocol::FormatOutput*) new filter::CharFilter<textwolf::charset::IsoLatin1>::FormatOutput();
	else if (equalIdent( nm, "char:UTF8")) rt = (protocol::FormatOutput*) new filter::CharFilter<textwolf::charset::UTF8>::FormatOutput();
	else if (equalIdent( nm, "char:UTF16")) rt = (protocol::FormatOutput*) new filter::CharFilter<textwolf::charset::UTF16BE>::FormatOutput();
	else if (equalIdent( nm, "char:UTF16LE")) rt = (protocol::FormatOutput*) new filter::CharFilter<textwolf::charset::UTF16LE>::FormatOutput();
	else if (equalIdent( nm, "char:UTF16BE")) rt = (protocol::FormatOutput*) new filter::CharFilter<textwolf::charset::UTF16BE>::FormatOutput();
	else if (equalIdent( nm, "char:UCS2LE")) rt = (protocol::FormatOutput*) new filter::CharFilter<textwolf::charset::UCS2LE>::FormatOutput();
	else if (equalIdent( nm, "char:UCS2BE")) rt = (protocol::FormatOutput*) new filter::CharFilter<textwolf::charset::UCS2BE>::FormatOutput();
	else if (equalIdent( nm, "char:UCS4LE")) rt = (protocol::FormatOutput*) new filter::CharFilter<textwolf::charset::UCS4LE>::FormatOutput();
	else if (equalIdent( nm, "char:UCS4BE")) rt = (protocol::FormatOutput*) new filter::CharFilter<textwolf::charset::UCS4BE>::FormatOutput();

	else if (equalIdent( nm, "line:isolatin1")) rt = (protocol::FormatOutput*) new filter::LineFilter<textwolf::charset::IsoLatin1>::FormatOutput();
	else if (equalIdent( nm, "line:iso88591")) rt = (protocol::FormatOutput*) new filter::LineFilter<textwolf::charset::IsoLatin1>::FormatOutput();
	else if (equalIdent( nm, "line:UTF8")) rt = (protocol::FormatOutput*) new filter::LineFilter<textwolf::charset::UTF8>::FormatOutput();
	else if (equalIdent( nm, "line:UTF16")) rt = (protocol::FormatOutput*) new filter::LineFilter<textwolf::charset::UTF16BE>::FormatOutput();
	else if (equalIdent( nm, "line:UTF16LE")) rt = (protocol::FormatOutput*) new filter::LineFilter<textwolf::charset::UTF16LE>::FormatOutput();
	else if (equalIdent( nm, "line:UTF16BE")) rt = (protocol::FormatOutput*) new filter::LineFilter<textwolf::charset::UTF16BE>::FormatOutput();
	else if (equalIdent( nm, "line:UCS2LE")) rt = (protocol::FormatOutput*) new filter::LineFilter<textwolf::charset::UCS2LE>::FormatOutput();
	else if (equalIdent( nm, "line:UCS2BE")) rt = (protocol::FormatOutput*) new filter::LineFilter<textwolf::charset::UCS2BE>::FormatOutput();
	else if (equalIdent( nm, "line:UCS4LE")) rt = (protocol::FormatOutput*) new filter::LineFilter<textwolf::charset::UCS4LE>::FormatOutput();
	else if (equalIdent( nm, "line:UCS4BE")) rt = (protocol::FormatOutput*) new filter::LineFilter<textwolf::charset::UCS4BE>::FormatOutput();

	else if (equalIdent( nm, "xml:isolatin1")) rt = (protocol::FormatOutput*) new filter::XmlFilter<textwolf::charset::IsoLatin1>::FormatOutput( buffersize);
	else if (equalIdent( nm, "xml:iso88591")) rt = (protocol::FormatOutput*) new filter::XmlFilter<textwolf::charset::IsoLatin1>::FormatOutput( buffersize);
	else if (equalIdent( nm, "xml:UTF8")) rt = (protocol::FormatOutput*) new filter::XmlFilter<textwolf::charset::UTF8>::FormatOutput( buffersize);
	else if (equalIdent( nm, "xml:UTF16")) rt = (protocol::FormatOutput*) new filter::XmlFilter<textwolf::charset::UTF16BE>::FormatOutput( buffersize);
	else if (equalIdent( nm, "xml:UTF16LE")) rt = (protocol::FormatOutput*) new filter::XmlFilter<textwolf::charset::UTF16LE>::FormatOutput( buffersize);
	else if (equalIdent( nm, "xml:UTF16BE")) rt = (protocol::FormatOutput*) new filter::XmlFilter<textwolf::charset::UTF16BE>::FormatOutput( buffersize);
	else if (equalIdent( nm, "xml:UCS2LE")) rt = (protocol::FormatOutput*) new filter::XmlFilter<textwolf::charset::UCS2LE>::FormatOutput( buffersize);
	else if (equalIdent( nm, "xml:UCS2BE")) rt = (protocol::FormatOutput*) new filter::XmlFilter<textwolf::charset::UCS2BE>::FormatOutput( buffersize);
	else if (equalIdent( nm, "xml:UCS4LE")) rt = (protocol::FormatOutput*) new filter::XmlFilter<textwolf::charset::UCS4LE>::FormatOutput( buffersize);
	else if (equalIdent( nm, "xml:UCS4BE")) rt = (protocol::FormatOutput*) new filter::XmlFilter<textwolf::charset::UCS4BE>::FormatOutput( buffersize);
	else if (equalIdent( nm, "xml:Header")) rt = (protocol::FormatOutput*) new filter::XmlHeaderFilter::FormatOutput( buffersize);
#ifdef WITH_LIBXML2
	else if (equalIdent( nm, "xml:libxml2")) rt = (protocol::FormatOutput*) new filter::XmlFilter<textwolf::charset::UTF8>::FormatOutput( buffersize);
#endif
	if (!rt)
	{
		LOG_ERROR << "unknown filter '" << name << "'";
	}
	return rt;
}

Output::ItemType Output::print( const char* tag, unsigned int tagsize, const char* val, unsigned int valsize, bool newline)
{
	if (!m_formatoutput.get())
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
						if (!m_formatoutput->print( protocol::FormatOutput::Attribute, tag, tagsize, newline)) break;
						m_state ++;
					case 1:
						if (!m_formatoutput->print( protocol::FormatOutput::Value, val, valsize)) break;
						m_state ++;
					case 2:
						m_state = 0;
						return Data;
				}
				int err = m_formatoutput->getError();
				if (err)
				{
					LOG_ERROR << "error in format output attribute (" << err << ")";
					return Error;
				}
				return DoYield;
			}
			else
			{
				if (!m_formatoutput->print( protocol::FormatOutput::OpenTag, tag, tagsize, newline))
				{
					int err = m_formatoutput->getError();
					if (err)
					{
						LOG_ERROR << "error in format output open tag (" << err << ")";
						return Error;
					}
					return DoYield;
				}
				return Data;
			}
		}
		else if (val)
		{
			if (!m_formatoutput->print( protocol::FormatOutput::Value, val, valsize, newline))
			{
				int err = m_formatoutput->getError();
				if (err)
				{
					LOG_ERROR << "error in format output value (" << err << ")";
					return Error;
				}
				return DoYield;
			}
			return Data;
		}
		else
		{
			if (!m_formatoutput->print( protocol::FormatOutput::CloseTag, 0, 0, newline))
			{
				int err = m_formatoutput->getError();
				if (err)
				{
					LOG_ERROR << "error in format output close tag (" << err << ")";
					return Error;
				}
				return DoYield;
			}
			return Data;
		}
	}
	catch (std::bad_alloc)
	{
		LOG_ERROR << "out of memory in format output";
		return Error;
	}
}






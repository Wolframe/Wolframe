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
	LOG_DATA << "illegal state in iterator";
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

protocol::InputFilter* System::createInputFilter( const char* name, unsigned int bufsize) const
{
	char nm[32];
	if (!getName( nm, sizeof(nm), name))
	{
		LOG_ERROR << "unknown filter '" << name << "'";
		return 0;
	}

	if (boost::algorithm::iequals( nm, "char:isolatin1")) return new filter::CharFilter<textwolf::charset::IsoLatin1>::InputFilter();

	if (boost::algorithm::iequals( nm, "char:isolatin1") || boost::algorithm::iequals( nm, "char:iso88591"))
	{
		return new filter::CharFilter<textwolf::charset::IsoLatin1>::InputFilter();
	}
	if (boost::algorithm::iequals( nm, "char:utf8")) return new filter::CharFilter<textwolf::charset::UTF8>::InputFilter();
	if (boost::algorithm::iequals( nm, "char:UCS2LE")) return new filter::CharFilter<textwolf::charset::UCS2LE>::InputFilter();
	if (boost::algorithm::iequals( nm, "char:UCS2BE")) return new filter::CharFilter<textwolf::charset::UCS2BE>::InputFilter();
	if (boost::algorithm::iequals( nm, "char:UCS2")) return new filter::CharFilter<textwolf::charset::UCS2BE>::InputFilter();
	if (boost::algorithm::iequals( nm, "char:UCS4LE")) return new filter::CharFilter<textwolf::charset::UCS4LE>::InputFilter();
	if (boost::algorithm::iequals( nm, "char:UCS4BE")) return new filter::CharFilter<textwolf::charset::UCS4BE>::InputFilter();
	if (boost::algorithm::iequals( nm, "char:UCS4")) return new filter::CharFilter<textwolf::charset::UCS4BE>::InputFilter();

	if (boost::algorithm::iequals( nm, "line:isolatin1") || boost::algorithm::iequals( nm, "line:iso88591"))
	{
		return new filter::LineFilter<textwolf::charset::IsoLatin1>::InputFilter();
	}
	if (boost::algorithm::iequals( nm, "line:utf8")) return new filter::LineFilter<textwolf::charset::UTF8>::InputFilter( bufsize);
	if (boost::algorithm::iequals( nm, "line:utf16")) return new filter::LineFilter<textwolf::charset::UTF8>::InputFilter( bufsize);
	if (boost::algorithm::iequals( nm, "line:UCS2LE")) return new filter::LineFilter<textwolf::charset::UCS2LE>::InputFilter( bufsize);
	if (boost::algorithm::iequals( nm, "line:UCS2BE")) return new filter::LineFilter<textwolf::charset::UCS2BE>::InputFilter( bufsize);
	if (boost::algorithm::iequals( nm, "line:UCS2")) return new filter::LineFilter<textwolf::charset::UCS2BE>::InputFilter( bufsize);
	if (boost::algorithm::iequals( nm, "line:UCS4LE")) return new filter::LineFilter<textwolf::charset::UCS4LE>::InputFilter( bufsize);
	if (boost::algorithm::iequals( nm, "line:UCS4BE")) return new filter::LineFilter<textwolf::charset::UCS4BE>::InputFilter( bufsize);
	if (boost::algorithm::iequals( nm, "line:UCS4")) return new filter::LineFilter<textwolf::charset::UCS4BE>::InputFilter( bufsize);

	if (boost::algorithm::iequals( nm, "xml:isolatin1") || boost::algorithm::iequals( nm, "xml:iso88591"))
	{
		return new filter::XmlFilter<textwolf::charset::IsoLatin1>::InputFilter( bufsize);
	}
	if (boost::algorithm::iequals( nm, "xml:utf8")) return new filter::XmlFilter<textwolf::charset::UTF8>::InputFilter( bufsize);
	if (boost::algorithm::iequals( nm, "xml:utf16")) return new filter::XmlFilter<textwolf::charset::UTF8>::InputFilter( bufsize);
	if (boost::algorithm::iequals( nm, "xml:UCS2LE")) return new filter::XmlFilter<textwolf::charset::UCS2LE>::InputFilter( bufsize);
	if (boost::algorithm::iequals( nm, "xml:UCS2BE")) return new filter::XmlFilter<textwolf::charset::UCS2BE>::InputFilter( bufsize);
	if (boost::algorithm::iequals( nm, "xml:UCS2")) return new filter::XmlFilter<textwolf::charset::UCS2BE>::InputFilter( bufsize);
	if (boost::algorithm::iequals( nm, "xml:UCS4LE")) return new filter::XmlFilter<textwolf::charset::UCS4LE>::InputFilter( bufsize);
	if (boost::algorithm::iequals( nm, "xml:UCS4BE")) return new filter::XmlFilter<textwolf::charset::UCS4BE>::InputFilter( bufsize);
	if (boost::algorithm::iequals( nm, "xml:UCS4")) return new filter::XmlFilter<textwolf::charset::UCS4BE>::InputFilter( bufsize);
	if (boost::algorithm::iequals( nm, "xml:Header")) return new filter::XmlHeaderFilter::InputFilter();

	LOG_ERROR << "unknown filter '" << name << "'";
	return 0;
}

protocol::FormatOutput* System::createDefaultFormatOutput() const
{
	return new filter::CharFilter<textwolf::charset::IsoLatin1>::FormatOutput();
}

protocol::FormatOutput* System::createFormatOutput( const char* name, unsigned int buffersize) const
{
	char nm[32];
	if (!getName( nm, sizeof(nm), name))
	{
		LOG_ERROR << "unknown filter '" << name << "'";
		return 0;
	}

	if (boost::algorithm::iequals( nm, "char-isolatin1")) return new filter::CharFilter<textwolf::charset::IsoLatin1>::FormatOutput();
	if (boost::algorithm::iequals( nm, "char:utf8")) return new filter::CharFilter<textwolf::charset::UTF8>::FormatOutput();
	if (boost::algorithm::iequals( nm, "char:UCS2LE")) return new filter::CharFilter<textwolf::charset::UCS2LE>::FormatOutput();
	if (boost::algorithm::iequals( nm, "char:UCS2BE")) return new filter::CharFilter<textwolf::charset::UCS2BE>::FormatOutput();
	if (boost::algorithm::iequals( nm, "char:UCS4LE")) return new filter::CharFilter<textwolf::charset::UCS4LE>::FormatOutput();
	if (boost::algorithm::iequals( nm, "char:UCS4BE")) return new filter::CharFilter<textwolf::charset::UCS4BE>::FormatOutput();
	if (boost::algorithm::iequals( nm, "line:isolatin1")) return new filter::LineFilter<textwolf::charset::IsoLatin1>::FormatOutput();
	if (boost::algorithm::iequals( nm, "line:utf8")) return new filter::LineFilter<textwolf::charset::UTF8>::FormatOutput();
	if (boost::algorithm::iequals( nm, "line:UCS2LE")) return new filter::LineFilter<textwolf::charset::UCS2LE>::FormatOutput();
	if (boost::algorithm::iequals( nm, "line:UCS2BE")) return new filter::LineFilter<textwolf::charset::UCS2BE>::FormatOutput();
	if (boost::algorithm::iequals( nm, "line:UCS4LE")) return new filter::LineFilter<textwolf::charset::UCS4LE>::FormatOutput();
	if (boost::algorithm::iequals( nm, "line:UCS4BE")) return new filter::LineFilter<textwolf::charset::UCS4BE>::FormatOutput();
	if (boost::algorithm::iequals( nm, "xml:isolatin1")) return new filter::XmlFilter<textwolf::charset::IsoLatin1>::FormatOutput( buffersize);
	if (boost::algorithm::iequals( nm, "xml:utf8")) return new filter::XmlFilter<textwolf::charset::UTF8>::FormatOutput( buffersize);
	if (boost::algorithm::iequals( nm, "xml:UCS2LE")) return new filter::XmlFilter<textwolf::charset::UCS2LE>::FormatOutput( buffersize);
	if (boost::algorithm::iequals( nm, "xml:UCS2BE")) return new filter::XmlFilter<textwolf::charset::UCS2BE>::FormatOutput( buffersize);
	if (boost::algorithm::iequals( nm, "xml:UCS4LE")) return new filter::XmlFilter<textwolf::charset::UCS4LE>::FormatOutput( buffersize);
	if (boost::algorithm::iequals( nm, "xml:UCS4BE")) return new filter::XmlFilter<textwolf::charset::UCS4BE>::FormatOutput( buffersize);
	if (boost::algorithm::iequals( nm, "xml:Header")) return new filter::XmlHeaderFilter::FormatOutput( buffersize);

	LOG_ERROR << "unknown filter '" << name << "'";
	return 0;
}

Output::ItemType Output::print( const char* tag, unsigned int tagsize, const char* val, unsigned int valsize)
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
						if (!m_formatoutput->print( protocol::FormatOutput::Attribute, tag, tagsize)) break;
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
					LOG_ERROR << "error in format output (" << err << ")";
					return Error;
				}
				return DoYield;
			}
			else
			{
				if (!m_formatoutput->print( protocol::FormatOutput::OpenTag, tag, tagsize))
				{
					int err = m_formatoutput->getError();
					if (err)
					{
						LOG_ERROR << "error in format output (" << err << ")";
						return Error;
					}
					return DoYield;
				}
				return Data;
			}
		}
		else if (val)
		{
			if (!m_formatoutput->print( protocol::FormatOutput::Value, val, valsize))
			{
				int err = m_formatoutput->getError();
				if (err)
				{
					LOG_ERROR << "error in format output (" << err << ")";
					return Error;
				}
				return DoYield;
			}
			return Data;
		}
		else
		{
			if (!m_formatoutput->print( protocol::FormatOutput::CloseTag, 0, 0))
			{
				int err = m_formatoutput->getError();
				if (err)
				{
					LOG_ERROR << "error in format output (" << err << ")";
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






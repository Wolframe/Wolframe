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

InputFilterClosure::ItemType InputFilterClosure::fetch( const char*& e1, unsigned int& e1size, const char*& e2, unsigned int& e2size)
{
	if (!m_inputfilter.get() || m_bufsize==0)
	{
		return EndOfData;
	}
	if (m_value)
	{
		e2 = m_buf;
		e2size = m_bufpos;
	}
	else
	{
		e2 = 0;
		e2size = 0;
	}
	e1 = 0;
	e1size = 0;

	if (!m_inputfilter->getNext( &m_type, m_buf, m_bufsize-1, &m_bufpos))
	{
		switch (m_inputfilter->state())
		{
			case protocol::InputFilter::EndOfMessage:
				return DoYield;

			case protocol::InputFilter::Error:
				LOG_ERROR << "error in iterator (" << m_inputfilter->getError() << ")";
				return Error;

			case protocol::InputFilter::Open:
				LOG_DATA << "end of input";
				return EndOfData;
		}
	}
	else
	{
		switch (m_type)
		{
			case protocol::InputFilter::OpenTag:
				m_buf[ m_bufpos] = 0;
				e2 = m_buf;
				e2size = m_bufpos;
				init();
				return Data;

			case protocol::InputFilter::Value:
				m_buf[ m_bufpos] = 0;
				if (m_value)
				{
					e1 = m_value;
					e1size = m_bufpos -e1size -1;
				}
				else
				{
					e1 = m_buf;
					e1size = m_bufpos;
					init();
				}
				return Data;

			 case protocol::InputFilter::Attribute:
				m_buf[ m_bufpos++] = 0;
				if (m_value)
				{
					init();
					LOG_DATA << "illegal state in iterator";
					return Error;
				}
				else
				{
					m_value = m_buf+m_bufpos;
					return fetch( e1, e1size, e2, e2size);
				}
			 case protocol::InputFilter::CloseTag:
				init();
				return Data;
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

protocol::InputFilter* System::createInputFilter( const char* name) const
{
	if (!name) return new filter::CharFilter<textwolf::charset::IsoLatin1>::InputFilter(); //< default input filter
	char nm[32];
	if (!getName( nm, sizeof(nm), name))
	{
		LOG_ERROR << "unknown filter '" << name << "'";
		return 0;
	}

	if (boost::algorithm::iequals( nm, "char:isolatin1")) return new filter::CharFilter<textwolf::charset::IsoLatin1>::InputFilter();
	if (boost::algorithm::iequals( nm, "char:utf8")) return new filter::CharFilter<textwolf::charset::UTF8>::InputFilter();
	if (boost::algorithm::iequals( nm, "char:UCS2LE")) return new filter::CharFilter<textwolf::charset::UCS2LE>::InputFilter();
	if (boost::algorithm::iequals( nm, "char:UCS2BE")) return new filter::CharFilter<textwolf::charset::UCS2BE>::InputFilter();
	if (boost::algorithm::iequals( nm, "char:UCS4LE")) return new filter::CharFilter<textwolf::charset::UCS4LE>::InputFilter();
	if (boost::algorithm::iequals( nm, "char:UCS4BE")) return new filter::CharFilter<textwolf::charset::UCS4BE>::InputFilter();
	if (boost::algorithm::iequals( nm, "line:isolatin1")) return new filter::LineFilter<textwolf::charset::IsoLatin1>::InputFilter();
	if (boost::algorithm::iequals( nm, "line:utf8")) return new filter::LineFilter<textwolf::charset::UTF8>::InputFilter();
	if (boost::algorithm::iequals( nm, "line:UCS2LE")) return new filter::LineFilter<textwolf::charset::UCS2LE>::InputFilter();
	if (boost::algorithm::iequals( nm, "line:UCS2BE")) return new filter::LineFilter<textwolf::charset::UCS2BE>::InputFilter();
	if (boost::algorithm::iequals( nm, "line:UCS4LE")) return new filter::LineFilter<textwolf::charset::UCS4LE>::InputFilter();
	if (boost::algorithm::iequals( nm, "line:UCS4BE")) return new filter::LineFilter<textwolf::charset::UCS4BE>::InputFilter();
	if (boost::algorithm::iequals( nm, "xml:isolatin1")) return new filter::XmlFilter<textwolf::charset::IsoLatin1>::InputFilter();
	if (boost::algorithm::iequals( nm, "xml:utf8")) return new filter::XmlFilter<textwolf::charset::UTF8>::InputFilter();
	if (boost::algorithm::iequals( nm, "xml:UCS2LE")) return new filter::XmlFilter<textwolf::charset::UCS2LE>::InputFilter();
	if (boost::algorithm::iequals( nm, "xml:UCS2BE")) return new filter::XmlFilter<textwolf::charset::UCS2BE>::InputFilter();
	if (boost::algorithm::iequals( nm, "xml:UCS4LE")) return new filter::XmlFilter<textwolf::charset::UCS4LE>::InputFilter();
	if (boost::algorithm::iequals( nm, "xml:UCS4BE")) return new filter::XmlFilter<textwolf::charset::UCS4BE>::InputFilter();
	if (boost::algorithm::iequals( nm, "xml:Header")) return new filter::XmlHeaderFilter::InputFilter();

	LOG_ERROR << "unknown filter '" << name << "'";
	return 0;
}

protocol::FormatOutput* System::createFormatOutput( const char* name) const
{
	if (!name) return new filter::CharFilter<textwolf::charset::IsoLatin1>::FormatOutput(); //< default output filter
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
	if (boost::algorithm::iequals( nm, "xml:isolatin1")) return new filter::XmlFilter<textwolf::charset::IsoLatin1>::FormatOutput();
	if (boost::algorithm::iequals( nm, "xml:utf8")) return new filter::XmlFilter<textwolf::charset::UTF8>::FormatOutput();
	if (boost::algorithm::iequals( nm, "xml:UCS2LE")) return new filter::XmlFilter<textwolf::charset::UCS2LE>::FormatOutput();
	if (boost::algorithm::iequals( nm, "xml:UCS2BE")) return new filter::XmlFilter<textwolf::charset::UCS2BE>::FormatOutput();
	if (boost::algorithm::iequals( nm, "xml:UCS4LE")) return new filter::XmlFilter<textwolf::charset::UCS4LE>::FormatOutput();
	if (boost::algorithm::iequals( nm, "xml:UCS4BE")) return new filter::XmlFilter<textwolf::charset::UCS4BE>::FormatOutput();
	if (boost::algorithm::iequals( nm, "xml:Header")) return new filter::XmlHeaderFilter::FormatOutput();

	LOG_ERROR << "unknown filter '" << name << "'";
	return 0;
}

Output::ItemType Output::print( const char* e1, unsigned int e1size, const char* e2, unsigned int e2size)
{
	if (!m_formatoutput.get())
	{
		LOG_ERROR << "no output sink defined (output ignored)";
		return Error;
	}
	try
	{
		if (e1)
		{
			if (e2)
			{
				switch (m_state)
				{
					case 0:
						if (!m_formatoutput->print( protocol::FormatOutput::Attribute, e1, e1size)) break;
						m_state ++;
					case 1:
						if (!m_formatoutput->print( protocol::FormatOutput::Value, e2, e2size)) break;
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
				m_opentags.push( std::string( e1, e1size));

				if (!m_formatoutput->print( protocol::FormatOutput::OpenTag, e1, e1size))
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
		else if (e2)
		{
			if (!m_formatoutput->print( protocol::FormatOutput::Value, e2, e2size))
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
			if (m_opentags.empty())
			{
				LOG_ERROR << "error in tag hierarchy of format output: more tags closed than opened";
				return Error;
			}
			if (!m_formatoutput->print( protocol::FormatOutput::CloseTag, m_opentags.top().c_str(), m_opentags.top().size()))
			{
				int err = m_formatoutput->getError();
				if (err)
				{
					LOG_ERROR << "error in format output (" << err << ")";
					return Error;
				}
				return DoYield;
			}
			m_opentags.pop();
			return Data;
		}
	}
	catch (std::bad_alloc)
	{
		LOG_ERROR << "out of memory in format output";
		return Error;
	}
}






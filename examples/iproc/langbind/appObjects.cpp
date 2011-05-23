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
		e1 = m_buf;
		e1size = m_bufpos;
	}
	else
	{
		e1 = 0;
		e1size = 0;
	}
	e2 = 0;
	e2size = 0;

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
				e1 = m_buf;
				e1size = m_bufpos;
				init();
				return Data;

			case protocol::InputFilter::Value:
				m_buf[ m_bufpos] = 0;
				if (m_value)
				{
					e2 = m_value;
					e2size = m_bufpos -e1size -1;
				}
				else
				{
					e2 = m_buf;
					e2size = m_bufpos;
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

protocol::InputFilter* System::createInputFilter( const char* name) const
{
	if (!name) return new filter::CharFilter<textwolf::charset::IsoLatin1>::InputFilter(); //< default input filter
	if (boost::algorithm::iequals( name, "char:isolatin1")) return new filter::CharFilter<textwolf::charset::IsoLatin1>::InputFilter();
	if (boost::algorithm::iequals( name, "char:utf8")) return new filter::CharFilter<textwolf::charset::UTF8>::InputFilter();

	LOG_ERROR << "unknown input filter function '" << name << "'";
	return 0;
}

protocol::FormatOutput* System::createFormatOutput( const char* name) const
{
	if (!name) return new filter::CharFilter<textwolf::charset::IsoLatin1>::FormatOutput(); //< default output filter
	if (boost::algorithm::iequals( name, "char-isolatin1")) return new filter::CharFilter<textwolf::charset::IsoLatin1>::FormatOutput();

	LOG_ERROR << "unknown output filter function '" << name << "'";
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






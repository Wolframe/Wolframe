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
	std::string nm( str, strlen(prefix));
	return boost::algorithm::iequals( nm, prefix);
}

static bool equalIdent( const char* str, const char* prefix)
{
	return boost::algorithm::iequals( str, prefix);
}

Filter::Filter( const char* name, unsigned int ib, unsigned int ob)
{
	if (startsWith( name, "char:"))
	{
		filter::CharFilter flt( name+5);
		m_inputfilter = flt.inputFilter();
		m_formatoutput = flt.formatOutput();
	}
	else if (startsWith( name, "line:"))
	{
		filter::LineFilter flt( name+5, ib?ib:2048);
		m_inputfilter = flt.inputFilter();
		m_formatoutput = flt.formatOutput();
	}
	else if (equalIdent( name, "xml:textwolf"))
	{
		filter::TextwolfXmlFilter tw( ib?ib:4096, ob?ob:1024);
		m_inputfilter = tw.inputFilter();
		m_formatoutput = tw.formatOutput();
	}
	else if (startsWith( name, "xml:textwolf:"))
	{
		filter::TextwolfXmlFilter tw( ib?ib:4096, ob?ob:1024, name+strlen("xml:textwolf:"));
		m_inputfilter = tw.inputFilter();
		m_formatoutput = tw.formatOutput();
	}
#ifdef WITH_LIBXML2
	else if (equalIdent( name, "xml:libxml2"))
	{
		filter::Libxml2Filter tw( ib?ib:4096);
		m_inputfilter = tw.inputFilter();
		m_formatoutput = tw.formatOutput();
	}
#endif
	else
	{
		LOG_ERROR << "unknown filter '" << name << "'";
	}
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
				else if (m_formatoutput->state() != protocol::FormatOutput::EndOfBuffer)
				{
					// in case of return false and no error check if there is a follow format ouptut filter to continue with:
					protocol::FormatOutput* follow = m_formatoutput->createFollow();
					if (follow)
					{
						m_formatoutput.reset( follow);
						return print( tag, tagsize, val, valsize);
					}

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
						LOG_ERROR << "error in format output open tag (" << err << ")";
						return Error;
					}
					else  if (m_formatoutput->state() != protocol::FormatOutput::EndOfBuffer)
					{
						// in case of return false and no error check if there is a follow format ouptut filter to continue with:
						protocol::FormatOutput* follow = m_formatoutput->createFollow();
						if (follow)
						{
							m_formatoutput.reset( follow);
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
			if (!m_formatoutput->print( protocol::FormatOutput::Value, val, valsize))
			{
				int err = m_formatoutput->getError();
				if (err)
				{
					LOG_ERROR << "error in format output value (" << err << ")";
					return Error;
				}
				else if (m_formatoutput->state() != protocol::FormatOutput::EndOfBuffer)
				{
					// in case of return false and no error check if there is a follow format ouptut filter to continue with:
					protocol::FormatOutput* follow = m_formatoutput->createFollow();
					if (follow)
					{
						m_formatoutput.reset( follow);
						return print( tag, tagsize, val, valsize);
					}
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
					LOG_ERROR << "error in format output close tag (" << err << ")";
					return Error;
				}
				else if (m_formatoutput->state() != protocol::FormatOutput::EndOfBuffer)
				{
					// in case of return false and no error check if there is a follow format ouptut filter to continue with:
					protocol::FormatOutput* follow = m_formatoutput->createFollow();
					if (follow)
					{
						m_formatoutput.reset( follow);
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
		LOG_ERROR << "out of memory in format output";
		return Error;
	}
}






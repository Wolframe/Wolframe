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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
/// \file cmdbind/ioFilterCommandHandler.cpp
/// \brief Command handler base class for processing content and writing output through filters
#include "cmdbind/ioFilterCommandHandler.hpp"
#include "logger-v1.hpp"

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;

void IOFilterCommandHandler::setInputFilter( const langbind::InputFilterR& in)
{
	m_input.setInputFilter( in);

	// synchronize inherited meta data:
	if (m_input.inputfilter().get() && m_output.outputfilter().get())
	{
		m_output.outputfilter()->inheritMetaData( m_input.inputfilter()->getMetaDataRef());
	}
}

void IOFilterCommandHandler::setOutputFilter( const langbind::OutputFilterR& out)
{
	m_output.setOutputFilter( out);

	// synchronize inherited meta data:
	if (m_input.inputfilter().get() && m_output.outputfilter().get())
	{
		m_output.outputfilter()->inheritMetaData( m_input.inputfilter()->getMetaDataRef());
	}
}

IOFilterCommandHandler::Operation IOFilterCommandHandler::nextOperation()
{
	if (m_done) return CLOSE;

	const char* errmsg = 0;
	for (;;) switch (call( errmsg))
	{
		case Ok:
			m_done = true;
			if (!m_output.outputfilter().get())
			{
				return CLOSE;
			}
			m_output.outputfilter()->getOutput( m_writeptr, m_writesize);
			return m_writesize?WRITE:CLOSE;

		case Error:
		{
			if (errmsg)
			{
				LOG_ERROR << "Error calling procedure: " << (errmsg?errmsg:"unknown");
				setLastError( (errmsg?errmsg:"processing error"));
			}
			return CLOSE;
		}
		case Yield:
		{
			if (m_input.inputfilter().get())
			{
				switch (m_input.inputfilter()->state())
				{
					case langbind::InputFilter::Start:
					case langbind::InputFilter::Open:
						break;
	
					case langbind::InputFilter::EndOfMessage:
						
						if (m_input.gotEoD())
						{
							LOG_ERROR << "error in input filter: unexpected end of input";
							setLastError( "unexpected end of input");
							return CLOSE;
						}
						return READ;
	
					case langbind::InputFilter::Error:
						errmsg = m_input.inputfilter()->getError();
						LOG_ERROR << "error in input filter: " << (errmsg?errmsg:"unknown");
						setLastError( std::string( "error in input: ") + (errmsg?errmsg:"unknown"));
						return CLOSE;
				}
			}
			if (m_output.outputfilter().get())
			{
				switch (m_output.outputfilter()->state())
				{
					case langbind::OutputFilter::Start:
					case langbind::OutputFilter::Open:
					case langbind::OutputFilter::EndOfBuffer:
						m_output.outputfilter()->getOutput( m_writeptr, m_writesize);
						return WRITE;

					case langbind::OutputFilter::Error:
						errmsg = m_output.outputfilter()->getError();
						LOG_ERROR << "error in output filter: " << (errmsg?errmsg:"unknown");
						setLastError( std::string("error in output: ") + (errmsg?errmsg:"unknown"));
						return CLOSE;
				}
			}
			LOG_ERROR << "illegal state (missing filter)";
			setLastError( "internal: illegal state (missing filter)");
			return CLOSE;
		}
	}
}

void IOFilterCommandHandler::putInput( const void* chunk_, std::size_t chunksize_, bool eod)
{
	m_input.putInput( (const char*)chunk_, chunksize_, eod);
}

void IOFilterCommandHandler::getOutput( const void*& chunk_, std::size_t& chunksize_)
{
	chunk_ = m_writeptr;
	chunksize_ = m_writesize;
	m_writesize = 0;
}


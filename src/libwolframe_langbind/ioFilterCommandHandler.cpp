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

void IOFilterCommandHandler::setFilterAs( const langbind::InputFilterR& in)
{
	if (m_inputfilter->state() != langbind::InputFilter::Start)
	{
		throw std::runtime_error( "cannot change input filter after first read");
	}
	m_inputfilter.reset( in->copy());

	// synchronize inherited meta data:
	if (m_outputfilter.get())
	{
		m_outputfilter->inheritMetaData( m_inputfilter->getMetaDataRef());
	}
}

void IOFilterCommandHandler::setFilterAs( const langbind::OutputFilterR& out)
{
	if (m_outputfilter->state() != langbind::OutputFilter::Start)
	{
		throw std::runtime_error( "cannot change output filter after first print");
	}
	langbind::OutputFilter* of = out->copy();
	of->setOutputChunkSize( m_outputfilter->outputChunkSize());
	m_outputfilter.reset( of);;
	// synchronize inherited meta data:
	m_outputfilter->inheritMetaData( m_inputfilter->getMetaDataRef());
}

IOFilterCommandHandler::Operation IOFilterCommandHandler::nextOperation()
{
	const char* errmsg = 0;
	for (;;) switch (call( errmsg))
	{
		case Ok:
			return CLOSE;

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
			if (m_inputfilter.get())
			{
				switch (m_inputfilter->state())
				{
					case langbind::InputFilter::Start:
					case langbind::InputFilter::Open:
						break;
	
					case langbind::InputFilter::EndOfMessage:
						
						if (m_gotEoD)
						{
							LOG_ERROR << "error in input filter: unexpected end of input";
							setLastError( "unexpected end of input");
							return CLOSE;
						}
						return READ;
	
					case langbind::InputFilter::Error:
						errmsg = m_inputfilter->getError();
						LOG_ERROR << "error in input filter: " << (errmsg?errmsg:"unknown");
						setLastError( std::string( "error in input: ") + (errmsg?errmsg:"unknown"));
						return CLOSE;
				}
			}
			if (m_outputfilter.get())
			{
				switch (m_outputfilter->state())
				{
					case langbind::OutputFilter::Start:
					case langbind::OutputFilter::Open:
					case langbind::OutputFilter::EndOfBuffer:
						return WRITE;
	
					case langbind::OutputFilter::Error:
						errmsg = m_outputfilter->getError();
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

void IOFilterCommandHandler::putInput( const void* begin, std::size_t bytesTransferred, bool eod)
{
	m_gotEoD = eod;
	m_inputfilter->putInput( (const char*)begin, bytesTransferred, eod);
}

void IOFilterCommandHandler::getOutput( const void*& begin, std::size_t& bytesToTransfer)
{
	m_outputfilter->getOutput( begin, bytesToTransfer);
}


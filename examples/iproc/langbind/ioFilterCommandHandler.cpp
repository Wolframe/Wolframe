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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file langbind/ioFilterCommandHandler.cpp

#include "langbind/ioFilterCommandHandler.hpp"
#include "logger-v1.hpp"
#include "filters/char_filter.hpp"

using namespace _Wolframe;
using namespace _Wolframe::protocol;

IOFilterCommandHandler::IOFilterCommandHandler()
	:m_state(Processing)
	,m_writedata(0)
	,m_writedatasize(0)

{
	filter::CharFilter flt( "UTF-8");
	m_inputfilter = flt.inputFilter();
	m_formatoutput = flt.formatOutput();
}

void IOFilterCommandHandler::setInputBuffer( void* buf, std::size_t allocsize, std::size_t size, std::size_t itrpos)
{
	m_input = protocol::InputBlock( (char*)buf, allocsize, size);
	protocol::InputBlock::iterator start = m_input.at( itrpos);
	m_eoD = m_input.getEoD( start);
	InputFilter* flt = m_inputfilter.get();
	if (flt)
	{
		flt->protocolInput( m_input.charptr()+itrpos, m_eoD-start, m_input.gotEoD());
	}
}

void IOFilterCommandHandler::setOutputBuffer( void* buf, std::size_t size, std::size_t pos)
{
	if (m_formatoutput.get()) m_formatoutput->init( (char*)buf+pos, size-pos);
}

enum
{
	ErrFormatOutputDeleted=-1,
	ErrInputFilterDeleted=-2,
	ErrFormatOutput=-3,
	ErrInputFilter=-4,
	ErrUnknown=-5
};

CommandHandler::Operation IOFilterCommandHandler::nextOperation()
{
	FormatOutput* flt;

	for (;;) switch (m_state)
	{
		case Terminated:
			return CLOSED;

		case FlushingOutput:
			m_state = Processing;
			if (!(flt = m_formatoutput.get()))
			{
				LOG_ERROR << "Output filter undefined";
				m_statusCode = ErrFormatOutputDeleted;
				m_state = DiscardInput;
				return READ;
			}
			m_writedata = flt->ptr();
			m_writedatasize = flt->pos();
			flt->setPos(0);
			return WRITE;

		case DiscardInput:
			flt = m_formatoutput.get();
			if (flt)
			{
				m_writedatasize=flt->pos();
				if (m_writedatasize)
				{
					m_writedata = flt->ptr();
					flt->setPos(0);
					return WRITE;
				}
			}
			if (m_input.gotEoD())
			{
				m_formatoutput.reset(0);
				m_writedata = "\r\n.\r\n";
				m_writedatasize = std::strlen("\r\n.\r\n");
				m_state = Terminated;
				return WRITE;
			}
			return READ;

		case Processing:
			switch (call( m_statusCode))
			{
				case Ok:
					m_state = DiscardInput;
					continue;

				case Error:
					m_state = DiscardInput;
					if (m_statusCode == 0)
					{
						m_statusCode = ErrUnknown;
					}
					continue;

				case Yield:
					if (m_inputfilter.get())
					{
						switch (m_inputfilter->state())
						{
							case InputFilter::Open:
								m_state = FlushingOutput;
								continue;

							case InputFilter::EndOfMessage:
								return READ;

							case InputFilter::Error:
								m_statusCode = ErrInputFilter;
								m_state = DiscardInput;
								return READ;
						}
					}
					else if (m_formatoutput.get())
					{
						switch (m_formatoutput->state())
						{
							case FormatOutput::Open:
								m_state = FlushingOutput;
								continue;

							case FormatOutput::EndOfBuffer:
								m_writedata = m_formatoutput->ptr();
								m_writedatasize = m_formatoutput->size();
								if (m_writedatasize > 0)
								{
									m_formatoutput->setState( FormatOutput::Open);
									return WRITE;
								}
								/* no break here !*/

							case FormatOutput::Error:
								m_statusCode = ErrFormatOutput;
								m_state = DiscardInput;
								return READ;
						}
					}
					else
					{
						m_writedata = "";
						m_writedatasize = 0;
						return WRITE;
					}
			}
	}
	LOG_ERROR << "illegal state";
	return CLOSED;
}



void IOFilterCommandHandler::putInput( const void *begin, std::size_t bytesTransferred)
{
	m_input.setPos( bytesTransferred + ((const char*)begin - m_input.charptr()));
	m_eoD = m_input.getEoD( m_input.begin());
	m_inputfilter.get()->protocolInput( m_input.ptr(), m_eoD-m_input.begin(), m_input.gotEoD(), 0);
}

void IOFilterCommandHandler::getInputBlock( void*& begin, std::size_t& maxBlockSize)
{
	if (!m_input.getNetworkMessageRead( begin, maxBlockSize))
	{
		throw std::logic_error( "buffer too small");
	}
}

void IOFilterCommandHandler::getOutput( const void*& begin, std::size_t& bytesToTransfer)
{
	begin = m_writedata;
	bytesToTransfer = m_writedatasize;
}

void IOFilterCommandHandler::getDataLeft( const void*& begin, std::size_t& nofBytes)
{
	std::size_t pos = m_eoD - m_input.begin();
	begin = (const void*)(m_input.charptr() + pos);
	nofBytes = m_input.pos() - pos;
}



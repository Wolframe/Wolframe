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
///\file langbind_ioFilterCommandHandler.cpp

#include "langbind/ioFilterCommandHandler.hpp"
#include "logger-v1.hpp"
#include "filter/char_filter.hpp"
#include "filter.hpp"

using namespace _Wolframe;
using namespace _Wolframe::protocol;

IOFilterCommandHandler::IOFilterCommandHandler()
	:m_state(Processing)
	,m_writedata(0)
	,m_writedatasize(0)
	,m_itrpos(0)
{
	static langbind::CharFilterFactory defaultff;
	langbind::Filter flt = defaultff.create( "UTF-8");
	m_inputfilter = flt.inputfilter();
	m_outputfilter = flt.outputfilter();
}

IOFilterCommandHandler::~IOFilterCommandHandler()
{}

void IOFilterCommandHandler::setInputBuffer( void* buf, std::size_t allocsize)
{
	m_input = protocol::InputBlock( (char*)buf, allocsize);
}

void IOFilterCommandHandler::setOutputBuffer( void* buf, std::size_t size, std::size_t pos)
{
	if (m_outputfilter.get()) m_outputfilter->init( (char*)buf+pos, size-pos);
}

enum
{
	ErrInternal=-1,
	ErrProcessing=-2,
	ErrResources=-3
};

CommandHandler::Operation IOFilterCommandHandler::nextOperation()
{
	OutputFilter* flt;
	const char* errmsg = 0;

	for (;;) switch (m_state)
	{
		case Terminated:
			return CLOSED;

		case FlushingOutput:
			m_state = Processing;
			if (!(flt = m_outputfilter.get()))
			{
				LOG_ERROR << "Output filter undefined";
				m_statusCode = ErrInternal;
				m_state = DiscardInput;
				return READ;
			}
			m_writedata = flt->ptr();
			m_writedatasize = flt->pos();
			flt->setPos(0);
			return WRITE;

		case DiscardInput:
			flt = m_outputfilter.get();
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
				m_outputfilter.reset(0);
				m_writedata = "\r\n.\r\n";
				m_writedatasize = std::strlen("\r\n.\r\n");
				m_state = Terminated;
				return WRITE;
			}
			return READ;

		case Processing:
			switch (call( errmsg))
			{
				case Ok:
					m_state = DiscardInput;
					continue;

				case Error:
					m_state = DiscardInput;
					if (errmsg)
					{
						LOG_ERROR << "Error calling procedure: " << (errmsg?errmsg:"unknown");
						m_statusCode = ErrProcessing;
					}
					continue;

				case Yield:
					if (m_inputfilter.get())
					{
						switch (m_inputfilter->state())
						{
							case InputFilter::Open:
								break;

							case InputFilter::EndOfMessage:
								return READ;

							case InputFilter::Error:
								errmsg = m_inputfilter->getError();
								LOG_ERROR << "Error in input filter: " << (errmsg?errmsg:"unknown");
								m_statusCode = ErrProcessing;
								m_state = DiscardInput;
								return READ;
						}
					}
					if (m_outputfilter.get())
					{
						switch (m_outputfilter->state())
						{
							case OutputFilter::Open:
								m_state = FlushingOutput;
								continue;

							case OutputFilter::EndOfBuffer:
								if (m_outputfilter->pos() > 0)
								{
									m_state = FlushingOutput;
									continue;
								}
								/* no break here !*/

							case OutputFilter::Error:
								errmsg = m_outputfilter->getError();
								LOG_ERROR << "Error in output filter: " << (errmsg?errmsg:"unknown");
								m_statusCode = ErrProcessing;
								m_state = DiscardInput;
								return READ;
						}
					}
					LOG_ERROR << "Illegal state (missing filter)";
					m_statusCode = ErrInternal;
					m_state = DiscardInput;
					return READ;
			}
	}
	LOG_ERROR << "illegal state";
	return CLOSED;
}


void IOFilterCommandHandler::putInput( const void *begin, std::size_t bytesTransferred)
{
	std::size_t startidx = (const char*)begin - m_input.charptr();
	m_input.setPos( bytesTransferred + startidx);
	if (m_itrpos != 0)
	{
		if (startidx != m_itrpos) throw std::logic_error( "unexpected buffer start for input to cmd handler");
		startidx = 0; //... start of buffer is end last message (part of eoD marker)
	}
	protocol::InputBlock::iterator start = m_input.at( startidx);
	m_eoD = m_input.getEoD( start);
	InputFilter* flt = m_inputfilter.get();
	if (flt)
	{
		flt->protocolInput( start.ptr(), m_eoD-start, m_input.gotEoD());
	}
}

void IOFilterCommandHandler::getInputBlock( void*& begin, std::size_t& maxBlockSize)
{
	if (!m_input.getNetworkMessageRead( begin, maxBlockSize))
	{
		throw std::logic_error( "buffer too small");
	}
	m_itrpos = ((const char*)begin - m_input.charptr());
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



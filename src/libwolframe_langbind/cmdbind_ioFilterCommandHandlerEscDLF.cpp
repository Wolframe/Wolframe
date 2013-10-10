/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
///\file cmdbind_ioFilterCommandHandlerEscDLF.cpp

#include "cmdbind/ioFilterCommandHandlerEscDLF.hpp"
#include "logger-v1.hpp"
#include "filter/null_filter.hpp"
#include "filter/filter.hpp"

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;
using namespace _Wolframe::langbind;

IOFilterCommandHandlerEscDLF::IOFilterCommandHandlerEscDLF()
	:m_state(Processing)
	,m_writedata(0)
	,m_writedatasize(0)
	,m_writedata_chksum(0)
	,m_writedata_chkpos(0)
	,m_itrpos(0)
{
	langbind::Filter flt = createNullFilter( "", "");
	m_inputfilter = flt.inputfilter();
	m_outputfilter = flt.outputfilter();
}

IOFilterCommandHandlerEscDLF::~IOFilterCommandHandlerEscDLF()
{}

const char* IOFilterCommandHandlerEscDLF::interruptDataSessionMarker() const
{
	switch (m_state)
	{
		case Processing:
		case FlushingOutput:
		case DiscardInput: return "\r\n.\r\n";
		case Terminated: return "";
	}
	return "";
}

void IOFilterCommandHandlerEscDLF::setInputBuffer( void* buf, std::size_t allocsize)
{
	m_input = protocol::InputBlock( (char*)buf, allocsize);
}

void IOFilterCommandHandlerEscDLF::setOutputBuffer( void* buf, std::size_t size, std::size_t pos)
{
	m_output = protocol::OutputBlock( buf, size, pos);
	if (m_outputfilter.get()) m_outputfilter->setOutputBuffer( (char*)buf+pos, size-pos);
}

void IOFilterCommandHandlerEscDLF::getFilterOutputWriteData()
{
	OutputFilter* flt = m_outputfilter.get();
	m_writedata = m_output.ptr();
	m_writedatasize = flt->getPosition()+m_output.pos();

	langbind::OutputFilter::calculateCheckSum( m_writedata_chksum, 0, (const char*)m_writedata, m_writedatasize);
	m_writedata_chkpos += m_writedatasize;

	m_escapeBuffer.process( m_output.charptr(), m_output.size(), m_writedatasize);
	m_output.setPos(0);
	flt->setOutputBuffer( m_output.ptr(), m_output.size());
}

CommandHandler::Operation IOFilterCommandHandlerEscDLF::nextOperation()
{
	OutputFilter* flt;
	const char* errmsg = 0;

	for (;;)
	{
		LOG_TRACE << "STATE IOFilterCommandHandlerEscDLF " << stateName( m_state);

		switch (m_state)
		{
			case Terminated:
				return CLOSE;

			case FlushingOutput:
				if (!(flt = m_outputfilter.get()))
				{
					LOG_ERROR << "Output filter undefined";
					m_lastError = "internal";
					m_state = DiscardInput;
					return READ;
				}
				getFilterOutputWriteData();
				if (!m_escapeBuffer.hasData())
				{
					m_state = Processing;
					flt->setState( OutputFilter::Open);
				}
				return WRITE;

			case DiscardInput:
				if (m_outputfilter.get())
				{
					getFilterOutputWriteData();
					if (m_writedatasize) return WRITE;
				}
				if (m_input.gotEoD())
				{
#ifdef WOLFRAME_OUTPUT_WITH_CHECKSUM
					if (m_writedata_chkpos != m_outputfilter->chkpos())
					{
						throw std::runtime_error( "output byte sum check failed");
					}
					if (m_writedata_chksum != m_outputfilter->chksum())
					{
						throw std::runtime_error( "output checksum check failed");
					}
#endif
					m_outputfilter.reset();
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
							m_lastError = (errmsg?errmsg:"processing error");
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
									if (m_input.gotEoD())
									{
										LOG_ERROR << "error in input filter: unexpected end of input";
										m_lastError = "input";
										m_state = DiscardInput;
										continue;
									}
									return READ;

								case InputFilter::Error:
									errmsg = m_inputfilter->getError();
									LOG_ERROR << "error in input filter: " << (errmsg?errmsg:"unknown");
									m_lastError = "input";
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
									m_state = FlushingOutput;
									continue;

								case OutputFilter::Error:
									errmsg = m_outputfilter->getError();
									LOG_ERROR << "error in output filter: " << (errmsg?errmsg:"unknown");
									m_lastError = "internal";
									m_state = DiscardInput;
									return READ;
							}
						}
						LOG_ERROR << "Illegal state (missing filter)";
						m_lastError = "internal";
						m_state = DiscardInput;
						return READ;
				}
		}
		LOG_ERROR << "illegal state";
		return CLOSE;
	}
}


void IOFilterCommandHandlerEscDLF::putInput( const void *begin, std::size_t bytesTransferred)
{
	std::size_t startidx = (const char*)begin - m_input.charptr();
	if (bytesTransferred + startidx > m_input.size())
	{
		throw std::logic_error( "illegal input range passed to IOFilterCommandHandlerEscDLF");
	}
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
		flt->putInput( start.ptr(), m_eoD-start, m_input.gotEoD());
		flt->setState( InputFilter::Open);
	}
}

void IOFilterCommandHandlerEscDLF::getInputBlock( void*& begin, std::size_t& maxBlockSize)
{
	if (!m_input.getNetworkMessageRead( begin, maxBlockSize))
	{
		throw std::logic_error( "buffer too small");
	}
	m_itrpos = ((const char*)begin - m_input.charptr());
}

void IOFilterCommandHandlerEscDLF::getOutput( const void*& begin, std::size_t& bytesToTransfer)
{
	begin = m_writedata;
	bytesToTransfer = m_writedatasize;
}

void IOFilterCommandHandlerEscDLF::getDataLeft( const void*& begin, std::size_t& nofBytes)
{
	std::size_t pos = m_eoD - m_input.begin();
	begin = (const void*)(m_input.charptr() + pos);
	nofBytes = m_input.pos() - pos;
}

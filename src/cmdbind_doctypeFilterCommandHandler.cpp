/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
///\file cmdbind_doctypeFilterCommandHandler.cpp

#include "cmdbind/doctypeFilterCommandHandler.hpp"
#include "logger-v1.hpp"
#include "filter/textwolf_filter.hpp"
#include "filter/filter.hpp"
#include "utils/doctype.hpp"

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;
using namespace _Wolframe::langbind;

DoctypeFilterCommandHandler::DoctypeFilterCommandHandler()
	:m_state(Processing)
	,m_itrpos(0)
{
	langbind::Filter flt = createTextwolfXmlFilter( "xml:textwolf");
	m_inputfilter = flt.inputfilter();
}

DoctypeFilterCommandHandler::~DoctypeFilterCommandHandler()
{}

void DoctypeFilterCommandHandler::setInputBuffer( void* buf, std::size_t allocsize)
{
	m_input = protocol::InputBlock( (char*)buf, allocsize);
}

void DoctypeFilterCommandHandler::setOutputBuffer( void*, std::size_t, std::size_t)
{}

std::string DoctypeFilterCommandHandler::doctypeid() const
{
	if (m_state != Terminated || m_state != Done) throw std::logic_error( "illegal call of get doctypeid in this state");
	return m_doctypeid;
}

CommandHandler::Operation DoctypeFilterCommandHandler::nextOperation()
{
	std::string doctype;

	for (;;) switch (m_state)
	{
		case Done:
		case Terminated:
			return CLOSE;

		case Processing:
			if (!m_inputfilter->getDocType( doctype))
			{
				return READ;
			}
			else
			{
				m_doctypeid = utils::getIdFromDoctype( doctype);
				m_state = Terminated;
				return CLOSE;
			}
		default:
			break;
	}
	LOG_ERROR << "illegal state";
	return CLOSE;
}


void DoctypeFilterCommandHandler::putInput( const void *begin, std::size_t bytesTransferred)
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
		flt->putInput( start.ptr(), m_eoD-start, m_input.gotEoD());
		flt->setState( InputFilter::Open);
	}
}

void DoctypeFilterCommandHandler::getInputBlock( void*& begin, std::size_t& maxBlockSize)
{
	if (!m_input.getNetworkMessageRead( begin, maxBlockSize))
	{
		throw std::logic_error( "buffer too small");
	}
	m_itrpos = ((const char*)begin - m_input.charptr());
}

void DoctypeFilterCommandHandler::getOutput( const void*& begin, std::size_t& bytesToTransfer)
{
	begin = 0;
	bytesToTransfer = 0;
}

void DoctypeFilterCommandHandler::getDataLeft( const void*& begin, std::size_t& nofBytes)
{
	if (m_state == Terminated)
	{
		std::size_t pos = m_eoD - m_input.begin();
		m_inputbuffer.append( m_input.charptr() + pos, m_input.pos() - pos);
		m_state = Done;
	}
	begin = (void*)m_inputbuffer.c_str();
	nofBytes = m_inputbuffer.size();
}



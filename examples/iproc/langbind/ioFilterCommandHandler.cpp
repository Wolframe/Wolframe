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
{
	filter::CharFilter flt( "UTF-8");
	m_inputfilter = flt.inputFilter();
	m_formatoutput = flt.formatOutput();
}

void IOFilterCommandHandler::passIO( const InputBlock& input, const OutputBlock& output)
{
	Parent::passIO( input, output);
	m_eoD = m_input.getEoD( m_input.begin());
	m_gotEoD = m_input.gotEoD();
	m_inputfilter.get()->protocolInput( m_input.ptr(), m_eoD-m_input.begin(), m_gotEoD);
	m_formatoutput->init( m_output.ptr(), m_output.size());
}

void IOFilterCommandHandler::putInput( const void *begin, std::size_t bytesTransferred)
{
	Parent::putInput( begin, bytesTransferred);
	m_eoD = m_input.getEoD( m_input.begin());
	m_gotEoD = m_input.gotEoD();
	m_inputfilter.get()->protocolInput( m_input.ptr(), m_eoD-m_input.begin(), m_gotEoD);
}

void IOFilterCommandHandler::getOutput( const void*& begin, std::size_t& bytesToTransfer)
{
	protocol::FormatOutput* flt = m_formatoutput.get();
	if (flt)
	{
		begin = flt->ptr();
		bytesToTransfer = flt->pos();
		m_output.setPos(0);
		flt->setPos(0);
		if (m_state == FlushingOutput)
		{
			m_state = Processing;
		}
	}
	else
	{
		begin = flt->ptr();
		bytesToTransfer = 0;
		m_output.setPos(0);
		if (m_state == FlushingOutput)
		{
			m_state = Processing;
		}
	}
}


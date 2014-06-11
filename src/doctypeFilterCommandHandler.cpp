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
///\file cmdbind_doctypeFilterCommandHandler.cpp
#include "doctypeFilterCommandHandler.hpp"
#include "processor/execContext.hpp"
#include "logger-v1.hpp"
#include "utils/parseUtils.hpp"
#include "utils/fileUtils.hpp"
#include <cstring>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;

void DoctypeFilterCommandHandler::setInputBuffer( void* buf, std::size_t allocsize)
{
	m_input = protocol::InputBlock( (char*)buf, allocsize);
}

void DoctypeFilterCommandHandler::setOutputBuffer( void*, std::size_t, std::size_t)
{}

const types::DoctypeInfoR& DoctypeFilterCommandHandler::info() const
{
	return m_doctypeDetector->info();
}

bool DoctypeFilterCommandHandler::createDoctypeDetector()
{
	proc::ExecContext* ctx = execContext();
	if (!ctx)
	{
		setLastError( "non execution context defined");
		return false;
	}
	m_doctypeDetector.reset( ctx->provider()->doctypeDetector());
	if (!m_doctypeDetector.get())
	{
		setLastError( "no document type detector defined");
		return false;
	}
	return true;
}

CommandHandler::Operation DoctypeFilterCommandHandler::nextOperation()
{
	try
	{
		if (!m_doctypeDetector.get())
		{
			if (m_done || !createDoctypeDetector())
			{
				return CLOSE;
			}
		}
		if (m_doctypeDetector->run())
		{
			if (!m_doctypeDetector->info().get())
			{
				setLastError( "document format recongition did not come to a result");
			}
			else
			{
				LOG_DEBUG << "document recognized as '" << m_doctypeDetector->info()->docformat() << "'";
			}
			return CLOSE;
		}
		else
		{
			if (m_doctypeDetector->lastError())
			{
				setLastError( m_doctypeDetector->lastError());
				LOG_ERROR << "document type not recognized: " << m_doctypeDetector->lastError();
				return CLOSE;
			}
			else if (m_inputbuffer.size() > MaxDoctypeDetectionBlockSize)
			{
				LOG_ERROR << "document type detection has consumed more than " << (int)MaxDoctypeDetectionBlockSize << " bytes (" << m_inputbuffer.size() << ") without getting a result";
				setLastError( "document format not recognized");
				return CLOSE;
			}
			else
			{
				return READ;
			}
		}
	}
	catch (const std::runtime_error& err)
	{
		setLastError( "document format not recognized");
		LOG_ERROR << "exception in document type recognition: " << err.what();
		return CLOSE;
	}
}

void DoctypeFilterCommandHandler::putInput( const void *begin, std::size_t bytesTransferred)
{
	m_inputbuffer.append( (const char*)begin, bytesTransferred);
	if (!m_doctypeDetector.get())
	{
		if (!createDoctypeDetector())
		{
			m_done = true;
		}
	}
	if (m_doctypeDetector.get())
	{
		m_doctypeDetector->putInput( (const char*)begin, bytesTransferred);
	}
}

void DoctypeFilterCommandHandler::getInputBlock( void*& begin, std::size_t& maxBlockSize)
{
	begin = m_input.ptr();
	maxBlockSize = m_input.size();
}

void DoctypeFilterCommandHandler::getOutput( const void*& begin, std::size_t& bytesToTransfer)
{
	begin = 0;
	bytesToTransfer = 0;
}

void DoctypeFilterCommandHandler::getDataLeft( const void*& begin, std::size_t& nofBytes)
{
	begin = m_input.ptr();
	nofBytes = 0;
}

void DoctypeFilterCommandHandler::getInputBuffer( void*& begin, std::size_t& nofBytes)
{
	begin = (void*)const_cast<char*>( m_inputbuffer.c_str());
	nofBytes = m_inputbuffer.size();
}


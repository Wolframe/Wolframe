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
///\file cmdbind/contentBufferCommandHandlerEscDLF.hpp
#ifndef _Wolframe_cmdbind_CONTENT_BUFFER_COMMAND_HANDLER_ESC_DOT_LF_HPP_INCLUDED
#define _Wolframe_cmdbind_CONTENT_BUFFER_COMMAND_HANDLER_ESC_DOT_LF_HPP_INCLUDED
#include "protocol/ioblocks.hpp"
#include "cmdbind/commandHandler.hpp"
#include <string>

namespace _Wolframe {
namespace cmdbind {

///\class ContentBufferCommandHandlerEscDLF
class ContentBufferCommandHandlerEscDLF :public CommandHandler
{
public:
	typedef CommandHandler Parent;

	///\brief Constructor
	ContentBufferCommandHandlerEscDLF();
	///\brief Destructor
	virtual ~ContentBufferCommandHandlerEscDLF();

	///\brief See Parent::setInputBuffer(void*,std::size_t,std::size_t,std::size_t)
	virtual void setInputBuffer( void* buf, std::size_t allocsize);

	///\brief See Parent::setOutputBuffer(void*,std::size_t,std::size_t)
	virtual void setOutputBuffer( void* buf, std::size_t size, std::size_t pos);

	///\brief See Parent::nextOperation()
	virtual Operation nextOperation();

	///\brief See Parent::putInput(const void*,std::size_t);
	virtual void putInput( const void *begin, std::size_t bytesTransferred);

	///\brief See Parent::getInputBlock(void*&,std::size_t&)
	virtual void getInputBlock( void*& begin, std::size_t& maxBlockSize);

	///\brief See Parent::getOutput(const void*&,std::size_t&)
	virtual void getOutput( const void*& begin, std::size_t& bytesToTransfer);

	///\brief See Parent::getDataLeft(const void*&,std::size_t&)
	virtual void getDataLeft( const void*& begin, std::size_t& nofBytes);

	const std::string& content() const
	{
		return m_content;
	}

private:
	enum State
	{
		Processing,
		Terminated
	};
	static const char* stateName( State st)
	{
		static const char* ar[] = {"Processing","Terminated"};
		return ar[ (int)st];
	}

	protocol::EscapeBuffer m_escapeBuffer;
	State m_state;					//< processing state machine state
	std::string m_content;
	protocol::InputBlock m_input;			//< input buffer
	protocol::OutputBlock m_output;			//< output buffer
	protocol::InputBlock::iterator m_eoD;		//< input end of data marker
	std::size_t m_itrpos;				//< read start position in buffer for the command handler
};
}}
#endif


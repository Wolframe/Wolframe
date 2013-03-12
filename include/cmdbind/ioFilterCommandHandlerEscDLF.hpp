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
///\file cmdbind/ioFilterCommandHandlerEscDLF.hpp
#ifndef _Wolframe_cmdbind_IOFILTER_COMMAND_HANDLER_ESC_DOT_LF_HPP_INCLUDED
#define _Wolframe_cmdbind_IOFILTER_COMMAND_HANDLER_ESC_DOT_LF_HPP_INCLUDED
#include "protocol/ioblocks.hpp"
#include "filter/filter.hpp"
#include "cmdbind/ioFilterCommandHandler.hpp"
#include "connectionHandler.hpp"

namespace _Wolframe {
namespace cmdbind {

///\class IOFilterCommandHandlerEscDLF
///\brief Command handler processing filter input/output with end of content marked as Dot ('.') LF or Dot CR LF
///\remark Dot ('.') on start of lines are escaped with Dot Dot LF
class IOFilterCommandHandlerEscDLF :public IOFilterCommandHandler
{
public:
	typedef IOFilterCommandHandler Parent;

	///\brief Constructor
	IOFilterCommandHandlerEscDLF();
	///\brief Destructor
	virtual ~IOFilterCommandHandlerEscDLF();

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

	///\brief See Parent::interruptDataSessionMarker()
	virtual const char* interruptDataSessionMarker() const;

	///\param[out] err error code in case of error
	///\return CallResult status of the filter input for the state machine of this command handler
	virtual CallResult call( const char*& err)=0;

private:
	enum State
	{
		Processing,
		FlushingOutput,
		DiscardInput,
		Terminated
	};
	static const char* stateName( State st)
	{
		static const char* ar[] = {"Processing","FlushingOutput","DiscardInput","Terminated"};
		return ar[ (int)st];
	}

	protocol::EscapeBuffer m_escapeBuffer;

	State m_state;					//< processing state machine state
	const void* m_writedata;			//< bytes to write next (WRITE)
	std::size_t m_writedatasize;			//< number of bytes to write next (WRITE)
	unsigned int m_writedata_chksum;		//< check sum to verify write
	unsigned int m_writedata_chkpos;		//< check position to verify write

	protocol::InputBlock m_input;			//< input buffer
	protocol::OutputBlock m_output;			//< output buffer
	protocol::InputBlock::iterator m_eoD;		//< input end of data marker
	std::size_t m_itrpos;				//< read start position in buffer for the command handler

private:
	void getFilterOutputWriteData();
};
}}
#endif


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
///\file cmdbind/ioFilterCommandHandler.hpp
#ifndef _Wolframe_cmdbind_IOFILTER_COMMAND_HANDLER_HPP_INCLUDED
#define _Wolframe_cmdbind_IOFILTER_COMMAND_HANDLER_HPP_INCLUDED
#include "protocol.hpp"
#include "protocol/ioblocks.hpp"
#include "cmdbind/commandHandler.hpp"
#include "connectionHandler.hpp"

namespace _Wolframe {
namespace cmdbind {

///\class IOFilterCommandHandler
///\brief Command handler processing filter input/output
class IOFilterCommandHandler :public CommandHandler
{
public:
	typedef CommandHandler Parent;

	///\brief Constructor
	IOFilterCommandHandler();
	///\brief Destructor
	virtual ~IOFilterCommandHandler();

	void setFilter( const protocol::InputFilterR& in)
	{
		m_inputfilter = in;
	}

	void setFilter( const protocol::OutputFilterR& out)
	{
		m_outputfilter = out;
	}
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

	///\enum CallResult
	///\brief Enumeration of call states of this application processor instance
	enum CallResult
	{
		Ok,		//< successful termination of call
		Error,		//< termination of call with error (not completed)
		Yield		//< call interrupted with request for a network operation
	};
	static const char* callResultName( CallResult cr)
	{
		static const char* ar[] = {"Ok","Error","Yield"};
		return ar[ (int)cr];
	}

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

	State m_state;					//< processing state machine state
	const void* m_writedata;			//< bytes to write next (WRITE)
	std::size_t m_writedatasize;			//< number of bytes to write next (WRITE)

	protocol::InputBlock m_input;			//< input buffer
	protocol::InputBlock::iterator m_eoD;		//< input end of data marker
	std::size_t m_itrpos;				//< read start position in buffer for the command handler
protected:
	protocol::InputFilterR m_inputfilter;		//< network input interface for the interpreter
	protocol::OutputFilterR m_outputfilter;		//< network output interface for the interpreter
};
}}
#endif


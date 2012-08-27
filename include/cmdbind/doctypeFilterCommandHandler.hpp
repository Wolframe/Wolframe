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
///\file cmdbind/doctypeFilterCommandHandler.hpp
#ifndef _Wolframe_cmdbind_DOCTYPE_FILTER_COMMAND_HANDLER_HPP_INCLUDED
#define _Wolframe_cmdbind_DOCTYPE_FILTER_COMMAND_HANDLER_HPP_INCLUDED
#include "protocol/ioblocks.hpp"
#include "filter/filter.hpp"
#include "cmdbind/protocolCommandHandler.hpp"
#include "connectionHandler.hpp"

namespace _Wolframe {
namespace cmdbind {

///\class DoctypeFilterCommandHandler
///\brief Command handler for exctracting the document type identifier from a content
///\remark This is a pre-processing command handler. It is returning the consumed input as data left for following processing
class DoctypeFilterCommandHandler :public ProtocolCommandHandler
{
public:
	typedef CommandHandler Parent;

	///\brief Constructor
	DoctypeFilterCommandHandler();
	///\brief Destructor
	virtual ~DoctypeFilterCommandHandler();

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

	std::string doctypeid() const;

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
		Terminated,
		Done
	};
	static const char* stateName( State st)
	{
		static const char* ar[] = {"Processing","Terminated"};
		return ar[ (int)st];
	}

	protocol::EscapeBuffer m_escapeBuffer;

	State m_state;					//< processing state machine state

	protocol::InputBlock m_input;			//< input buffer
	protocol::InputBlock::iterator m_eoD;		//< input end of data marker
	std::size_t m_itrpos;				//< read start position in buffer for the command handler
	std::string m_inputbuffer;			//< buffer for consumed input (is returned to caller because this is a preprocessing command handler)
	std::string m_doctypeid;			//< document type identifier extracted

protected:
	langbind::InputFilterR m_inputfilter;		//< network input interface for this command handler
};
}}
#endif


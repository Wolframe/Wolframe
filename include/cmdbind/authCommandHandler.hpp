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
/// \file authCommandHandler.hpp
/// \brief Interface of the authentication command handler

#ifndef _Wolframe_AUTH_COMMAND_HANDLER_HPP_INCLUDED
#define _Wolframe_AUTH_COMMAND_HANDLER_HPP_INCLUDED

#include "cmdbind/lineCommandHandler.hpp"
#include "AAAA/authenticator.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace cmdbind {

/// \class AuthCommandHandler
/// \brief Command handler for the sub protocol for authentication
class AuthCommandHandler
	:public cmdbind::CommandHandler
{
public:
	explicit AuthCommandHandler( const boost::shared_ptr<AAAA::Authenticator>& authenticator_);
	virtual ~AuthCommandHandler();

	const AAAA::User& user() const
	{
		static const AAAA::User undefined;
		return undefined;
		//[+] if (!m_authenticator) return undefined;
		//[+] AAAA::User* usr = m_authenticator->user();
		//[+] return usr?*usr:undefined;
	}

	/// \brief See CommandHandler::setInputBuffer(void*,std::size_t,std::size_t,std::size_t)
	virtual void setInputBuffer( void* buf, std::size_t allocsize);

	/// \brief See CommandHandler::setOutputBuffer(void*,std::size_t,std::size_t)
	virtual void setOutputBuffer( void* buf, std::size_t size, std::size_t pos);

	/// \brief See CommandHandler::nextOperation()
	virtual Operation nextOperation();

	/// \brief See CommandHandler::putInput(const void*,std::size_t);
	virtual void putInput( const void *begin, std::size_t bytesTransferred);

	/// \brief See CommandHandler::getInputBlock(void*&,std::size_t&)
	virtual void getInputBlock( void*& begin, std::size_t& maxBlockSize);

	/// \brief See CommandHandler::getOutput(const void*&,std::size_t&)
	virtual void getOutput( const void*& begin, std::size_t& bytesToTransfer);

	/// \brief See CommandHandler::getDataLeft(const void*&,std::size_t&)
	virtual void getDataLeft( const void*& begin, std::size_t& nofBytes);

	/// \brief See CommandHandler::interruptDataSessionMarker()
	virtual const char* interruptDataSessionMarker() const;

private:
	/// \enum State
	/// \brief Enumeration of processor states
	enum State
	{
		Init,				///< start state, called first time in this session
		NextOperation,			///< running a command
		ReadConsumed,			///< state set by putInput for guarantee that pusInput message is valid till net call of AAAA::Authenticator::nextOperation()
		FlushOutput			///< flush network output
	};
	/// \brief Returns the state as string for logging etc.
	/// \param [in] i state to get as string
	static const char* stateName( State i)
	{
		static const char* ar[] = {
			"Init",
			"NextOperation",
			"ReadConsumed",
			"FlushOutput"
		};
		return ar[i];
	}
	boost::shared_ptr<AAAA::Authenticator> m_authenticator;	///< authenticator object reference
	protocol::InputBlock m_input;				///< protocol input buffer
	protocol::InputBlock::iterator m_eoD;			///< input end of data marker
	std::size_t m_itrpos;					///< read start position in buffer for the command handler

	char* m_outputbuf;					///< protocol output buffer
	std::size_t m_outputbufsize;				///< protocol output buffer allocation size
	std::size_t m_outputbufpos;				///< protocol output buffer start of data chunk

	State m_state;						///< processing state of the command handler
	std::string m_readbuffer;				///< buffer for chunkwise network read
// MBa: temporary, avoid not used error
//	std::size_t m_readpos;					///< current position in buffer for chunkwise network read
	std::string m_writebuffer;				///< buffer for chunkwise network write
	std::size_t m_writepos;					///< current position in buffer for chunkwise network write
};

}} //namespace

#endif


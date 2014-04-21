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
#include "AAAA/"

namespace _Wolframe {
namespace AAAA {

/// \class Authenticator
/// \brief Interface for the authentication global mechanism
class Authenticator
{
public:
	typedef _Wolframe::FSM::Operation::FSMoperation Operation;

	struct Message
	{
		std::size_t size;
		const char* ptr;

		Message()
			:size(0),ptr(0){}
		Message( const Message& o)
			:size(o.size),ptr(o.ptr){}
		Message( const char* ptr_, std::size_t size_)
			:size(size_),ptr(ptr_){}
	};

public:
	/// \brief Destructor
	virtual ~Authenticator(){}

	/// \brief Initialize statemachine before execution
	virtual void init(){};

	/// \brief Get the next operation
	virtual Operation nextOperation()=0;

	/// \brief Get the message of a WRITE operation
	virtual Message getMessage()=0;

	/// \brief Put the answer of a READ request operation
	virtual void putMessage( const Message& msg)=0;

	/// \brief Close the authenticator and destroy all sensible data
	virtual void close(){}
};
}//namespace AAAA

namespace cmdbind {

/// \class AuthCommandHandler
/// \brief Command handler for the sub protocol for authentication
class AuthCommandHandler
	:public cmdbind::CommandHandler
{
public:
	explicit AuthCommandHandler( AAAA::Authenticator* authenticator_);
	virtual ~AuthCommandHandler();

	const AAAA::User& user() const
	{
		static const AAAA::User undefined;
		if (!m_authenticator) return undefined;
		AAAA::User* usr = m_authenticator->user();
		return usr?*usr:undefined;
	}

	///\brief See CommandHandler::setInputBuffer(void*,std::size_t,std::size_t,std::size_t)
	virtual void setInputBuffer( void* buf, std::size_t allocsize);

	///\brief See CommandHandler::setOutputBuffer(void*,std::size_t,std::size_t)
	virtual void setOutputBuffer( void* buf, std::size_t size, std::size_t pos);

	///\brief See CommandHandler::nextOperation()
	virtual Operation nextOperation();

	///\brief See CommandHandler::putInput(const void*,std::size_t);
	virtual void putInput( const void *begin, std::size_t bytesTransferred);

	///\brief See CommandHandler::getInputBlock(void*&,std::size_t&)
	virtual void getInputBlock( void*& begin, std::size_t& maxBlockSize);

	///\brief See CommandHandler::getOutput(const void*&,std::size_t&)
	virtual void getOutput( const void*& begin, std::size_t& bytesToTransfer);

	///\brief See CommandHandler::getDataLeft(const void*&,std::size_t&)
	virtual void getDataLeft( const void*& begin, std::size_t& nofBytes);

	///\brief See CommandHandler::interruptDataSessionMarker()
	virtual const char* interruptDataSessionMarker() const;

private:
	void getOutputWriteData();

private:
	/// \enum State
	/// \brief Enumeration of processor states
	enum State
	{
		Init,				///< start state, called first time in this session
		NextOperation,			///< running a command
		FlushOutput,			///< flush network output
		Terminate			///< terminate application processor session
	};
	/// \brief Returns the state as string for logging etc.
	/// \param [in] i state to get as string
	static const char* stateName( State i)
	{
		static const char* ar[] = {
			"Init",
			"NextOperation",
			"FlushOutput"
			"Terminate"
		};
		return ar[i];
	}
	protocol::EscapeBuffer m_escapeBuffer;

	State m_state;				///< processing state of the command handler
	const void* m_writedata;		///< bytes to write next (WRITE)
	std::size_t m_writedatasize;		///< number of bytes to write next (WRITE)
	unsigned int m_writedata_chksum;	///< check sum to verify write
	unsigned int m_writedata_chkpos;	///< check position to verify write

	AAAA:Authenticator* m_authenticator;	///< instance of authenticator owned by this
	protocol::InputBlock m_input;		///< buffer for network read messages
	protocol::OutputBlock m_output;		///< buffer for network write messages
	protocol::InputBlock::iterator m_eoD;	///< input end of data marker
	std::size_t m_itrpos;			///< read start position in buffer for the command handler
	std::string m_readbuffer;		///< buffer for message read
	std::string m_writebuffer;		///< buffer for message written
	std::size_t m_writepos;			///< position of cosumption of content of m_writebuffer
};

}} //namespace

#endif


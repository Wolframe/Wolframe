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
///\file langbind/ioFilterCommandHandler.hpp
#ifndef _Wolframe_IOFILTER_COMMAND_HANDLER_HPP_INCLUDED
#define _Wolframe_IOFILTER_COMMAND_HANDLER_HPP_INCLUDED
#include "protocol.hpp"
#include "protocol/commandHandler.hpp"
#include "connectionHandler.hpp"

namespace _Wolframe {
namespace protocol {

class IOFilterCommandHandler :public CommandHandler
{
public:
	typedef CommandHandler Parent;

	IOFilterCommandHandler();

	void setFilter( const protocol::InputFilterR& in)
	{
		m_inputfilter = in;
	}
	void setFilter( const protocol::FormatOutputR& out)
	{
		m_formatoutput = out;
	}

	///\brief Pass the IO for processing (See Parent::passIO(const InputBlock&,const OutputBlock&))
	///\param [in] input network input interface
	///\param [in] output network output interface
	virtual void passIO( const InputBlock& input, const OutputBlock& output);

	///\brief Passes the network input to the command handler (See Parent::putInput(const void*,std::size_t))
	///\param [in] begin start of the network input block.
	///\param [in] bytesTransferred number of bytes passed in the input block
	virtual void putInput( const void *begin, std::size_t bytesTransferred);

	///\brief Get the next output chunk from the command handler (See Parent::getOutput(const void*&,std::size_t&))
	///\param [out] begin start of the output chunk
	///\param [out] bytesToTransfer size of the output chunk to send in bytes
	virtual void getOutput( const void*& begin, std::size_t& bytesToTransfer);

	///\enum CallResult
	///\brief Enumeration of call states of this application processor instance
	enum CallResult
	{
		Ok,		///< successful termination of call
		Error,		///< termination of call with error (not completed)
		YieldRead,	///< call interrupted with request for more network input
		YieldWrite	///< call interrupted with request for sending data from the write buffer that is full
	};
	virtual CallResult call()=0;

	virtual void run()
	{
		switch (call())
		{
			case Ok: terminate( 0); break;
			case Error: terminate( 1); break;
			case YieldRead: break;
			case YieldWrite: flushOutput(); break;
		}
	}
protected:
	protocol::InputFilterR m_inputfilter;		///< network input interface for the interpreter
	protocol::FormatOutputR m_formatoutput;		///< network output interface for the interpreter
};
}}
#endif


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
///\file wolfilter/src/echo_cmdhandler.hpp
///\brief Interface wolfilter test command handler
#ifndef _Wolframe_TESTS_WOLFILTER_echo_cmdhandler_HPP_INCLUDED
#define _Wolframe_TESTS_WOLFILTER_echo_cmdhandler_HPP_INCLUDED
#include <string>
#include "cmdbind/commandHandler.hpp"
#include "protocol/ioblocks.hpp"

namespace _Wolframe {
namespace test {

class EchoCommandHandler :public cmdbind::CommandHandler
{
public:
	///\brief Default constructor
	EchoCommandHandler();

	///\brief Destructor
	virtual ~EchoCommandHandler(){}

	///\brief Implement CommandHandler::setInputBuffer(void*,std::size_t)
	virtual void setInputBuffer( void* buf, std::size_t allocsize);

	///\brief Implement CommandHandler::setOutputBuffer(void*,std::size_t,std::size_t)
	virtual void setOutputBuffer( void* buf, std::size_t size, std::size_t pos);

	///\brief Implement CommandHandler::nextOperation()
	virtual Operation nextOperation();

	///\brief Implement CommandHandler::putInput(const void*,std::size_t)
	virtual void putInput( const void* begin, std::size_t bytesTransferred);

	///\brief Implement CommandHandler::getInputBlock( void*&,std::size_t&)
	virtual void getInputBlock( void*& begin, std::size_t& maxBlockSize);

	///\brief Implement CommandHandler::getOutput(const void*&,std::size_t&)
	virtual void getOutput( const void*& begin, std::size_t& bytesToTransfer);

	///\brief Implement CommandHandler::getDataLeft(const void*&,std::size_t&)
	virtual void getDataLeft( const void*& begin, std::size_t& nofBytes);

protected:
	virtual bool executeCommand( char cmd, const std::string& arg);

	virtual void pushResult( char cmd, const std::string& arg);

private:
	void flushOutput();

	enum State
	{
		Init,
		ParseCommand,
		Process,
		FlushOutput,
		OnTerminate
	};
private:
	State m_state;
	protocol::InputBlock m_in;
	protocol::OutputBlock m_out;
	std::string m_inputline;
	std::string m_outputline;
	std::size_t m_outputitr;
	std::size_t m_inpos;
	char m_cmd;
};

}}//namespace
#endif


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
/// \file ioFilterCommandHandler.cpp
/// \brief Command handler base class for processing content and writing output through filters

#ifndef _Wolframe_IOFILTER_COMMAND_HANDLER_BASE_HPP_INCLUDED
#define _Wolframe_IOFILTER_COMMAND_HANDLER_BASE_HPP_INCLUDED
#include "filter/filter.hpp"
#include "langbind/input.hpp"
#include "langbind/output.hpp"
#include "cmdbind/commandHandler.hpp"
#include "system/connectionHandler.hpp"

namespace _Wolframe {
namespace cmdbind {

/// \class IOFilterCommandHandler
/// \brief Abstract class for command handler processing filter input/output
class IOFilterCommandHandler
	:public CommandHandler
{
public:
	/// \brief Constructor
	explicit IOFilterCommandHandler()
		:m_writeptr(0)
		,m_writesize(0)
		,m_done(false){}
	/// \brief Constructor
	IOFilterCommandHandler( const std::string& docformat_)
		:m_input(docformat_)
		,m_writeptr(0)
		,m_writesize(0)
		,m_done(false){}

	/// \brief Destructor
	virtual ~IOFilterCommandHandler(){}

	/// \brief Define the input filter to use (makes an own copy of it)
	void setInputFilter( const langbind::InputFilterR& in);

	/// \brief Define the output filter to use (makes an own copy of it)
	void setOutputFilter( const langbind::OutputFilterR& out);

	/// \brief Get a reference to the input filter
	langbind::InputFilterR inputfilter()
	{
		return m_input.inputfilter();
	}

	/// \brief Get a const reference to the output object
	const langbind::Output& output()
	{
		return m_output;
	}

	/// \brief Get a const reference to the input object
	const langbind::Input& input()
	{
		return m_input;
	}

	/// \brief Get a reference to the output filter
	langbind::OutputFilterR outputfilter()
	{
		return m_output.outputfilter();
	}

	/// \brief Get the next operation to do for the connection handler
	/// \return the next operation for the connection handler
	virtual Operation nextOperation();

	/// \brief Passes the network input to the command handler (READ operation)
	/// \param [in] begin start of the network input block.
	/// \param [in] bytesTransferred number of bytes passed in the input block
	/// \param [in] eod (end of data) true, if the passed chunk is the last one
	virtual void putInput( const void* begin, std::size_t bytesTransferred, bool eod);

	/// \brief Get the next output chunk from the command handler (WRITE operation)
	/// \param [out] begin start of the output chunk
	/// \param [out] bytesToTransfer size of the output chunk to send in bytes
	virtual void getOutput( const void*& begin, std::size_t& bytesToTransfer);

	/// \enum CallResult
	/// \brief Enumeration of call states of this application processor instance
	enum CallResult
	{
		Ok,		//< successful termination of call
		Error,		//< termination of call with error (not completed)
		Yield		//< call interrupted with request for a network operation
	};

	/// \brief Get an enumeration value of call states as string
	static const char* callResultName( CallResult cr)
	{
		static const char* ar[] = {"Ok","Error","Yield"};
		return ar[ (int)cr];
	}

	/// \brief Hook to call function to execute
	/// \param[out] err error code in case of error
	/// \return CallResult status of the filter input for the state machine of this command handler
	virtual CallResult call( const char*& err)=0;

	/// \brief Overloaded setter of the size of output chunk, setting it also in output
	virtual void setOutputChunkSize( std::size_t outputChunkSize_)
	{
		CommandHandler::setOutputChunkSize( outputChunkSize_);
		m_output.setOutputChunkSize( outputChunkSize_);
	}

private:
	langbind::Input m_input;			///< Input interface for this command handler
	langbind::Output m_output;			///< Output interface for this command handler
	const void* m_writeptr;				///< What to write next
	std::size_t m_writesize;			///< Size of what to write next in bytes
	bool m_done;					///< Terminated
};
}}
#endif


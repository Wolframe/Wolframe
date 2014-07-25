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
	IOFilterCommandHandler()
		:m_gotEoD(false){}

	/// \brief Destructor
	virtual ~IOFilterCommandHandler(){}

	void setFilter( const langbind::InputFilterR& in)
	{
		m_inputfilter = in;
	}

	void setFilter( const langbind::OutputFilterR& out)
	{
		m_outputfilter = out;
	}

	langbind::InputFilterR inputfilter()
	{
		return m_inputfilter;
	}

	langbind::OutputFilterR outputfilter()
	{
		return m_outputfilter;
	}

	void setFilterAs( const langbind::InputFilterR& in);

	void setFilterAs( const langbind::OutputFilterR& out);

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

	static const char* callResultName( CallResult cr)
	{
		static const char* ar[] = {"Ok","Error","Yield"};
		return ar[ (int)cr];
	}

	/// \param[out] err error code in case of error
	/// \return CallResult status of the filter input for the state machine of this command handler
	virtual CallResult call( const char*& err)=0;


protected:
	langbind::InputFilterR m_inputfilter;		///< input interface for this command handler
	langbind::OutputFilterR m_outputfilter;		///< output interface for this command handler
	bool m_gotEoD;					///< true, if we got end of data
};
}}
#endif


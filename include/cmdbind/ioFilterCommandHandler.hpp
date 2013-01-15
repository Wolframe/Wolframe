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
///\file cmdbind/ioFilterCommandHandlerBase.hpp
#ifndef _Wolframe_cmdbind_IOFILTER_COMMAND_HANDLER_BASE_HPP_INCLUDED
#define _Wolframe_cmdbind_IOFILTER_COMMAND_HANDLER_BASE_HPP_INCLUDED
#include "protocol/ioblocks.hpp"
#include "filter/filter.hpp"
#include "cmdbind/commandHandler.hpp"
#include "connectionHandler.hpp"

namespace _Wolframe {
namespace cmdbind {

///\class IOFilterCommandHandler
///\brief Abstract class for command handler processing filter input/output
class IOFilterCommandHandler :public CommandHandler
{
public:
	///\brief Constructor
	IOFilterCommandHandler(){}
	///\brief Destructor
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

	void setFilterAs( const langbind::InputFilterR& in)
	{
		// assign the rest of the input to the new filter attached
		const void* chunk;
		std::size_t chunksize;
		bool chunkend;
		m_inputfilter->getRest( chunk, chunksize, chunkend);
		m_inputfilter.reset( in->copy());
		m_inputfilter->putInput( chunk, chunksize, chunkend);
	}

	void setFilterAs( const langbind::OutputFilterR& out)
	{
		langbind::OutputFilter* of = out->copy();
		of->assignState( *m_outputfilter);
		m_outputfilter.reset( of);;
	}

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

protected:
	langbind::InputFilterR m_inputfilter;		//< input interface for this command handler
	langbind::OutputFilterR m_outputfilter;		//< output interface for this command handler
};
}}
#endif


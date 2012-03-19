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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///
///\file appProcessor.hpp
///\brief the application processor interface
///
#ifndef _Wolframe_APPLICATION_PROCESSOR_HPP_INCLUDED
#define _Wolframe_APPLICATION_PROCESSOR_HPP_INCLUDED

namespace _Wolframe {
namespace app {

///\class AppProcessorBase
///\brief Abstract base class for the application processor interface representing one instance
class AppProcessorBase
{
public:
	///\brief Constructor
	AppProcessorBase(){}
	///\brief Destructor
	virtual ~AppProcessorBase(){}

	///\enum CallResult
	///\brief Enumeration of call states of this application processor instance
	enum CallResult
	{
		Ok,		///< successful termination of call
		Error,		///< termination of call with error (not completed)
		YieldRead,	///< call interrupted with request for more network input
		YieldWrite	///< call interrupted with request for sending data from the write buffer that is full
	};

	///\brief Get the application processor script function to execute for a protocol command (as defined in configuration)
	///\param[in] protocolCmd protocol command
	///\param[out] functionName name of application processor script function to execute
	///\param[out] hasIO true, if the application processor script function processes data from network input, false else
	///\return true, if the command exists
	virtual bool getCommand( const char* protocolCmd, const char*& functionName, bool& hasIO) const=0;

	///\brief Define input and output interface of the application processor
	///\param[in] in input filter reference
	///\param[in] out output filter reference
	virtual void setIO( const protocol::InputFilterR& in, const protocol::OutputFilterR& out)=0;

	///\brief Function call of an application processor script function
	///\param[in] argc number of arguments inclusing the script function name as first argument
	///\param[in] argv array of arguments inclusing the script function name as first argument
	///\return application processor instance call state
	virtual CallResult call( unsigned int argc, const char** argv)=0;
};

}}//namespace
#endif


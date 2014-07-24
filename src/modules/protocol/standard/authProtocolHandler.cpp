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
/// \file authProtocolHandler.cpp
/// \brief Implementation of the authentication command handler

#include "authProtocolHandler.hpp"

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;

AuthProtocolHandler::AuthProtocolHandler( const boost::shared_ptr<AAAA::Authenticator>& authenticator_)
	:m_authenticator(authenticator_)
{}

AuthProtocolHandler::~AuthProtocolHandler()
{
}

void AuthProtocolHandler::processMessage( const std::string& msg)
{
	m_authenticator->messageIn( msg);
}

ProtocolHandler::Operation AuthProtocolHandler::nextOperation()
{
	for (;;)
	{
		LOG_TRACE << "STATE AuthProtocolHandler " << stateName( state()) << " " << m_authenticator->statusName( m_authenticator->status());
		switch (state())
		{
			case Init:
				setState( NextOperation);
				/*no break here!*/

			case NextOperation:
				switch (m_authenticator->status())
				{
					case AAAA::Authenticator::INITIALIZED:
						throw std::logic_error("authentication protocol operation in state INITIALIZED");
					case AAAA::Authenticator::MESSAGE_AVAILABLE:
						pushOutput( m_authenticator->messageOut());
						continue;
					case AAAA::Authenticator::AWAITING_MESSAGE:
						if (!consumeNextMessage())
						{
							return READ;
						}
						continue;
					case AAAA::Authenticator::AUTHENTICATED:
						return CLOSE;
					case AAAA::Authenticator::INVALID_CREDENTIALS:
						setLastError( "either the username or the credentials are invalid");
						return CLOSE;
					case AAAA::Authenticator::MECH_UNAVAILABLE:
						setLastError( "the requested authentication mech is not available");
						return CLOSE;
					case AAAA::Authenticator::SYSTEM_FAILURE:
						setLastError( "unspecified authentication system error");
						return CLOSE;
				}
				setLastError( "internal: unhandled authenticator status");
				return CLOSE;

			case FlushOutput:
				if (endOfOutput())
				{
					setState( NextOperation);
					continue;
				}
				return WRITE;
		}
	}//for(;;)
	setLastError( "internal: unhandled state in authentication protocol");
	return CLOSE;
}




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
//
// Authentication factory implementation
//

#include <stdexcept>

#include "AAAAproviderImpl.hpp"
#include "logger-v1.hpp"
#include "boost/algorithm/string.hpp"
#include "module/moduleDirectory.hpp"

namespace _Wolframe {
namespace AAAA {

AuthenticationFactory::AuthenticationFactory( const std::list< config::NamedConfiguration* >& confs,
					  const module::ModulesDirectory* modules )
{
	for ( std::list<config::NamedConfiguration*>::const_iterator it = confs.begin();
							it != confs.end(); it++ )	{
		module::ConfiguredBuilder* builder = modules->getBuilder((*it)->className());
		if ( builder )	{
			ConfiguredObjectConstructor< AuthenticationUnit >* auth =
					dynamic_cast< ConfiguredObjectConstructor< AuthenticationUnit >* >( builder->constructor());
			if ( auth == NULL )	{
				LOG_ALERT << "AuthenticationFactory: '" << builder->objectClassName()
					  << "' is not an Authentication Unit builder";
				throw std::logic_error( "object is not an AuthenticationUnit builder" );
			}
			m_authenticators.push_back( auth->object( **it ));
			LOG_TRACE << "'" << auth->objectClassName() << "' authentication unit registered";
		}
		else	{
			LOG_ALERT << "AuthenticationFactory: unknown authentication type '" << (*it)->className() << "'";
			throw std::domain_error( "Unknown authentication mechanism type in AAAAprovider constructor. See log" );
		}
	}
}

AuthenticationFactory::~AuthenticationFactory()
{
	for ( std::list< AuthenticationUnit* >::const_iterator it = m_authenticators.begin();
								it != m_authenticators.end(); it++ )
		delete *it;
}

bool AuthenticationFactory::resolveDB( const db::DatabaseProvider& db )
{
	for ( std::list< AuthenticationUnit* >::const_iterator it = m_authenticators.begin();
								it != m_authenticators.end(); it++ )
		if ( ! (*it)->resolveDB( db ) )
			return false;
	return true;
}


Authenticator* AuthenticationFactory::authenticator()
{
	return new StandardAuthenticator();
}


//*********************************************************************************
// Standard authenticator
StandardAuthenticator::StandardAuthenticator()
{
}

StandardAuthenticator::~StandardAuthenticator()
{
}

void StandardAuthenticator::close()
{
	//[PF:REMARK 'delete this' prevents use in different context] delete this;
}

// The FSM interface functions
// The input data.
void StandardAuthenticator::receiveData( const void* /*data*/, std::size_t /*size*/ )
{
}

// What should be done next.
const FSM::Operation StandardAuthenticator::nextOperation()
{
	FSM::Operation	op;
	return op;
}

// signal the FSM
void StandardAuthenticator::signal( FSM::Signal /*event*/ )
{
}

// Data not consumed.
std::size_t StandardAuthenticator::dataLeft( const void*& /*begin*/ )
{
	return 0;
}

}} // namespace _Wolframe::AAAA

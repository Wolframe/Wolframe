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
//
// AAAA provider
//

#ifndef _AAAA_PROVIDER_HPP_INCLUDED
#define _AAAA_PROVIDER_HPP_INCLUDED

#include "config/configurationBase.hpp"
#include "auditor.hpp"
#include "authenticator.hpp"
#include "authentication.hpp"
#include "database/database.hpp"

#include <string>
#include <list>

namespace _Wolframe {
namespace AAAA {

class AAAAconfiguration : public config::ConfigurationBase
{
	friend class AAAAprovider;
	friend class config::ConfigurationParser;
public:
	/// constructor
	AAAAconfiguration();

	/// methods
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );
private:
	AuthenticationConfiguration	auth;
	AuditConfiguration		audit;
};


class AAAAprovider
{
public:
	AAAAprovider( const AAAAconfiguration& conf );
	~AAAAprovider();
	bool resolveDB( db::DatabaseProvider& db );

	Authenticator* authenticator() const		{ return NULL; }
private:
	std::list<AuthenticatorBase*>	m_authenticators;
	std::list<AuditorBase*>		m_auditors;
};

}} // namespace _Wolframe::AAAA

#endif // _AAAA_PROVIDER_HPP_INCLUDED

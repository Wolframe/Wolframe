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
// AAAA provider interface
//

#ifndef _AAAA_PROVIDER_HPP_INCLUDED
#define _AAAA_PROVIDER_HPP_INCLUDED

#include "authentication.hpp"
#include "authorization.hpp"
#include "audit.hpp"
#include "config/configurationBase.hpp"
#include <boost/noncopyable.hpp>

namespace _Wolframe {
namespace AAAA {

class AAAAconfiguration : public config::ConfigurationBase
{
	friend class AAAAprovider;
public:
	/// x-structor
	AAAAconfiguration();
	~AAAAconfiguration();

	/// methods
	bool parse( const config::ConfigurationNode& pt, const std::string& node,
		    const module::ModulesDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );
private:
	std::string					m_randomDevice;
	std::list< config::NamedConfiguration* >	m_authConfig;
	bool						m_authzDefault;
	std::list< config::NamedConfiguration* >	m_authzConfig;
	bool						m_mandatoryAudit;
	std::list< config::NamedConfiguration* >	m_auditConfig;
};

class AAAAprovider : public boost::noncopyable
{
public:
	AAAAprovider( const AAAAconfiguration* conf,
		      const module::ModulesDirectory* modules );
	~AAAAprovider();

	bool resolveDB( const db::DatabaseProvider& db );

	Authenticator* authenticator();
	Authorizer* authorizer() const;
	Auditor* auditor() const;
private:
	class AAAAprovider_Impl;
	AAAAprovider_Impl*	m_impl;
};

}} // namespace _Wolframe::AAAA

#endif // _AAAA_PROVIDER_HPP_INCLUDED

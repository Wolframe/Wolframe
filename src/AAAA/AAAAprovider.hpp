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

#include "configurationBase.hpp"
#include <string>

namespace _Wolframe {
namespace AAAA {

	class User
	{
	public:
		User();
	};


	/// Authentication type
	enum AuthenticationType	{
		AUTH_UNKNOWN
	};


	struct	AuthenticationConfigBase
	{
		const AuthenticationType	type;
	public:
		AuthenticationConfigBase( AuthenticationType Type )	: type( Type )	{}
	//	virtual ~DatabaseConfigBase();

		virtual bool parse( const std::string& module, const boost::property_tree::ptree& pt ) = 0;
		virtual bool check( const std::string& module ) const = 0;
		virtual void print( std::ostream& os ) const = 0;
		virtual void setCanonicalPathes( const std::string& /* referencePath */ )	{}
	};


	class AuthenticationConfiguration : public _Wolframe::config::ConfigurationBase
	{
	public:
		/// constructor
		AuthenticationConfiguration();

		/// methods
		bool parse( const boost::property_tree::ptree& pt, const std::string& node );
		bool check() const;
		void print( std::ostream& os ) const;
		void setCanonicalPathes( const std::string& referencePath );

		// bool test() const;	// Not implemented yet, inherited from base
	};

	class Authenticator
	{
	public:
//		Authenticator( AuthenticatorConfiguration& conf );
	};


	class Authorizer
	{
	public:
	};


	class AuditConfiguration : public _Wolframe::config::ConfigurationBase
	{
	public:
		/// constructor
		AuditConfiguration();

		/// methods
		bool parse( const boost::property_tree::ptree& pt, const std::string& node );
		bool check() const;
		void print( std::ostream& os ) const;
		void setCanonicalPathes( const std::string& referencePath );

		// bool test() const;	// Not implemented yet, inherited from base
	};

	class Auditor
	{
	public:
//		Auditor( AuditorConfiguration& conf );
	};


	class Configuration : public _Wolframe::config::ConfigurationBase
	{
	public:
		/// constructor
		Configuration();

		/// methods
		bool parse( const boost::property_tree::ptree& pt, const std::string& node );
		bool check() const;
		void print( std::ostream& os ) const;
		void setCanonicalPathes( const std::string& referencePath );

		// bool test() const;	// Not implemented yet, inherited from base
	};

	class AAAAprovider
	{
	public:
		AAAAprovider( const Configuration& conf );

		const Authenticator& authenticator()	{ return authenticator_; }
		const Authorizer& authorizer()		{ return authorizer_; }
		const Auditor& auditor()		{ return auditor_; }
	private:
		const Authenticator	authenticator_;
		const Authorizer	authorizer_;
		const Auditor		auditor_;
	};

}} // namespace _Wolframe::AAAA

#endif // _AAAA_PROVIDER_HPP_INCLUDED

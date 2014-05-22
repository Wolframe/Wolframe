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
// text file authentication
//

#ifndef _SASL_AUTHENTICATION_HPP_INCLUDED
#define _SASL_AUTHENTICATION_HPP_INCLUDED

#include "AAAA/authUnit.hpp"
#include "module/constructor.hpp"

#include <string>
#include "sasl/sasl.h"

namespace _Wolframe {
namespace AAAA {

static const char* SASL_AUTHENTICATION_CLASS_NAME = "SASLAuth";

class SaslAuthConfig :  public config::NamedConfiguration
{
	friend class SaslAuthConstructor;
public:
	SaslAuthConfig( const char* cfgName, const char* logParent, const char* logName )
		: config::NamedConfiguration( cfgName, logParent, logName ) {}

	virtual const char* className() const	{ return SASL_AUTHENTICATION_CLASS_NAME; }

	/// methods
	bool parse( const config::ConfigurationNode& pt, const std::string& node,
		    const module::ModulesDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );
private:
	std::string		m_identifier;
	std::string		m_service;
	std::string		m_confPath;
};


class SaslAuthUnit : public AuthenticationUnit
{
public:
	SaslAuthUnit( const std::string& Identifier,
			   const std::string& service, const std::string& confpath );
	~SaslAuthUnit();
	virtual const char* className() const	{ return SASL_AUTHENTICATION_CLASS_NAME; }

	const std::string* mechs() const	{ return m_mechs; }

	AuthenticatorInstance* instance( const std::string& /*mech*/ )	{ return NULL; }

private:
	static const std::string	m_mechs[];	///<	list of mechs for the unit
	const std::string		m_service;	///<	registered name of the service,
							///	should maybe be fixed (or default to) 'wolframe'
	const std::string		m_confPath;	///<	a SASL configuration path for optional config
							///	(overridding system-wide one)
};


class SaslAuthConstructor : public ConfiguredObjectConstructor< AuthenticationUnit >
{
public:
	virtual ObjectConstructorBase::ObjectType objectType() const
						{ return AUTHENTICATION_OBJECT; }
	virtual const char* objectClassName() const	{ return SASL_AUTHENTICATION_CLASS_NAME; }
	virtual SaslAuthUnit* object( const config::NamedConfiguration& conf );
};

}} // namespace _Wolframe::AAAA

#endif // _TEXT_FILE_AUTHENTICATION_HPP_INCLUDED

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

	const char** mechs() const;

	AuthenticatorSlice* slice( const std::string& /*mech*/,
				   const net::RemoteEndpoint& /*client*/ );

	User* authenticatePlain( const std::string& username, const std::string& password ) const;

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

class SaslAuthSlice : public AuthenticatorSlice
{
	enum	SliceState	{
		SLICE_INITIALIZED = 0,		///< Has been initialized, no other data
		SLICE_ASK_FOR_PASSWORD,		///< Ask for password, send 'password?' to the client
		SLICE_WAITING_FOR_PWD,		///< We have sent 'password?' and wait for an answer
		SLICE_USER_NOT_FOUND,		///< User has not been found -> fail
		SLICE_INVALID_CREDENTIALS,	///< Response was wrong -> fail
		SLICE_AUTHENTICATED,		///< Response was correct -> user available
		SLICE_SYSTEM_FAILURE		///< Something is wrong
	};

public:
	SaslAuthSlice( const SaslAuthUnit& backend );

	~SaslAuthSlice();

	void dispose();

	virtual const char* className() const		{ return m_backend.className(); }

	virtual const std::string& identifier() const	{ return m_backend.identifier(); }

	/// The input message
	virtual void messageIn( const std::string& message );

	/// The output message
	virtual std::string messageOut();

	/// The current status of the authenticator slice
	virtual Status status() const;

	/// Is the last input message reusable for this mech ?
	virtual bool inputReusable() const		{ return m_inputReusable; }

	/// The authenticated user or NULL if not authenticated
	virtual User* user();

private:
	const SaslAuthUnit&	m_backend;
//	SliceState		m_state;
	bool			m_inputReusable;
	std::string		m_user;
};

}} // namespace _Wolframe::AAAA

#endif // _TEXT_FILE_AUTHENTICATION_HPP_INCLUDED

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
// PAM authentication
//

#ifndef _PAM_AUTHENTICATION_HPP_INCLUDED
#define _PAM_AUTHENTICATION_HPP_INCLUDED

#include <string>
#include "AAAA/authUnit.hpp"
#include "module/constructor.hpp"

#include <security/pam_appl.h>

namespace _Wolframe {
namespace AAAA {

static const char* PAM_AUTHENTICATION_CLASS_NAME = "PAMAuth";

class PAMAuthConfig :  public config::NamedConfiguration
{
	friend class PAMAuthConstructor;
public:
	PAMAuthConfig( const char* cfgName, const char* logParent, const char* logName )
		: config::NamedConfiguration( cfgName, logParent, logName ) {}

	virtual const char* className() const	{ return PAM_AUTHENTICATION_CLASS_NAME; }

	/// methods
	bool parse( const config::ConfigurationNode& pt, const std::string& node,
		    const module::ModulesDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
private:
	std::string	m_identifier;
	std::string	m_service;
};

// the structure with data we have to pass to the
// PAM callback function (a transport wagon)
typedef struct {
	bool has_login;
	std::string login;
	bool has_pass;
	std::string pass;
	std::string errmsg;
	pam_handle_t *h;
} pam_appdata;

class PAMAuthUnit : public AuthenticationUnit
{
public:
	PAMAuthUnit( const std::string& Identifier,
			  const std::string& service );
	~PAMAuthUnit();
	virtual const char* className() const	{ return PAM_AUTHENTICATION_CLASS_NAME; }

	const char** mechs() const;

	AuthenticatorSlice* slice( const std::string& /*mech*/,
				   const net::RemoteEndpoint& /*client*/ );

	User* authenticatePlain( const std::string& username, const std::string& password ) const;

private:
	friend class PAMAuthSlice;
	const std::string		m_service;	///< name of the PAM service
};

class PAMAuthConstructor : public ConfiguredObjectConstructor< AuthenticationUnit >
{
public:
	virtual ObjectConstructorBase::ObjectType objectType() const
						{ return AUTHENTICATION_OBJECT; }
	const char* objectClassName() const		{ return PAM_AUTHENTICATION_CLASS_NAME; }
	PAMAuthUnit* object( const config::NamedConfiguration& conf );
};

class PAMAuthSlice : public AuthenticatorSlice
{
	enum	SliceState	{
		SLICE_INITIALIZED = 0,		///< Has been initialized, no other data
		SLICE_HAS_LOGIN_NEED_PASS,	///< Also need the password of the user
		SLICE_INVALID_CREDENTIALS,	///< Response was wrong -> fail
		SLICE_AUTHENTICATED,		///< Response was correct -> user available
		SLICE_SYSTEM_FAILURE		///< Something is wrong
	};
	
public:
	PAMAuthSlice( const PAMAuthUnit& backend );

	~PAMAuthSlice();

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
	const PAMAuthUnit&	m_backend;
	SliceState		m_state;
	struct pam_conv		m_conv;		///< PAM internal data structure
	pam_appdata		m_appdata;	///< our void * for PAM data
	std::string		m_user;		///< usename when authenticated
	bool			m_inputReusable;
};

}} // namespace _Wolframe::AAAA

#endif // _PAM_AUTHENTICATION_HPP_INCLUDED

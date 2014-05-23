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

	const std::string* mechs() const	{ return m_mechs; }

	AuthenticatorInstance* instance( const std::string& /*mech*/,
					 const net::RemoteEndpoint& /*client*/ )
						{ return NULL; }

private:
	static const std::string	m_mechs[];	///< list of mechs for the unit
	const std::string		m_service;	///< name of the PAM service
	struct pam_conv			m_conv;		///< PAM internal data structure
	pam_appdata			m_appdata;	///< our void * for PAM data

	// states of the authenticator state machine
	enum {
		_Wolframe_PAM_STATE_NEED_LOGIN,
		_Wolframe_PAM_STATE_HAS_LOGIN,
		_Wolframe_PAM_STATE_NEED_PASS,
		_Wolframe_PAM_STATE_HAS_PASS,
		_Wolframe_PAM_STATE_ERROR
	} m_state;
};


class PAMAuthConstructor : public ConfiguredObjectConstructor< AuthenticationUnit >
{
public:
	virtual ObjectConstructorBase::ObjectType objectType() const
						{ return AUTHENTICATION_OBJECT; }
	const char* objectClassName() const		{ return PAM_AUTHENTICATION_CLASS_NAME; }
	PAMAuthUnit* object( const config::NamedConfiguration& conf );
};

}} // namespace _Wolframe::AAAA

#endif // _PAM_AUTHENTICATION_HPP_INCLUDED

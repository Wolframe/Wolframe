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
// Text file authentication
//

#ifndef _TEXT_FILE_AUTHENTICATION_HPP_INCLUDED
#define _TEXT_FILE_AUTHENTICATION_HPP_INCLUDED

#include <string>
#include <vector>
#include "AAAA/authUnit.hpp"
#include "module/constructor.hpp"
#include "AAAA/user.hpp"
#include "AAAA/CRAM.hpp"
#include "passwdFile.hpp"

#ifndef _WIN32
static const bool	USERNAME_DEFAULT_CASE_SENSIVE = true;
#else
static const bool	USERNAME_DEFAULT_CASE_SENSIVE = false;
#endif

namespace _Wolframe {
namespace AAAA {

static const char* TEXT_FILE_AUTH_CLASS_NAME = "TextFileAuth";

class TextFileAuthConfig :  public config::NamedConfiguration
{
	friend class TextFileAuthConstructor;
public:
	TextFileAuthConfig( const char* cfgName, const char* logParent, const char* logName )
		: config::NamedConfiguration( cfgName, logParent, logName ) {}

	virtual const char* className() const		{ return TEXT_FILE_AUTH_CLASS_NAME; }

	/// methods
	bool parse( const config::ConfigurationNode& pt, const std::string& node,
		    const module::ModulesDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );
private:
	std::string			m_identifier;
	std::string			m_file;
};


class TextFileAuthenticator : public AuthenticationUnit
{
public:
	TextFileAuthenticator( const std::string& Identifier, const std::string& filename );

	~TextFileAuthenticator();

	virtual const char* className() const		{ return TEXT_FILE_AUTH_CLASS_NAME; }

	AuthenticatorInstance* instance();

 /// \brief	Authenticate a user with its plain username and password
 /// \note	This function is supposed to be used only for tests.
 ///		DO NOT USE THIS FUNCTION IN REAL AUTHENTICATION MECHANISMS
 ///
 /// \param [in]	username
 /// \param [in]	password	guess what this are :D
 /// \param [in]	caseSensitveUser should the username be treated as case-sensitive or not
	User* authenticatePlain( const std::string& username, const std::string& password,
				 bool caseSensitveUser = USERNAME_DEFAULT_CASE_SENSIVE ) const;

	/// \brief
	PwdFileUser getUser( const std::string& hash, const std::string& key, PwdFileUser& user,
			     bool caseSensitveUser = USERNAME_DEFAULT_CASE_SENSIVE ) const;
private:
	const PasswordFile	m_pwdFile;
};


// Flow:
// Initialize --> receive username key + HMAC --> + user found --> send salt + challenge --> (*)
//                                                + user not found --> finish
//
// (*) --> receive response --> + got user
//				+ invalid credentials
//
class TextFileAuthInstance : public AuthenticatorInstance
{
	enum	State	{
		INSTANCE_INITIALIZED,		///< Has been initialized, no other data
		INSTANCE_USER_FOUND,		///< User has been found, will send challenge
		INSTANCE_USER_NOT_FOUND,	///< User has not been found -> fail
		INSTANCE_CHALLENGE_SENT,	///< Waiting for the response
		INSTANCE_INVALID_CREDENTIALS,	///< Response was wrong -> fail
		INSTANCE_AUTHENTICATED		///< Response was correct -> user available
	};

public:
	TextFileAuthInstance( const TextFileAuthenticator& backend );

	~TextFileAuthInstance();

	const char* typeName() const			{ return m_backend.className(); }

	/// Get the list of available mechs
	virtual const std::vector<std::string>& mechs() const;

	/// Set the authentication mech
	virtual bool setMech( const std::string& mech );

	/// Input message
	virtual void messageIn( const void* message, std::size_t size );

	/// Output message
	virtual int messageOut( const void** message, std::size_t size );

	/// The current status of the authenticator
	virtual Status status() const;

	/// The authenticated user or NULL if not authenticated
	virtual User* user() const;

private:
	const TextFileAuthenticator&	m_backend;
	struct PwdFileUser		m_usr;
	User*				m_user;
};


// Text file authentication - constructor
//***********************************************************************
class TextFileAuthConstructor : public ConfiguredObjectConstructor< AuthenticationUnit >
{
public:
	virtual ObjectConstructorBase::ObjectType objectType() const
							{ return AUTHENTICATION_OBJECT; }

	const char* objectClassName() const		{ return TEXT_FILE_AUTH_CLASS_NAME; }

	TextFileAuthenticator* object( const config::NamedConfiguration& conf );
};

}} // namespace _Wolframe::AAAA

#endif // _TEXT_FILE_AUTHENTICATION_HPP_INCLUDED

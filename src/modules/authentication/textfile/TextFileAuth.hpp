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


class TextFileAuthUnit : public AuthenticationUnit
{
public:
	TextFileAuthUnit( const std::string& Identifier, const std::string& filename );

	~TextFileAuthUnit();

	virtual const char* className() const		{ return TEXT_FILE_AUTH_CLASS_NAME; }

	const char** mechs() const;

	AuthenticatorSlice* slice( const std::string& mech, const net::RemoteEndpoint& client );
	PasswordChanger* passwordChanger( const User& user, const net::RemoteEndpoint& client );

	/// \brief	Authenticate a user with its plain username and password
	/// \note	This function is supposed to be used only for tests.
	///		DO NOT USE THIS FUNCTION IN REAL AUTHENTICATION MECHANISMS
	///
	/// \param [in]	username
	/// \param [in]	password	guess what this are :D
	/// \param [in]	caseSensitveUser should the username be treated as case-sensitive or not
	User* authenticatePlain( const std::string& username, const std::string& password,
				 bool caseSensitveUser = USERNAME_DEFAULT_CASE_SENSIVE ) const;

	/// \brief	Get the user data
	/// \param [in]	hash		username hash (base64 of the HMAC-SHA256)
	/// \param [in]	key		the key used for computing the username hash (base64)
	/// \param [out] user		reference to the user structure that will be filled
	///				by the function in case of success
	/// \param [in]	caseSensitveUser should the username be treated as case-sensitive or not
	/// \returns	true if the user was found, false otherwise
	bool getUser( const std::string& hash, const std::string& key, PwdFileUser& user,
		      bool caseSensitveUser = USERNAME_DEFAULT_CASE_SENSIVE ) const;
	bool getUser( const std::string& userHash, PwdFileUser& user,
		      bool caseSensitveUser = USERNAME_DEFAULT_CASE_SENSIVE ) const;

	bool getUserPlain( const std::string& username, PwdFileUser& user,
			   bool caseSensitveUser = USERNAME_DEFAULT_CASE_SENSIVE ) const;

private:
	static const std::string	m_mechs[];
	const PasswordFile		m_pwdFile;
};


// Flow:
// Initialize --> receive username key + HMAC --> + user found --> send salt + challenge --> (*)
//                                                + user not found --> finish
//
// (*) --> receive response --> + got user
//				+ invalid credentials
//
class TextFileAuthSlice : public AuthenticatorSlice
{
	enum	SliceState	{
		SLICE_INITIALIZED,		///< Has been initialized, no other data
		SLICE_USER_FOUND,		///< User has been found, will send challenge
		SLICE_USER_NOT_FOUND,		///< User has not been found -> fail
		SLICE_CHALLENGE_SENT,		///< Waiting for the response
		SLICE_INVALID_CREDENTIALS,	///< Response was wrong -> fail
		SLICE_AUTHENTICATED,		///< Response was correct -> user available
		SLICE_SYSTEM_FAILURE		///< Something is wrong
	};

public:
	TextFileAuthSlice( const TextFileAuthUnit& backend );

	~TextFileAuthSlice();

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

	/// Tell the slice that it is the last one
	virtual void lastSlice()			{ m_lastSlice = true; }

	/// The authenticated user or NULL if not authenticated
	virtual User* user();

private:
	const TextFileAuthUnit&	m_backend;
	struct PwdFileUser	m_usr;
	SliceState		m_state;
	CRAMchallenge*		m_challenge;
	bool			m_inputReusable;
	bool			m_lastSlice;
	bool			m_fakeUser;
};


// Flow:
// Initialize --> send salt + challenge -->
//  --> client encrypts the new password + password crc with the response -->
//  --> server decrypts with the response --> crc matches --> + - yes --> change pwd
//                                                            + - no  --> abort
//
class TextFilePwdChanger : public PasswordChanger
{
	enum	ChangerState	{
		CHANGER_INITIALIZED,		///< Has been initialized, no other data
		CHANGER_CHALLENGE_SENT,		///< Waiting for the answer
		CHANGER_INVALID_MESSAGE,	///< Answer CRC was wrong -> fail
		CHANGER_PASSWORD_EXCHANGED,	///< Answer CRC was correct -> password changed
		CHANGER_SYSTEM_FAILURE		///< Something is wrong
	};

	static const char* statusName( ChangerState i )
	{
		static const char* ar[] = {	"CHANGER_INITIALIZED",
						"CHANGER_CHALLENGE_SENT",
						"CHANGER_INVALID_MESSAGE",
						"CHANGER_PASSWORD_EXCHANGED",
						"CHANGER_SYSTEM_FAILURE"
					  };
		return ar[ i ];
	}

public:
	TextFilePwdChanger( const TextFileAuthUnit& backend, const std::string& username );

	~TextFilePwdChanger();

	void dispose();

	virtual const char* className() const		{ return m_backend.className(); }

	virtual const std::string& identifier() const	{ return m_backend.identifier(); }

	/// The input message
	virtual void messageIn( const std::string& message );

	/// The output message
	virtual std::string messageOut();

	/// The current status of the authenticator slice
	virtual Status status() const;

	/// The new password
	virtual std::string password();

private:
	const TextFileAuthUnit&	m_backend;
	struct PwdFileUser	m_usr;
	ChangerState		m_state;
	CRAMchallenge*		m_challenge;
	std::string		m_password;
};


// Text file authentication - constructor
//***********************************************************************
class TextFileAuthConstructor : public ConfiguredObjectConstructor< AuthenticationUnit >
{
public:
	virtual ObjectConstructorBase::ObjectType objectType() const
							{ return AUTHENTICATION_OBJECT; }

	const char* objectClassName() const		{ return TEXT_FILE_AUTH_CLASS_NAME; }

	TextFileAuthUnit* object( const config::NamedConfiguration& conf );
};

}} // namespace _Wolframe::AAAA

#endif // _TEXT_FILE_AUTHENTICATION_HPP_INCLUDED

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
// Text file authentication
//

#ifndef _TEXT_FILE_AUTHENTICATION_HPP_INCLUDED
#define _TEXT_FILE_AUTHENTICATION_HPP_INCLUDED

#include <string>
#include <vector>
#include "AAAA/authentication.hpp"
#include "AAAA/user.hpp"
#include "AAAA/CRAM.hpp"
#include "moduleInterface.hpp"

namespace _Wolframe {
namespace AAAA {

class TextFileAuthConfig :  public config::ObjectConfiguration
{
	friend class TextFileAuthContainer;
public:
	TextFileAuthConfig( const char* cfgName, const char* logParent, const char* logName )
		: config::ObjectConfiguration( cfgName, logParent, logName ) {}

	virtual const char* objectName() const		{ return "TextFileAuth"; }

	/// methods
	bool parse( const config::ConfigurationTree& pt, const std::string& node,
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
	virtual const char* typeName() const		{ return "TextFileAuth"; }

	AuthenticatorInstance* instance();

	User* authenticatePlain( const std::string& username, const std::string& password,
				 bool caseSensitveUser ) const;

	User* authenticate(const CRAMchallenge &username, const std::string& response,
			    bool caseSensitveUser ) const;
	User* authenticate( const CRAMchallenge& challenge, const CRAMresponse& response,
			    bool caseSensitveUser ) const;
private:
	const std::string		m_file;
};


class TextFileAuthSlice : public AuthenticatorInstance
{
	enum	FSMstate	{
		INIT,
		PARSING,
		FINISHED
	};

public:
	TextFileAuthSlice( const TextFileAuthenticator& backend );
	~TextFileAuthSlice();
	void close()					{ delete this; }

	const char* typeName() const			{ return m_backend.typeName(); }
	AuthProtocol protocolType() const		{ return AuthenticatorInstance::PLAIN; }

	void receiveData( const void* data, std::size_t size );
	const FSMinterface::Operation nextOperation();
	void signal( FSMinterface::Signal event );
	std::size_t dataLeft( const void*& begin );

	User* user();
private:
	const TextFileAuthenticator&	m_backend;
	User*				m_user;
	std::string			m_username;
	std::string			m_password;
};


class TextFileAuthContainer : public ObjectContainer< AuthenticationUnit >
{
public:
	TextFileAuthContainer( const TextFileAuthConfig& conf );
	~TextFileAuthContainer()			{}

	virtual const char* objectName() const		{ return m_auth->typeName(); }
	virtual AuthenticationUnit* object() const	{ return m_auth; }
	void dispose()					{ m_auth = NULL; delete this; }
private:
	TextFileAuthenticator*		m_auth;
};

}} // namespace _Wolframe::AAAA

#endif // _TEXT_FILE_AUTHENTICATION_HPP_INCLUDED

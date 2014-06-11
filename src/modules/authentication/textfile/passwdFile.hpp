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
//
//

#ifndef _PASSWDFILE_HPP_INCLUDED
#define _PASSWDFILE_HPP_INCLUDED

#include <string>

namespace _Wolframe {
namespace AAAA {

struct PwdFileUser	{
	std::string	user;
	std::string	hash;
	std::string	info;
	unsigned long	expiry;
public:
	~PwdFileUser()		{ clear(); }
	void clear();
};

///\brief Password file
///\note The password file format is:
/// <username>:<password hash>:<user info>:<expiry date>
/// blank lines and the lines starting with # are ignored
class PasswordFile
{
public:
	///\brief constructor
	///\param filename	password file filename
	///\param create	flag, create the file if it doesn't exist.
	///			this flag is set only in the password utility
	PasswordFile( const std::string& file, bool create = false )
		: m_filename( file ), m_create( create )	{}

	const std::string& filename() const			{ return m_filename; }

	///\brief Return the string as it would be in the password file.
	///	  Used by the password utility in 'no action' mode
	///\param user	A filled PwdFileUser structure
	static std::string passwdLine( const PwdFileUser& user );

	///\brief Add an user to the password file
	///\param username	A filled PwdFileUser structure
	///\param [in] caseSensitive The username should be treated as case sensitive
	///			if this flag is set (default)
	///\return		true if the user has been add or false
	///			if the user already exists in the password file
	///\note Throws in case of file operation error.
	bool addUser( const PwdFileUser& user, bool caseSensitive = true );

	///\brief Delete an user from the password file
	///\param username	The username of the user to delete
	///\param [in] caseSensitive The username should be treated as case sensitive
	///			if this flag is set (default)
	///\return		true if the user has been deleted or false
	///			if the user doesn't exist in the password file
	///\note Throws in case of file operation error.
	bool delUser( const std::string& username, bool caseSensitive = true );

	///\brief Get an user from the password file
	///\param [in] username	The username of the user to get
	///\param [in] caseSensitive The username should be treated as case sensitive
	///			if this flag is set (default)
	///\param [out] user	A filled PwdFileUser structure
	///\return		true if the user has been found or false
	///			if the user doesn't exist in the password file
	///\note Throws in case of file operation error.
	bool getUser( const std::string& username, PwdFileUser& user,
		      bool caseSensitive = true ) const;

	///\brief Get an user from the password file defined by an username hash
	///\param [in] hash	The HMAC-SHA256 hash of the username (base64)
	///\param [in] key	The HMAC-SHA256 key (base64 string)
	///\param [out] user	A filled PwdFileUser structure
	///\param [in] caseSensitive The username should be treated as case sensitive
	///			if this flag is set (converted to lower case)
	///\return		true if the user has been found or false
	///			if the user doesn't exist in the password file
	///\note Throws in case of file operation error or if the hash cannot
	///			be converted to a HMAC-SHA256
	bool getHMACuser( const std::string& hash, const std::string& key,
			  PwdFileUser& user, bool caseSensitive = true ) const;

	///\brief Get an user from the password file defined by an username hash
	///\param [in] userHash	The HMAC-SHA256 hash of the username (base64)
	///			combined with the HMAC-SHA256 key (base64)
	///			in the form $key$hash
	///\param [out] user	A filled PwdFileUser structure
	///\param [in] caseSensitive The username should be treated as case sensitive
	///			if this flag is set (converted to lower case)
	///\return		true if the user has been found or false
	///			if the user doesn't exist in the password file
	///\note Throws in case of file operation error or if the hash cannot
	///			be converted to a HMAC-SHA256
	bool getHMACuser( const std::string& userHash,
			  PwdFileUser& user, bool caseSensitive = true ) const;
private:
	const std::string	m_filename;
	const bool		m_create;
};

}} // namepspace _Wolframe::AAAA

#endif // _PASSWDFILE_HPP_INCLUDED

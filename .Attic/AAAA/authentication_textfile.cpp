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
// authentication_textfile.cpp
//

#include "AAAA/authentication_textfile.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/thread/thread.hpp>

#include <vector>
#include <stdexcept>
#include <fstream>

namespace _Wolframe {
namespace AAAA {

TextFileAuthenticator::TextFileAuthenticator( const std::string _filename )
{
	std::ifstream f;

	f.open( _filename.c_str( ), std::ifstream::in );
	if( f.good( ) ) {
		while( !f.eof( ) ) {
			char line[256];
			f.getline( line, 255 );
			std::vector<std::string> v;
			split( v, line, boost::is_any_of( "\t" ) );
			if( v.size( ) == 2 ) {
				m_creds.insert( std::make_pair<std::string, std::string>( v[0], v[1] ) );
			}
		}
		f.close( );
	}

	m_state = _Wolframe_TEXTFILE_STATE_NEED_LOGIN;
}

Step::AuthStep TextFileAuthenticator::nextStep( )
{
	switch( m_state ) {
		case _Wolframe_TEXTFILE_STATE_NEED_LOGIN:
			m_token = "login";
			return Step::_Wolframe_AUTH_STEP_RECV_DATA;

		case _Wolframe_TEXTFILE_STATE_NEED_PASS:
			m_token = "password";
			return Step::_Wolframe_AUTH_STEP_RECV_DATA;

		case _Wolframe_TEXTFILE_STATE_COMPUTE:
			std::map<std::string, std::string>::const_iterator it = m_creds.find( m_login );
// user not in text file
			if( it == m_creds.end( ) ) {
				m_state = _Wolframe_TEXTFILE_STATE_NEED_LOGIN;
				goto FAIL;
			}
// user found, but password doesn't match
			if( it->second != m_pass ) {
				m_state = _Wolframe_TEXTFILE_STATE_NEED_LOGIN;
				goto FAIL;
			}

// everythink is peachy
			m_state = _Wolframe_TEXTFILE_STATE_NEED_LOGIN;
			return Step::_Wolframe_AUTH_STEP_SUCCESS;
	}

FAIL:
	boost::this_thread::sleep( boost::posix_time::seconds( 1 ) );

	return Step::_Wolframe_AUTH_STEP_FAIL;
}

// never used
std::string TextFileAuthenticator::sendData( )
{
	return 0;
}

std::string TextFileAuthenticator::token( )
{
	return m_token;
}

void TextFileAuthenticator::receiveData( const std::string data )
{
	switch( m_state ) {
		case _Wolframe_TEXTFILE_STATE_NEED_LOGIN:
			m_login = data;
			m_state = _Wolframe_TEXTFILE_STATE_NEED_PASS;
			break;

		case _Wolframe_TEXTFILE_STATE_NEED_PASS:
			m_pass = data;
			m_state = _Wolframe_TEXTFILE_STATE_COMPUTE;
			break;

// TODO: application exception
		case _Wolframe_TEXTFILE_STATE_COMPUTE:
			throw new std::runtime_error( "Illegal state in auhenticator" );
			break;
	}
}

std::string TextFileAuthenticator::getError( )
{
	return "";
}

}} // namespace _Wolframe::AAAA

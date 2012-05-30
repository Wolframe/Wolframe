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
//
//

#include <string>
#include <iostream>

#include <unistd.h>

#include "passwdFile.hpp"

namespace _Wolframe {
namespace AAAA {

void getPassword( std::string& password, const std::string& prompt, const char /*displayChar*/ )
{
	char* pass = getpass( prompt.c_str() );
	password = pass;
//	std::cout << prompt;

//	char key;

//	do	{
//		std::cin >> key;

//		switch (key)	{
//			case '\b': //backspace pressed
//				if(password .length() > 0)	{
//					password .erase(password .length() - 1, 1);
//					//Erase the last Character in password signs
//					std::cout << '\b' << " " << '\b';
//				}
//				break;

//			default:
//				if(key > 31 && key < 127)	{   //Only no control characters are valid
//					password .push_back(key);
//					std::cout << displayChar;
//				}
//		}

//	} while(key != '\r'); //Quit if Enter is Pressed
}

//#include <iostream>
//#include <stdexcept>
//#include <string>
//#include <windows.h>
//using namespace std;

//string getpassword( const string& prompt = "Enter password> " )
//{
//	string result;

//	// Set the console mode to no-echo, not-line-buffered input
//	DWORD mode, count;
//	HANDLE ih = GetStdHandle( STD_INPUT_HANDLE  );
//	HANDLE oh = GetStdHandle( STD_OUTPUT_HANDLE );
//	if (!GetConsoleMode( ih, &mode ))
//		throw runtime_error(
//				"getpassword: You must be connected to a console to use this program.\n"
//				);
//	SetConsoleMode( ih, mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT) );

//	// Get the password string
//	WriteConsoleA( oh, prompt.c_str(), prompt.length(), &count, NULL );
//	char c;
//	while (ReadConsoleA( ih, &c, 1, &count, NULL) && (c != '\r') && (c != '\n'))
//	{
//		if (c == '\b')
//		{
//			if (result.length())
//			{
//				WriteConsoleA( oh, "\b \b", 3, &count, NULL );
//				result.erase( result.end() -1 );
//			}
//		}
//		else
//		{
//			WriteConsoleA( oh, "*", 1, &count, NULL );
//			result.push_back( c );
//		}
//	}

//	// Restore the console mode
//	SetConsoleMode( ih, mode );

//	return result;
//}


PasswordFile::PasswordFile()
{
}

PasswordFile::PasswordFile( const std::string& /*filename*/ )
{
}

bool PasswordFile::open()
{
	return true;
}

bool PasswordFile::open( const std::string& /*filename*/, bool /*create*/ )
{
	return true;
}

bool PasswordFile::create( const std::string& /*filename*/ )
{
	return true;
}

bool PasswordFile::addUser( const std::string& /*user*/, const std::string& /*password*/,
			    const std::string& /*userInfo*/, const std::string& /*comment*/ )
{
	return true;
}

bool PasswordFile::modifyUser(const std::string& /*user*/, const std::string& /*password*/,
			      const std::string& /*userInfo*/, const std::string& /*comment*/ )
{
	return true;
}

bool PasswordFile::addOrModifyUser(const std::string& /*user*/, const std::string& /*password*/,
				   const std::string& /*userInfo*/, const std::string& /*comment*/ )
{
	return true;
}

}} // namepspace _Wolframe::AAAA

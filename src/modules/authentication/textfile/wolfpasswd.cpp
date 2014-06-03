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
// wolfpasswd - text file password utility
//

#include <boost/program_options.hpp>
#include <string>
#include <iostream>

#include "utils/getPassword.hpp"
#include "utils/fileUtils.hpp"
#include "passwdFile.hpp"
#include "AAAA/passwordHash.hpp"
#include "types/base64.hpp"
#include "system/globalRngGen.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

namespace PO = boost::program_options;
namespace WA = _Wolframe::AAAA;
namespace WU = _Wolframe::utils;


int main( int argc, char* argv[] )
{
	unsigned	days;
	std::string	saltStr;
	std::string	b64SaltStr;

	PO::options_description desc( "Usage:\n"
				      "  wolfpasswd -D passwordfile username\n"
				      "  wolfpasswd [-c] [-d days] passwordfile username [user info]\n"
				      "  wolfpasswd -b[c] [-d days] passwordfile username password [user info]\n"
				      "  wolfpasswd -n [-s/-S salt] [-d days] username [user info]\n"
				      "  wolfpasswd -nb [-s/-S salt] [-d days] username password [user info]\n"
				      "  wolfpasswd -h\n"
				      "Options" );
	desc.add_options()
			( "help,h", "Display this help message." )
			( "create,c", "Create the file if it doesn't exist." )
			( "salt,s", PO::value< std::string >( &saltStr ), "Use the specified plain text salt (only valid with --display-only)." )
			( "base64-salt,S", PO::value< std::string >( &b64SaltStr ), "Use the specified base64 salt (only valid with --display-only)." )
			( "days,d", PO::value< unsigned >( &days )->default_value( 0 ), "Set the password expiration after <days>." )
			( "display-only,n", "Don't update the password file; display results on stdout." )
			( "batch,b", "Use the password from the command line instead of prompting for it.")
			( "delete,D", "Delete the specified user." )
			;

	PO::options_description cmdArgs( "Arguments" );
	cmdArgs.add_options()
			( "posArgs", PO::value< std::vector<std::string> >(), "positional arguments" )
			;
	cmdArgs.add( desc );

	PO::positional_options_description posArgs;
	posArgs.add( "posArgs", -1 );

	PO::variables_map vm;
	try	{
		PO::store( PO::command_line_parser( argc, argv ).
			   options( cmdArgs ).positional( posArgs ).run(), vm );
		PO::notify( vm );
	}
	catch( std::exception& e )	{
		std::cerr << "\nERROR: " << e.what() << "\n\n";
		std::cout << desc << "\n";
		return 2;
	}

	bool	createFile = false;
	bool	displayOnly = false;
	bool	batchPwd = false;
	bool	delUser = false;

	if ( vm.count( "create" ))
		createFile = true;
	if ( vm.count( "display-only" ))
		displayOnly = true;
	if ( vm.count( "batch" ))
		batchPwd = true;
	if ( vm.count( "delete" ))
		delUser = true;

	// deal with help
	if ( vm.count( "help" ))	{
		if ( createFile || displayOnly|| batchPwd || delUser
				|| vm.count( "posArgs" ))
			std::cout << "\nWARNING: --help ignores all other flags and arguments.\n\n";
		std::cout << desc << "\n";
		return 1;
	}

	if ( !vm.count( "posArgs" ))	{
		std::cerr << "\nERROR: no arguments given.\n\n";
		std::cout << desc << "\n";
		return 2;
	}
	const std::vector<std::string>& args = vm["posArgs"].as< std::vector<std::string> >();

	_Wolframe::GlobalRandomGenerator& rnd = _Wolframe::GlobalRandomGenerator::instance( "" );

	// display only
	if ( displayOnly )	{
		bool	wrong = false;
		if ( createFile )	{
			std::cerr << "\nERROR: --create cannot be used with --display-only.";
			wrong = true;
		}
		if ( delUser )	{
			std::cerr << "\nERROR: --delete cannot be used with --display-only.";
			wrong = true;
		}
		if ( batchPwd )	{
			if ( args.size() < 2 )	{
				std::cerr << "\nERROR: too few arguments.";
				wrong = true;
			}
			else if ( args.size() > 3 )	{
				std::cerr << "\nERROR: too many arguments.";
				wrong = true;
			}
		}
		else	{
			if ( args.size() > 2 )	{
				std::cerr << "\nERROR: too many arguments.";
				wrong = true;
			}
		}
		if ( !saltStr.empty() && !b64SaltStr.empty() )	{
			std::cerr << "\nERROR: both --salt and --base64-salt arguments given.";
			wrong = true;
		}
		if ( wrong )	{
			std::cout << "\n\n" << desc << "\n";
			return 2;
		}

		// All parameters are OK
		std::string passwd;
		WA::PwdFileUser user;
		if ( ! batchPwd )	{
			passwd = WA::getPassword();
			if( args.size() == 2 )
				user.info = args[1];
		}
		else	{
			passwd = args[1];
			if( args.size() == 3 )
				user.info = args[2];
		}
		// now do the job
		WA::PasswordHash::Salt salt;
		if ( saltStr.empty() && b64SaltStr.empty() )
			salt = WA::PasswordHash::Salt( rnd );
		else	{
			if ( !saltStr.empty() )
				b64SaltStr = _Wolframe::base64::encode( saltStr.data(), saltStr.size(), 0 );
			salt = WA::PasswordHash::Salt( b64SaltStr );
		}
		WA::PasswordHash pwd( salt, passwd );

		user.user = args[0];
		user.hash = pwd.toString();
		user.expiry = 0;
		std::cout << "Password line: " << WA::PasswordFile::passwdLine( user );
	}
	// delete user
	else if ( delUser )	{
		bool	wrong = false;
		if ( createFile )	{
			std::cerr << "\nERROR: --create cannot be used with --delete.";
			wrong = true;
		}
		if ( args.size() < 2 )	{
			std::cerr << "\nERROR: too few arguments.";
			wrong = true;
		}
		else if ( args.size() > 2 )	{
			std::cerr << "\nERROR: too many arguments.";
			wrong = true;
		}
		if ( wrong )	{
			std::cout << "\n\n" << desc << "\n";
			return 2;
		}
		if ( ! saltStr.empty() )
			std::cerr << "\nWarning: --salt argument ignored.";
		if ( ! b64SaltStr.empty() )
			std::cerr << "\nWarning: --base64-salt argument ignored.";

		// All parameters are OK
		try	{
			std::string filename = WU::resolvePath( boost::filesystem::absolute( args[0] ).string());
			WA::PasswordFile pwdFile( filename );

			if ( pwdFile.delUser( args[1] ))
				std::cout << "User '" << args[1] << "' removed from password file '"
					  << filename << "'.";
			else
				std::cout << "User '" << args[1] << "' not found in password file '"
					  << filename << "'.";
		}
		catch( std::exception& e )	{
			std::cerr << "Error removing user '" << args[1] << "': " << e.what() << "\n\n";
			return 1;
		}
	}
	// normal operation
	else	{
		bool	wrong = false;
		if ( batchPwd )	{
			if ( args.size() < 3 )	{
				std::cerr << "\nERROR: too few arguments.";
				wrong = true;
			}
			else if ( args.size() > 4 )	{
				std::cerr << "\nERROR: too many arguments.";
				wrong = true;
			}
		}
		else	{
			if ( args.size() < 2 )	{
				std::cerr << "\nERROR: too few arguments.";
				wrong = true;
			}
			if ( args.size() > 3 )	{
				std::cerr << "\nERROR: too many arguments.";
				wrong = true;
			}
		}
		if ( !saltStr.empty() && !b64SaltStr.empty() )	{
			std::cerr << "\nERROR: both --salt and --base64-salt arguments given.";
			wrong = true;
		}

		if ( wrong )	{
			std::cout << "\n\n" << desc << "\n";
			return 2;
		}

		// Al parameters are OK, do the job
		bool create = false;
		if ( createFile )
			create = true;
		try	{
			std::string filename = WU::resolvePath( boost::filesystem::absolute( args[0] ).string());
			WA::PasswordFile pwdFile( filename, create );
			WA::PwdFileUser user;

			std::string passwd;
			if ( ! batchPwd )	{
				passwd = WA::getPassword();
				if( args.size() == 3 )
					user.info = args[2];
			}
			else	{
				passwd = args[2];
				if( args.size() == 4 )
					user.info = args[3];
			}
			WA::PasswordHash::Salt salt;
			if ( saltStr.empty() && b64SaltStr.empty() )
				salt = WA::PasswordHash::Salt( rnd );
			else	{
				if ( !saltStr.empty() )
					b64SaltStr = _Wolframe::base64::encode( saltStr.data(), saltStr.size(), 0 );
				salt = WA::PasswordHash::Salt( b64SaltStr );
			}
			WA::PasswordHash pwd( salt, passwd );
			user.user = args[1];
			user.hash = pwd.toString();
			user.expiry = 0;
			if ( pwdFile.addUser( user ) )
				std::cout << "User '" << args[1] << "' added to the password file '"
					  << filename << "'.";
			else
				std::cout << "Error: user '" << args[1] << "' already exists in the password file '"
					  << filename << "'.";

		}
		catch( std::exception& e )	{
			std::cerr << "Error adding user '" << args[1] << "': " << e.what() << "\n\n";
			return 1;
		}
	}
	std::cout << "\nDone.\n\n";
	return 0;
}

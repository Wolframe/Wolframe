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
// wolfpasswd - text file password utility
//

#include <boost/program_options.hpp>
#include <string>

namespace PO = boost::program_options;

int main( int argc, char* argv[] )
{
	PO::options_description desc( "Usage:\n"
				      "  wolfpasswd [-cD] passwordfile username\n"
				      "  wolfpasswd -b[c] passwordfile username password\n"
				      "  wolfpasswd -n username\n"
				      "  wolfpasswd -nb username password\n"
				      "  wolfpasswd -h\n"
				      "Options" );
	desc.add_options()
			( "help,h", "Display this help message." )
			( "create,c", "Create the file if it doesn't exist." )
			( "display-only,n", "Don't update the password file; display results on stdout." )
			( "command-line-password,b", "Use the password from the command line instead of prompting for it.")
			( "delete,D", "Delete the specified user." )
			;

	PO::options_description args( "Arguments" );
	args.add_options()
			( "posArgs", PO::value< std::vector<std::string> >(), "positional arguments" )
			;
	args.add( desc );

	PO::positional_options_description posArgs;
	posArgs.add( "posArgs", 3 );

	PO::variables_map vm;
	try	{
		PO::store( PO::command_line_parser( argc, argv ).
					  options( args ).positional( posArgs ).run(), vm );
		PO::notify( vm );
	}
	catch( std::exception& e )	{
		std::cerr << "\nERROR: " << e.what() << "\n\n";
		std::cout << desc << "\n";
		return 2;
	}

	bool	createFile = false;
	bool	displayOnly = false;
	bool	cmdLinePwd = false;
	bool	delUser = false;

	if ( vm.count( "create" ))
		createFile = true;
	if ( vm.count( "display-only" ))
		displayOnly = true;
	if ( vm.count( "command-line-password" ))
		cmdLinePwd = true;
	if ( vm.count( "delete" ))
		delUser = true;

	if ( vm.count( "help" ))	{
		if ( createFile || displayOnly|| cmdLinePwd || delUser
				|| vm.count( "posArgs" ))
			std::cout << "\nWARNING: --help ignores all other flags and arguments.\n\n";
		std::cout << desc << "\n";
		return 1;
	}

	if ( !vm.count( "posArgs" ))	{
		std::cerr << "\nERROR: no arguments given.\n\n";
		std::cout << desc << "\n";
	}
	else	{
		std::cout << "Program arguments (" << vm.count( "posArgs" ) << "):\n";
		for ( std::vector<std::string>::const_iterator it = vm["posArgs"].as< std::vector<std::string> >().begin();
						it != vm["posArgs"].as< std::vector<std::string> >().end(); it ++ )
			std::cout << "\t" << *it << "\n";
		std::cout << std::endl;
	}
}

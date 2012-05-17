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
	PO::options_description desc( "wolfpasswd [-cD] passwordfile username\n"
				      "wolfpasswd -b[c] passwordfile username password\n"
				      "wolfpasswd -n username\n"
				      "wolfpasswd -nb username password\n" );
	desc.add_options()
			( "create,c", "Create a new file." )
			( "display-only,n", "Don't update file; display results on stdout." )
			( "command-line-password,b", "Use the password from the command line instead of prompting for it.")
			( "delete,D", "Delete the specified user." )
			;

	PO::variables_map vm;
	PO::store(PO::parse_command_line(argc, argv, desc), vm);
	PO::notify(vm);

	if (vm.count("help")) {
	    std::cout << desc << "\n";
	    return 1;
	}

	if (vm.count("compression")) {
	    std::cout << "Compression level was set to "
	 << vm["compression"].as<int>() << ".\n";
	} else {
	    std::cout << "Compression level was not set.\n";
	}
}

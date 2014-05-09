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
// integer limit unit tests
//

#include <limits>
#include <iostream>
#include "types/integer.hpp"

int main( )
{
  std::cout << "std::numeric_limits<signed int>::max( ): "
            << std::numeric_limits<signed int>::max( ) 
            << " " << std::hex
            << std::numeric_limits<signed int>::max( )
            << std::dec
            << std::endl;
  std::cout << "std::numeric_limits<signed int>::min( ): "
            << std::numeric_limits<signed int>::min( )
            << " " << std::hex
            << std::numeric_limits<signed int>::min( )
            << std::dec
            << std::endl;

  std::cout << "std::numeric_limits<unsigned int>::max( ): "
            << std::numeric_limits<unsigned int>::max( ) 
            << " " << std::hex
            << std::numeric_limits<unsigned int>::max( )
            << std::dec
            << std::endl;
  std::cout << "std::numeric_limits<unsigned int>::min( ): "
            << std::numeric_limits<unsigned int>::min( )
            << " " << std::hex
            << std::numeric_limits<unsigned int>::min( )
            << std::dec
            << std::endl;
  std::cout << "std::numeric_limits<_WOLFRAME_INTEGER>::max( ): "
            << std::numeric_limits<_WOLFRAME_INTEGER>::max( ) 
            << " " << std::hex
            << std::numeric_limits<_WOLFRAME_INTEGER>::max( )
            << std::dec
            << std::endl;
  std::cout << "std::numeric_limits<_WOLFRAME_INTEGER>::min( ): "
            << std::numeric_limits<_WOLFRAME_INTEGER>::min( )
            << " " << std::hex
            << std::numeric_limits<_WOLFRAME_INTEGER>::min( )
            << std::dec
            << std::endl;

  std::cout << "std::numeric_limits<_WOLFRAME_UINTEGER>::max( ): "
            << std::numeric_limits<_WOLFRAME_UINTEGER>::max( ) 
            << " " << std::hex
            << std::numeric_limits<_WOLFRAME_UINTEGER>::max( )
            << std::dec
            << std::endl;
  std::cout << "std::numeric_limits<_WOLFRAME_UINTEGER>::min( ): "
            << std::numeric_limits<_WOLFRAME_UINTEGER>::min( )
            << " " << std::hex
            << std::numeric_limits<_WOLFRAME_UINTEGER>::min( )
            << std::dec
            << std::endl;

  std::cout << "std::numeric_limits<double>::max( ): "
            << std::numeric_limits<double>::max( ) 
            << " " << std::hex
            << std::numeric_limits<double>::max( )
            << std::dec
            << std::endl;
  std::cout << "std::numeric_limits<double>::min( ): "
            << std::numeric_limits<double>::min( )
            << " " << std::hex
            << std::numeric_limits<double>::min( )
            << std::dec
            << std::endl;

  return 0;
}


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

  return 0;
}


#include "ddl/simpleFormCompile.hpp"
#include <iostream>
#include <string>

using namespace _Wolframe;

int main( int argc, const char** argv)
{
	if (argc <= 1)
	{
		std::cerr << "missing input file parameter" << std::endl;
		return -1;
	}
	ddl::simpleform::SimpleformDDLParser mm;
	std::string error;
	if (!mm.compile( argv[1], error))
	{
		std::cerr << error << std::endl;
		return -2;
	}
	return 0;
}



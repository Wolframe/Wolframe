#include "DataTree.hpp"
#include <iostream>

static const char* testDataTree[] =
{
	"xyz =1",
	"aa= 1.0",
	"doc { a = b }",
	0
};

int main( int, const char**)
{
	int ii=0;
	for (; testDataTree[ii]; ++ii)
	{
		DataTree dt = DataTree::fromString( QString( testDataTree[ii]));
		if (dt.isValid())
		{
			std::cerr << "test " << ii << " returned valid structure" << std::endl;
		}
		else
		{
			std::cerr << "test " << ii << " failed (returned invalid structure)" << std::endl;
		}
	}
	return 0;
}


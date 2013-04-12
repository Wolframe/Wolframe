#include "DataTree.hpp"
#include <QDebug>

struct Test
{
	const char* input;
	const char* output;
};

static const Test testDataTree[] =
{
	{"xyz =1", "xyz = 1"},
	{"aa= 1.0", "aa = 1.0"},
	{"doc { a = b }","doc { a = b }"},
	{"doc { bli = 1.0; bla='723974' }","doc { bli = 1.0; bla = 723974 }"},
	{"doc { substruct{ bla='72 3974'} }","doc { substruct { bla = \"72 3974\" } }"},
	{"doc { bli = 1.0; substruct{ bla='72 3974'} }","doc { bli = 1.0; substruct { bla = \"72 3974\" } }"},
	{"doc { bli = 1.0; substruct{ bla='\"72 3974\"'}; oth { x {{abc}} }}","doc { bli = 1.0; substruct { bla = '\"72 3974\"' }; oth { x { {abc} } } }"},
	{"doc { a[] {b} }","doc { a[] { b } }"},
	{0,0}
};

int main( int, const char**)
{
	int ii=0;
	int successCnt = 0;
	int failedCnt = 0;
	for (; testDataTree[ii].input; ++ii)
	{
		DataTree dt = DataTree::fromString( QString( testDataTree[ii].input));
		if (dt.isValid())
		{
			QString output = dt.toString();
			QString expected = QString( testDataTree[ii].output);
			if (output != expected)
			{
				qCritical() << "test " << ii << " failed (output does not match):";
				qDebug() << "output:" << output;
				qDebug() << "expected:" << expected;
				++failedCnt;
			}
			else
			{
				dt = DataTree::fromString( QString( testDataTree[ii].output));
				output = dt.toString();
				if (output != expected)
				{
					qCritical() << "test " << ii << " failed (non idempotent):";
					qDebug() << "output:" << output;
					qDebug() << "expected:" << expected;
					++failedCnt;
				}
				else
				{
					++successCnt;
				}
			}
		}
		else
		{
			qCritical() << "test " << ii << " failed (returned invalid structure)";
			++failedCnt;
		}
	}
	qDebug() << "number of tests executed:" << ii;
	qDebug() << "number of tests succeded:" << successCnt;
	qDebug() << "number of tests failed:" << failedCnt;
	return failedCnt==0?0:1;
}


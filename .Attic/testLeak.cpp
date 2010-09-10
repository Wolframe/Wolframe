// test for SSL library init leaks
// compile with:
//    g++ -o testLeak testLeak.cpp -lboost_system -lssl
// test with:
//    valgrind --leak-check=full --show-reachable=yes ./testLeak

#include <iostream>
#include <boost/asio/ssl.hpp> // Memory Leak

using namespace std;

int main(int argc, char *argv[])
{
	cout << "Hello, world !" << endl;

	return 0;
}


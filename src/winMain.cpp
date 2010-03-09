//
// winMain.cpp
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "server.hpp"

#if !defined(_WIN32)
#error "This is the WIN32 main !"
#else

boost::function0<void> consoleCtrlFunction;

BOOL WINAPI consoleCtrlHandler(DWORD ctrlType)
{
	switch (ctrlType)	{
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_SHUTDOWN_EVENT:
			consoleCtrlFunction();
			return TRUE;
		default:
			return FALSE;
	}
}


int main(int argc, char* argv[])
{
	try	{
		// get configuration !!!!
		std::size_t numThreads = 4;
		long timeoutDuration = 5000;
		std::string port = "8080";
		std::string address = "0.0.0.0";

		_SMERP::server s(address, port, numThreads, timeoutDuration);

		// Set console control handler to allow server to be stopped.
		consoleCtrlFunction = boost::bind(&_SMERP::server::stop, &s);
		SetConsoleCtrlHandler(consoleCtrlHandler, TRUE);

		// Run the server until stopped.
		s.run();
	}
	catch (std::exception& e)	{
		std::cerr << "exception: " << e.what() << "\n";
	}

	return 0;
}

#endif // defined(_WIN32)

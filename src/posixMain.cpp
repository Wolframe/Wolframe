//
// posixMain.cpp
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "server.hpp"
#include "commandLine.hpp"
#include "configFile.hpp"
#include "serverConfig.hpp"

#if !defined(_WIN32)

#include <pthread.h>
#include <signal.h>

int main(int argc, char* argv[])
{
	try	{
		// get configuration !!!!
		std::size_t num_threads = 4;
		long timeout_duration_ms = 5000;
		std::string port = "8080";
		std::string address = "0.0.0.0";

		_SMERP::cmdLineConfig	cmdLine;
		_SMERP::cfgFileConfig	cfgFile;
		_SMERP::serverConfig	*config = new _SMERP::serverConfig;

		// Block all signals for background thread.
		sigset_t new_mask;
		sigfillset(&new_mask);
		sigset_t old_mask;
		pthread_sigmask(SIG_BLOCK, &new_mask, &old_mask);

		// Run server in background thread(s).
		_SMERP::server s( address, port, num_threads, timeout_duration_ms);
		boost::thread t(boost::bind(&_SMERP::server::run, &s));

		// Restore previous signals.
		pthread_sigmask(SIG_SETMASK, &old_mask, 0);

		// Wait for signal indicating time to shut down.
		sigset_t wait_mask;
		sigemptyset(&wait_mask);
		sigaddset(&wait_mask, SIGINT);
		sigaddset(&wait_mask, SIGQUIT);
		sigaddset(&wait_mask, SIGTERM);
		pthread_sigmask(SIG_BLOCK, &wait_mask, 0);
		int sig = 0;
		sigwait(&wait_mask, &sig);

		// Stop the server.
		s.stop();
		t.join();
	}
	catch (std::exception& e)	{
		std::cerr << "exception: " << e.what() << "\n";
	}

	return 0;
}

#endif // !defined(_WIN32)

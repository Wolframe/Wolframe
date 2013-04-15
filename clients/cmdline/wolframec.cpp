/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
///\brief Command line client for wolframe
#include "options.hpp"
#include "utils.hpp"
#include "session.hpp"
#include "textwolf/xmlscanner.hpp"
#include "textwolf/charset_utf8.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <boost/thread.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/filesystem.hpp>

using namespace _Wolframe;
using namespace _Wolframe::client;

///\TODO Not to be defined here
static const unsigned short APP_MAJOR_VERSION = 0;
static const unsigned short APP_MINOR_VERSION = 0;
static const unsigned short APP_REVISION = 6;
static const unsigned short APP_BUILD = 0;

class CmdlineSession :public Session
{
public:
	CmdlineSession( const Session::Configuration& cfg, const std::string& uiformdir_)
		:Session(cfg)
		,m_uiformdir(uiformdir_)
		,m_termination_signal(false){}

	virtual void receiveUIForm( const char* id, const char* data, std::size_t datasize)
	{
		std::string uiform( data, datasize);
		boost::filesystem::path path( m_uiformdir);
		path /= id;
		path.replace_extension( ".ui");
		std::string uiformfile( path.string());
		writeFile( uiformfile, uiform);
	}

	virtual void notifyState( const char* msg)
	{
		boost::interprocess::scoped_lock<boost::mutex> lock( m_io_mutex);
		std::cerr << "state '" << msg << "'" << std::endl;
	}

	virtual void notifyAttribute( const char* id, const char* value)
	{
		boost::interprocess::scoped_lock<boost::mutex> lock( m_io_mutex);
		std::cerr << "attribute " << id << " '" << value << "'" << std::endl;
	}

	virtual void notifyError( const char* msg)
	{
		boost::interprocess::scoped_lock<boost::mutex> lock( m_io_mutex);
		std::cerr << "error '" << msg << "'" << std::endl;
	}

	virtual void notifyTermination()
	{
		{
			boost::lock_guard<boost::mutex> lock(m_termination_mutex);
			m_termination_signal = true;
		}
		m_termination_cond.notify_one();
	}

	void waitTermination()
	{
		boost::unique_lock<boost::mutex> lock( m_termination_mutex);
		while (!m_termination_signal)
		{
			m_termination_cond.wait( lock);
		}
	}

private:
	std::string m_uiformdir;
	boost::mutex m_io_mutex;
	boost::mutex m_termination_mutex;
	boost::condition_variable m_termination_cond;
	bool m_termination_signal;
};

class CmdlineRequestHandler :public RequestHandler
{
public:
	CmdlineRequestHandler( const std::string& outputfile_)
		:m_outputfile(outputfile_){}

	virtual ~CmdlineRequestHandler()
	{
		if (!m_outputfile.empty())
		{
			writeFile( m_outputfile, m_out.str());
		}
	}

	static void checkValidXML( const char* data, std::size_t datasize)
	{
		std::string str( data, datasize);
		typedef textwolf::XMLScanner<char*,textwolf::charset::UTF8,textwolf::charset::UTF8,std::string> MyXMLScanner;
		char* xmlitr = const_cast<char*>( str.c_str());

		MyXMLScanner xs( xmlitr);
		MyXMLScanner::iterator itr,end;
		int taglevel = 0;

		for (itr=xs.begin(),end=xs.end(); itr!=end; itr++)
		{
			if (itr->type() == MyXMLScanner::ErrorOccurred) throw std::runtime_error( itr->content());
			if (itr->type() == MyXMLScanner::OpenTag) ++taglevel;
			if (itr->type() == MyXMLScanner::CloseTag) --taglevel;
		}
		if (taglevel != 0) throw std::runtime_error( "tags not balanced");
	}

	virtual void answer( const char* data, std::size_t datasize)
	{
		try
		{
			checkValidXML( data, datasize);
			std::cerr << "ok request" << std::endl;
		}
		catch (const std::runtime_error& e)
		{
			std::cerr << "Error " << e.what() << std::endl;
			std::cerr << "SLEEPING ..." << std::endl;
			boost::this_thread::sleep( boost::posix_time::seconds( 10));
		}
		if (m_outputfile.empty())
		{
			std::cout << std::string( data, datasize);
		}
		else
		{
			m_out << std::string( data, datasize);
		}
	}

	virtual void error( const char* msg)
	{
		std::cerr << "ERROR " << msg << std::endl;
	}

private:
	std::ostringstream m_out;
	std::string m_outputfile;
};

int main( int argc, char **argv )
{
	bool doExit = false;
	try
	{
		static boost::filesystem::path execdir = boost::filesystem::system_complete( argv[0]).parent_path();
		WolframecCommandLine cmdline( argc, argv);

		if (cmdline.printversion())
		{
			std::cerr << "wolframe client version ";
			std::cerr << APP_MAJOR_VERSION << "." << APP_MINOR_VERSION << "." << APP_REVISION << "." << APP_BUILD << std::endl;
			doExit = true;
		}
		if (cmdline.printhelp())
		{
			cmdline.print( std::cerr);
			doExit = true;
		}
		if (doExit) return 0;

		CmdlineSession session( cmdline.config(), cmdline.uiformdirectory());
		CmdlineRequestHandler requesthandler( cmdline.outputfile());
		session.start();

		std::vector<std::string>::const_iterator qi = cmdline.document().begin(), qe = cmdline.document().end();
		for (; qi != qe; ++qi)
		{
			if (!session.doRequest( &requesthandler, cmdline.request(), qi->c_str(), qi->size()))
			{
				std::cerr << "REQUEST ERROR service not ready" << std::endl;
			}
		}
		session.quit();
		session.waitTermination();
		session.stop();
	}
	catch (const std::bad_alloc& e)
	{
		std::cerr << "out of memory" << std::endl;
		return 1;
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
		return 2;
	}
	catch (const std::exception& e)
	{
		std::cerr << "exception " << e.what() << std::endl;
		return 2;
	}
	return 0;
}


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
///\file options.hpp
///\brief Implementation of command line options of wolframec
#include "options.hpp"
#include "utils.hpp"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <map>

using namespace _Wolframe;
using namespace _Wolframe::client;

namespace po = boost::program_options;

struct OptionStruct
{
	po::options_description fopt;
	po::positional_options_description popt;

	OptionStruct()
		:fopt("Options")
	{
		fopt.add_options()
			( "version,v", "print version")
			( "help,h", "print help message")
			( "uiformdir,u", boost::program_options::value<std::string>(), "directory containing the forms (where to write the uiforms to)")
			( "outputfile,o", boost::program_options::value<std::string>(), "output file (where to write the request answers to)")
			( "request,q", boost::program_options::value<std::string>(), "type of requests to process")
			( "document,d", boost::program_options::value<std::vector<std::string> >(), "file containing a document of a request to process")
#ifdef WITH_SSL
			( "ssl,S", "use SSL encryption")
			( "CA-cert-file,C", boost::program_options::value<std::string>(), "certificate file containing the CA (default: ./certs/CAclient.cert.pem)")
			( "client-cert-file,c", boost::program_options::value<std::string>(), "client certificate to present to the server (default: ./certs/client.crt)")
			( "client-cert-key,k", boost::program_options::value<std::string>(), "client key file (default: ./private/client.key)")
#endif
			( "connect-timeout", boost::program_options::value<unsigned short>(), "in seconds, how long to wait for connect")
			( "read-timeout", boost::program_options::value<unsigned short>(), "in seconds, terminate after inactivity")
			( "host", po::value<std::string>(), "the host to connect to (default 127.0.0.1)")
			( "port", po::value<std::string>(), "the port to connect to (default 7661)")
			;

		popt.add( "host", 1);
		popt.add( "port", 1);
	}
};

WolframecCommandLine::WolframecCommandLine( int argc, char** argv)
	:m_printhelp(false)
	,m_printversion(false)
{
	static const OptionStruct ost;
	std::string address = "127.0.0.1";
	std::string name = "7661";
	std::vector<std::string> documentfiles;
	unsigned short connect_timeout = 30;
	unsigned short read_timeout = 30;
	std::string CA_cert_file = "./certs/CAclient.cert.pem";
	std::string client_cert_file = "./certs/client.crt";
	std::string client_cert_key = "./private/client.key";

	po::variables_map vmap;
	try
	{
		po::store( po::command_line_parser(argc, argv).options(ost.fopt).positional(ost.popt).run(), vmap);
		po::notify( vmap);

		m_printversion = vmap.count( "version");
		m_printhelp = vmap.count( "help");

		if (vmap.count("connect-timeout")) connect_timeout = vmap[ "connect-timeout"].as<unsigned short>();
		if (vmap.count("read-timeout")) read_timeout = vmap[ "read-timeout"].as<unsigned short>();
		if (vmap.count("host")) address = vmap[ "host"].as<std::string>();
		if (vmap.count("port")) name = vmap[ "port"].as<std::string>();
		if (vmap.count("request")) m_request = vmap[ "request"].as<std::string>();
		if (vmap.count("document"))
		{
			documentfiles = vmap[ "document"].as< std::vector< std::string> >();
			std::vector<std::string>::const_iterator ri = documentfiles.begin(), re = documentfiles.end();
			for (; ri != re; ++ri)
			{
				m_document.push_back( readFile( *ri));
			}
		}
		if (vmap.count("uiformdir")) m_uiformdirectory = vmap[ "uiformdir"].as<std::string>();
		if (vmap.count("outputfile")) m_outputfile = vmap[ "outputfile"].as<std::string>();

		m_config = Session::Configuration( address, name, connect_timeout, read_timeout);

		if (vmap.count("ssl"))
		{
			if (vmap.count( "CA-cert-file")) CA_cert_file = vmap["CA-cert-file"].as<std::string>();
			if (vmap.count( "client-cert-file")) client_cert_file = vmap["client-cert-file"].as<std::string>();
			if (vmap.count( "client-cert-key")) client_cert_key = vmap["client-cert-key"].as<std::string>();
			m_config.ssl_certificate( CA_cert_file, client_cert_file, client_cert_key);
		}
		else
		{
			if (vmap.count( "CA-cert-file")) std::cerr << "option 'CA-cert-file' specified without SSL. Option is ignored" << std::endl;
			if (vmap.count( "client-cert-file")) std::cerr << "option 'client-cert-file' specified without SSL. Option is ignored" << std::endl;
			if (vmap.count( "client-cert-key")) std::cerr << "option 'client-cert-key' specified without SSL. Option is ignored" << std::endl;
		}

		std::ostringstream dd;
		dd << ost.fopt;
		m_helpstring = dd.str();
	}
	catch (std::exception& e)
	{
		throw std::runtime_error( std::string("error parsing command line options: ") + e.what());
	}
}

void WolframecCommandLine::print(std::ostream& out) const
{
	out << "Call:" << std::endl;
	out << "\twolframec [OPTION] <host> <port>" << std::endl;
	out << m_helpstring << std::endl;
}




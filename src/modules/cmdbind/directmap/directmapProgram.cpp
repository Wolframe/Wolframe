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
///\brief Implementation of directmap programs
///\file directmapProgram.cpp
#include "directmapProgram.hpp"
#include "utils/miscUtils.hpp"
#include "logger-v1.hpp"
#include "config/programBase.hpp"
#include <stdexcept>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

std::string DirectmapCommandDescription::tostring() const
{
	std::ostringstream rt;
	rt << "call='" << call << "'";
	rt << ", filter='" << filter << "'";
	rt << ", input form='" << inputform << "'";
	rt << ", output form='" << outputform << "'";
	return rt.str();
}


bool DirectmapProgram::is_mine( const std::string& filename) const
{
	std::string ext( utils::getFileExtension( filename));
	return (boost::iequals( ext, ".directmap"));
}

void DirectmapProgram::loadProgram( const std::string& filename)
{
	try
	{
		addProgram( utils::readSourceFileContent( filename));
	}
	catch (const config::PositionalErrorException& e)
	{
		throw config::PositionalFileErrorException( config::PositionalFileError( e, filename));
	}
	catch (const std::runtime_error& err)
	{
		throw std::runtime_error( std::string( "error in directmap program file '") + filename + "' :" + err.what());
	}
}

void DirectmapProgram::addProgram( const std::string& source)
{
	static const utils::CharTable optab( "=;:)(,");
	config::PositionalErrorMessageBase ERROR(source);
	config::PositionalErrorMessageBase::Message MSG;
	std::string prgname,tok;
	std::string::const_iterator argstart;
	std::string::const_iterator si = source.begin(), se = source.end();
	char ch;

	try
	{
		while ((ch = utils::gotoNextToken( si, se)) != 0)
		{
			if (ch == '#')
			{
				// ... comment
				for (; si != se && *si != '\n'; ++si);
				continue;
			}

			DirectmapCommandDescription cmd_descr;
			switch ((ch=utils::parseNextToken( prgname, si, se, optab)))
			{
				case ';': throw ERROR( si, "empty statement");
				case '\'':
				case '\"': throw ERROR( si, "identifier expected instead of string at start of statement");
				default:
					if (optab[ ch]) throw ERROR( si, MSG << "identifier expected at start of statement instead of '" << ch << "'");
			}
			switch ((ch=utils::parseNextToken( tok, si, se, optab)))
			{
				case '\0':
					throw ERROR( si, "unexpected end of program");
				case '=':
					break;
				case '(':
					cmd_descr.call = prgname;
					break;
				default:
					if (optab[ ch]) throw ERROR( si, MSG << "'=' expected instead of '" << ch << "'");
					throw ERROR( si, MSG << "'=' expected instead of '" << tok << "'");
			}
			if (cmd_descr.call.empty())
			{
				switch ((ch=utils::parseNextToken( cmd_descr.call, si, se, optab)))
				{
					case '\0': throw ERROR( si, "unexpected end of program");
					default:
						if (optab[ ch]) throw ERROR( si, MSG << "function name identifier expected instead of '" << ch << "'");
				}
				switch ((ch=utils::parseNextToken( tok, si, se, optab)))
				{
					case '\0': throw ERROR( si, "unexpected end of program");
					case '(': break;
					default:
						if (optab[ ch]) throw ERROR( si, MSG << "'(' expected instead of '" << ch << "'");
						throw ERROR( si, MSG << "'(' expected instead of '" << tok << "'");
				}
			}
			switch ((ch=utils::parseNextToken( cmd_descr.filter, si, se, optab)))
			{
				case '\0': throw ERROR( si, "unexpected end of program");
				case '\'': break;
				case '\"': throw ERROR( si, MSG << "filter identifier expected instead of string");
				default:
					if (optab[ ch]) throw ERROR( si, MSG << "filter name identifier expected instead of '" << ch << "'");
			}
			if ((ch=utils::gotoNextToken( si, se)) == ':')
			{
				++si;
				switch ((ch=utils::parseNextToken( cmd_descr.inputform, si, se, optab)))
				{
					case '\0': throw ERROR( si, "unexpected end of program");
					case '\'':
					case '\"': throw ERROR( si, MSG << "input form identifier expected instead of string");
					default:
						if (optab[ ch]) throw ERROR( si, MSG << "input form identifier expected instead of '" << ch << "'");
				}
				ch = utils::gotoNextToken( si, se);
			}
			if (ch != ')')
			{
				if (!ch) throw ERROR( si, "unexpected end of program");
				if (optab[ ch]) throw ERROR( si, MSG << "')' expected instead of '" << ch << "'");
				throw ERROR( si, MSG << "')' expected instead of identifier or string");
			}
			++si;
			if (utils::gotoNextToken( si, se) == ':')
			{
				++si;
				switch ((ch=utils::parseNextToken( cmd_descr.outputform, si, se, optab)))
				{
					case '\0': throw ERROR( si, "unexpected end of program");
					case '\'': break;
					case '\"': throw ERROR( si, MSG << "output form identifier expected instead of string");
					default:
						if (optab[ ch]) throw ERROR( si, MSG << "output form identifier expected instead of '" << ch << "'");
				}
			}
			if (utils::gotoNextToken( si, se) != ';')
			{
				if (!ch) throw ERROR( si, "unexpected end of program");
				if (optab[ ch]) throw ERROR( si, MSG << "';' expected instead of '" << ch << "'");
				throw ERROR( si, MSG << "';' expected instead of identifier or string");
			}
			++si;
			LOG_TRACE << "Loading direct map function " << prgname << " as " << cmd_descr.tostring();
			Parent::insert( prgname, cmd_descr);
		}
	}
	catch (const config::PositionalErrorException& e)
	{
		throw e;
	}
	catch (const std::runtime_error& e)
	{
		throw ERROR( si, e.what());
	}
}

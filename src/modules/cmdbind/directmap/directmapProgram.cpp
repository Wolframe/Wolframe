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
#include "utils/fileUtils.hpp"
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
	rt << ", input filter='" << inputfilter << "'";
	rt << ", output filter='" << outputfilter << "'";
	rt << ", input form='" << inputform << "'";
	rt << ", output form='" << outputform << "'";
	return rt.str();
}


bool DirectmapProgram::check( const proc::ProcessorProvider* provider) const
{
	bool rt = true;
	types::keymap<DirectmapCommandDescription>::const_iterator di = begin(), de = end();
	for (; di != de; ++di)
	{
		const langbind::FormFunction* func = provider->formFunction( di->second.call);
		if (!func)
		{
			LOG_ERROR << "configured non existing form function call in directmap for '" << di->second.name << "': '" << di->second.call << "'";
			rt = false;
		}
	}
	return rt;
}

bool DirectmapProgram::is_mine( const std::string& filename) const
{
	std::string ext( utils::getFileExtension( filename));
	if (boost::iequals( ext, ".directmap"))
	{
		LOG_WARNING << "Using deprecated file extension for program '.directmap' instead of '.dmap'";
		return true;
	}
	if (boost::iequals( ext, ".dmap")) return true;
	return false;
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

static std::vector<langbind::FilterArgument> parseFilterArguments( std::string::const_iterator& si, const std::string::const_iterator& se)
{
	static const utils::CharTable optab( "(),=");
	std::vector<langbind::FilterArgument> rt;
	char ch;
	std::string tok;
	std::string val;

	for (;;)
	{
		// parse argument identifier or value:
		ch = utils::parseNextToken( tok, si, se, optab);
		if (!ch) throw std::runtime_error("brackets not balanced in filter arguments");
		if (ch == ')')
		{
			return rt;
		}
		else if (ch == '(' || ch == ',' || ch == '=')
		{
			throw std::runtime_error( std::string( "syntax error in filter arguments: token expected instead of comma (") + (char)ch + ")");
		}

		// check for assignment:
		ch = utils::parseNextToken( val, si, se, optab);
		if (ch == ',')
		{
			rt.push_back( langbind::FilterArgument( "", tok));
			continue;
		}
		else if (ch == ')')
		{
			rt.push_back( langbind::FilterArgument( "", tok));
			return rt;
		}
		else if (ch == '=')
		{
			// for assignment get value:
			ch = utils::parseNextToken( val, si, se, optab);
			if (ch == ')' || ch == '(' || ch == ',' || ch == '=')
			{
				throw std::runtime_error( "syntax error in filter arguments: unexpected operator");
			}
			rt.push_back( langbind::FilterArgument( tok, val));

			ch = utils::parseNextToken( val, si, se, optab);
			if (ch == ',')
			{
				continue;
			}
			else if (ch == ')')
			{
				return rt;
			}
		}
	}
}

static DirectmapCommandDescription parseCommandDescription( std::string::const_iterator& si, const std::string::const_iterator& se)
{
	DirectmapCommandDescription rt;
	static const utils::CharTable optab( ";()");
	std::string tok;
	std::vector<std::string> toklist;
	std::string cmdname;
	std::string in_doctype;
	std::string out_doctype;
	std::string call_arg;
	std::string return_arg;
	bool validate = true;
	bool call_arg_set = false;
	bool return_arg_set = false;
	bool input_filter_set = false;
	bool output_filter_set = false;
	enum Lexem {PASS,RETURN,CALL,IDENTIFIER,FILTER,INPUT,OUTPUT};
	enum State {ParseCommand,ParseInputDoctype,ParseAttribute,ParseAttributeFilter,ParseCallArg,ParseReturnArg,ParseFilter,ParseInputFilter,ParseOutputFilter};
	State state = ParseCommand;
	char ch;

	ch = utils::parseNextToken( tok, si, se, optab);
	for (; ch != ';' && ch; ch = utils::parseNextToken( tok, si, se, optab))
	{
		Lexem lexem = IDENTIFIER;
		toklist.clear();
		if (ch == '(')
		{
			for (;;)
			{
				ch = utils::parseNextToken( tok, si, se, optab);
				if (ch == ')') break;
				if (!ch) throw std::runtime_error( "unexpected end of expression: argument list not closed with ')'");
				if (optab[ch]) throw std::runtime_error("expected identifier or string as argument of COMMAND");
				toklist.push_back( tok);
			}
			if (toklist.size())
			{
				tok = toklist.at(0);
			}
			else
			{
				tok.clear();
			}
		}
		if (ch != '\'' && ch != '"')
		{
			if (boost::iequals( tok, "PASS"))
			{
				if (!validate) throw std::runtime_error("duplicate definition of PASS");
				lexem = PASS;
			}
			else if (boost::iequals( tok, "RETURN"))
			{
				if (return_arg_set) throw std::runtime_error("duplicate definition of RETURN");
				lexem = RETURN;
			}
			else if (boost::iequals( tok, "CALL"))
			{
				if (call_arg_set) throw std::runtime_error("duplicate definition of CALL");
				lexem = CALL;
			}
			else if (boost::iequals( tok, "FILTER"))
			{
				lexem = FILTER;
			}
			else if (boost::iequals( tok, "INPUT"))
			{
				lexem = INPUT;
			}
			else if (boost::iequals( tok, "OUTPUT"))
			{
				lexem = OUTPUT;
			}
		}
		switch (state)
		{
			case ParseCommand:
				if (lexem != IDENTIFIER) throw std::runtime_error( std::string("expected identifier after command and got keyword '") + tok + "'");
				if (toklist.size() > 2) throw std::runtime_error( "to many arguments for COMMAND");
				if (toklist.size() == 2)
				{
					cmdname = toklist.at(0);
					in_doctype = toklist.at(1);
					state = ParseAttribute;
					break;
				}
				else if (tok.empty())
				{
					throw std::runtime_error( "expected nonempty argument for COMMAND");
				}
				else
				{
					cmdname = tok;
					state = ParseInputDoctype;
				}
				break;

			case ParseInputDoctype:
				if (lexem != IDENTIFIER)
				{
					in_doctype = cmdname;
					cmdname.clear();
					state = ParseAttribute;
					/*no break here!*/
				}
				else if (tok.empty())
				{
					if (toklist.size() > 1) throw std::runtime_error( "to many arguments for COMMAND");
					throw std::runtime_error( "expected nonempty argument for document type of command");
				}
				else
				{
					if (toklist.size() > 1) throw std::runtime_error( "to many arguments for COMMAND");
					in_doctype = tok;
					state = ParseAttribute;
					break;
				}

			case ParseAttributeFilter:
				if (lexem == INPUT)
				{
					state = ParseInputFilter;
					continue;
				}
				if (lexem == OUTPUT)
				{
					state = ParseOutputFilter;
					continue;
				}
				state = ParseAttribute;
				/*no break here!*/

			case ParseAttribute:
				switch (lexem)
				{
					case PASS:
						validate = false;
						state = ParseAttribute;
						continue;
					case RETURN:
						state = ParseReturnArg;
						continue;
					case CALL:
						state = ParseCallArg;
						continue;
					case IDENTIFIER:
					case INPUT:
					case OUTPUT:
						break;
					case FILTER:
						state = ParseFilter;
						continue;
				}
				throw std::runtime_error("PASS,FILTER,CALL or RETURN expected instead of token");

			case ParseReturnArg:
				if (lexem != IDENTIFIER) throw std::runtime_error("identifier expected as argument of RETURN");
				if (toklist.size() > 1) throw std::runtime_error( "to many arguments for RETURN");
				if (tok.empty()) throw std::runtime_error( "expected nonempty argument for RETURN");
				return_arg_set = true;
				return_arg = tok;
				state = ParseAttribute;
				continue;

			case ParseCallArg:
				if (lexem != IDENTIFIER) throw std::runtime_error("identifier expected as argument of CALL");
				if (toklist.size() > 1) throw std::runtime_error( "to many arguments for CALL");
				if (tok.empty()) throw std::runtime_error( "expected nonempty argument for CALL");
				call_arg_set = true;
				call_arg = tok;
				state = ParseAttribute;
				continue;

			case ParseFilter:
				if (lexem == INPUT)
				{
					state = ParseInputFilter;
					continue;
				}
				if (lexem == OUTPUT)
				{
					state = ParseOutputFilter;
					continue;
				}
				if (lexem != IDENTIFIER) throw std::runtime_error("identifier expected as argument of FILTER");
				if (toklist.size() > 1) throw std::runtime_error( "to many arguments for FILTER");
				if (input_filter_set) throw std::runtime_error("duplicate definition of FILTER");
				if (output_filter_set) throw std::runtime_error("duplicate definition of FILTER");
				if (tok.empty()) throw std::runtime_error( "expected nonempty argument for FILTER");
				input_filter_set = true;
				rt.inputfilter = tok;
				output_filter_set = true;
				rt.outputfilter = tok;
				state = ParseAttribute;
				continue;

			case ParseInputFilter:
				if (lexem != IDENTIFIER) throw std::runtime_error("identifier expected as argument of FILTER INPUT");
				if (toklist.size() > 1) throw std::runtime_error( "to many arguments for FILTER INPUT");
				if (input_filter_set) throw std::runtime_error("duplicate definition of FILTER INPUT");
				if (tok.empty()) throw std::runtime_error( "expected nonempty argument for FILTER INPUT");
				input_filter_set = true;
				rt.inputfilter = tok;
				state = ParseAttributeFilter;
				ch = utils::gotoNextToken( si, se);
				if (ch == '(')
				{
					++si;
					rt.inputfilterarg = parseFilterArguments( si, se);
				}
				continue;

			case ParseOutputFilter:
				if (lexem != IDENTIFIER) throw std::runtime_error("identifier expected as argument of FILTER OUTPUT");
				if (toklist.size() > 1) throw std::runtime_error( "to many arguments for FILTER OUTPUT");
				if (output_filter_set) throw std::runtime_error("duplicate definition of FILTER OUTPUT");
				if (tok.empty()) throw std::runtime_error( "expected nonempty argument for FILTER OUTPUT");
				output_filter_set = true;
				rt.outputfilter = tok;
				state = ParseAttributeFilter;
				ch = utils::gotoNextToken( si, se);
				if (ch == '(')
				{
					++si;
					rt.inputfilterarg = parseFilterArguments( si, se);
				}
				continue;
		}
	}
	if (state != ParseAttribute) throw std::runtime_error( "unexpected end of command");

	rt.name = cmdname + in_doctype;
	if (call_arg_set)
	{
		rt.call = call_arg;
	}
	else
	{
		rt.call = rt.name;
	}
	if (validate)
	{
		rt.inputform = in_doctype;
	}
	if (return_arg_set)
	{
		rt.outputform = return_arg;
	}
	return rt;
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
	bool hasProtocolCmd = false;

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

			switch ((ch=utils::parseNextToken( prgname, si, se, optab)))
			{
				case ';': throw ERROR( si, "empty statement");
				case '\'':
				case '\"': throw ERROR( si, "identifier expected instead of string at start of statement");
				default:
					if (optab[ ch]) throw ERROR( si, MSG << "identifier expected at start of statement instead of '" << ch << "'");
			}
			DirectmapCommandDescription cmd_descr;

			if (boost::iequals( prgname, "COMMAND"))
			{
				cmd_descr = parseCommandDescription( si, se);
				LOG_TRACE << "Loading direct map function " << cmd_descr.name << " as " << cmd_descr.tostring();
				Parent::insert( cmd_descr.name, cmd_descr);
				continue;
			}
			switch ((ch=utils::parseNextToken( tok, si, se, optab)))
			{
				case '\0':
					throw ERROR( si, "unexpected end of program");
				case '=':
					break;
				case '~':
					if ('(' !=  (ch=utils::parseNextToken( tok, si, se, optab)))
					{
						if (optab[ ch]) throw ERROR( si, MSG << "'(' expected instead of '" << ch << "'");
						throw ERROR( si, MSG << "'(' expected instead of '" << tok << "'");
					}
					cmd_descr.call = prgname;
					hasProtocolCmd = true;
					break;
				case '(':
					cmd_descr.call = prgname;
					break;
				default:
					if (optab[ ch]) throw ERROR( si, MSG << "'=','~' or '(' expected instead of '" << ch << "'");
					throw ERROR( si, MSG << "'=','~' or '(' expected instead of '" << tok << "'");
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
			switch ((ch=utils::parseNextToken( cmd_descr.inputfilter, si, se, optab)))
			{
				case '\0': throw ERROR( si, "unexpected end of program");
				case '\'': break;
				case '\"': throw ERROR( si, MSG << "filter identifier expected instead of string");
				default:
					if (optab[ ch]) throw ERROR( si, MSG << "filter name identifier expected instead of '" << ch << "'");
			}
			cmd_descr.outputfilter = cmd_descr.inputfilter;
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
				if (hasProtocolCmd)
				{
					cmd_descr.call.append( cmd_descr.inputform);
				}
			}
			else if (hasProtocolCmd)
			{
				throw ERROR( si, "command name referencing document type ('~') but no input document type specified");
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


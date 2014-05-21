/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
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
#include "utils/parseUtils.hpp"
#include "logger-v1.hpp"
#include "config/programBase.hpp"
#include <stdexcept>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

bool DirectmapProgram::checkReferences( const proc::ProcessorProviderInterface* provider) const
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
		if (!di->second.inputform.empty() && !provider->formDescription( di->second.inputform))
		{
			LOG_ERROR << "configured non existing input form in directmap for '" << di->second.name << "': '" << di->second.inputform << "'";
		}
		if (!di->second.outputform.empty() && !di->second.skipvalidation_output && !provider->formDescription( di->second.outputform))
		{
			LOG_ERROR << "configured non existing output form in directmap for '" << di->second.name << "': '" << di->second.outputform << "'";
		}
		if (!di->second.inputfilter.empty())
		{
			langbind::Filter* flt = provider->filter( di->second.inputfilter);
			if (flt)
			{
				delete flt;
			}
			else
			{
				LOG_ERROR << "configured non existing input filter in directmap for '" << di->second.name << "': '" << di->second.inputfilter << "'";
			}
		}
		if (!di->second.outputfilter.empty() && !provider->filter( di->second.outputfilter))
		{
			langbind::Filter* flt = provider->filter( di->second.outputfilter);
			if (flt)
			{
				delete flt;
			}
			else
			{
				LOG_ERROR << "configured non existing output filter in directmap for '" << di->second.name << "': '" << di->second.outputfilter << "'";
			}
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

enum Lexem
{
	IDENTIFIER,SKIP,STANDALONE,RETURN,CALL,FILTER,INPUT,OUTPUT,AUTHORIZE
};
static const char* lexemName( Lexem i)
{
	static const char* ar[] = {"IDENTIFIER","SKIP","STANDALONE","RETURN","CALL","FILTER","INPUT","OUTPUT","AUTHORIZE",0};
	return ar[ (int)i];
}
Lexem lexemId( const std::string& id)
{
	const char* nam;
	for (int ii=1; (0!=(nam=lexemName((Lexem)ii))); ++ii)
	{
		if (boost::iequals( id, nam)) return (Lexem)ii;
	}
	return IDENTIFIER;
}

static bool parseNextLexem( Lexem& lexem, std::vector<std::string>& toklist, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	static const utils::CharTable optab( ";()");
	std::string tok;
	Lexem follow;

	char ch = utils::parseNextToken( tok, si, se, optab);
	if (!ch) throw std::runtime_error( "missing ';' at end of expression");
	if (ch == ';') return false;

	lexem = IDENTIFIER;
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
		return true;
	}
	if (ch != '\'' && ch != '"')
	{
		lexem = lexemId( tok);
		if (lexem != IDENTIFIER) return true;
	}
	if (tok.empty())
	{
		throw std::runtime_error( "empty string as token is not allowed");
	}
	toklist.push_back( tok);
	std::string::const_iterator start = si;

	for (ch = utils::parseNextToken( tok, start, se, optab); ch && ch != ';';ch = utils::parseNextToken( tok, start, se, optab))
	{
		follow = lexemId( tok);
		if (follow != IDENTIFIER) break;
		si = start;
		toklist.push_back( tok);
	}
	if (!ch) throw std::runtime_error( "missing ';' at end of expression");
	return true;
}

static DirectmapCommandDescription parseCommandDescription( std::string::const_iterator& si, const std::string::const_iterator& se)
{
	static const utils::CharTable fchartab( "a..zA..Z_0..9.");
	static const utils::CharTable fcharoptab( ";()");
	DirectmapCommandDescription rt;
	std::vector<std::string> toklist;
	std::string cmdname;
	std::string in_doctype;
	std::string call_arg;
	std::string return_arg;
	std::string return_root;
	bool validate_input = true;
	bool call_arg_set = false;
	bool return_arg_set = false;
	bool return_skip = false;
	bool return_standalone = false;
	bool input_filter_set = false;
	bool output_filter_set = false;
	bool authorize_set = false;
	enum State {ParseCommand,ParseInputDoctype,ParseAttribute,ParseAttributeFilter,ParseReturnArg,ParseFilter,ParseInputFilter,ParseOutputFilter,ParseAuthorize};

	State state = ParseCommand;
	char ch;
	Lexem lexem = IDENTIFIER;

	while (parseNextLexem( lexem, toklist, si, se))
	{
		switch (state)
		{
			case ParseCommand:
				if (lexem != IDENTIFIER) throw std::runtime_error( std::string("expected identifier after command and got keyword '") + lexemName(lexem) + "'");
				if (toklist.size() > 2) throw std::runtime_error( "to many arguments for COMMAND");
				if (toklist.size() == 2)
				{
					cmdname = toklist.at(0);
					in_doctype = toklist.at(1);
					state = ParseAttribute;
					break;
				}
				else if (toklist.empty())
				{
					throw std::runtime_error( "expected nonempty argument for COMMAND");
				}
				else
				{
					cmdname = toklist.at(0);
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
				else if (toklist.empty())
				{
					if (toklist.size() > 1) throw std::runtime_error( "to many arguments for COMMAND");
					throw std::runtime_error( "expected nonempty argument for document type of command");
				}
				else
				{
					if (toklist.size() > 1) throw std::runtime_error( "to many arguments for COMMAND");
					in_doctype = toklist.at(0);
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
					case SKIP:
						if (!validate_input) throw std::runtime_error( "SKIP (input) specified twice");
						validate_input = false;
						state = ParseAttribute;
						rt.skipvalidation_input = true;
						continue;
					case RETURN:
						if (return_arg_set || return_standalone) throw std::runtime_error( "RETURN specified twice");
						state = ParseReturnArg;
						continue;
					case CALL:
						if (call_arg_set) throw std::runtime_error( "CALL specified twice");
						ch = utils::parseNextToken( call_arg, si, se, fcharoptab, fchartab);
						if (ch == '(')
						{
							ch = utils::parseNextToken( call_arg, si, se, fcharoptab, fchartab);
							if (!ch || ch == ';') throw std::runtime_error("brackets '(' not closed in argument of 'CALL'");
							if (ch == ')') throw std::runtime_error("empty argument of 'CALL'");
							ch = utils::gotoNextToken( si, se);
							if (!ch || ch == ';') throw std::runtime_error("brackets '(' not closed in argument of 'CALL'");
							if (ch != ')') throw std::runtime_error("expected ')' after '(' and function name argument of 'CALL'");
							++si;
						}
						else
						{
							if (!ch || ch == ';') throw std::runtime_error("function name expected as argument of 'CALL'");
							if (ch != '\'' && ch != '"' && lexemId(call_arg) != IDENTIFIER) throw std::runtime_error("keyword instead of function name used as argument of 'CALL'");
						}
						call_arg_set = true;
						state = ParseAttribute;
						continue;
					case STANDALONE:
					case IDENTIFIER:
					case INPUT:
					case OUTPUT:
						break;
					case FILTER:
						if (input_filter_set || output_filter_set) throw std::runtime_error( "FILTER specified twice");
						state = ParseFilter;
						continue;
					case AUTHORIZE:
						if (authorize_set) throw std::runtime_error( "AUTHORIZE specified twice");
						state = ParseAuthorize;
						continue;
				}
				throw std::runtime_error("SKIP,FILTER,CALL,AUTHORIZE or RETURN expected instead of token");

			case ParseAuthorize:
				if (lexem != IDENTIFIER) throw std::runtime_error( std::string("expected identifier after command and got keyword '") + lexemName(lexem) + "'");
				if (toklist.size() > 2) throw std::runtime_error( "to many arguments for COMMAND");
				if (toklist.size() == 2)
				{
					rt.authfunction = toklist.at(0);
					rt.authresource = toklist.at(1);
					state = ParseAttribute;
					break;
				}
				else if (toklist.empty())
				{
					throw std::runtime_error( "expected nonempty argument for AUTHORIZE if specified");
				}
				else
				{
					rt.authfunction = toklist.at(0);
					state = ParseAttribute;
				}
				if (rt.authfunction.empty())
				{
					throw std::runtime_error( "expected nonempty authorization function name after AUTHORIZE");
				}
				break;

			case ParseReturnArg:
				if (lexem == SKIP)
				{
					if (return_standalone) throw std::runtime_error( "SKIP specified after STANDALONE in RETURN");
					if (return_skip) throw std::runtime_error( "SKIP specified twice after RETURN");
					return_skip = true;
					continue;
				}
				else if (lexem == STANDALONE)
				{
					if (return_standalone) throw std::runtime_error( "STANDALONE specified twice after RETURN");
					return_standalone = true;
					return_skip = true;
					continue;
				}
				if (lexem != IDENTIFIER) throw std::runtime_error("identifier or SKIP expected as argument of RETURN");
				if (return_standalone)
				{
					if (toklist.size() > 1) throw std::runtime_error( "to many arguments for RETURN");
					if (toklist.size() > 0)
					{
						return_root = toklist.at(0);
						if (return_root.empty()) throw std::runtime_error( "expected nonempty document root element as argument for RETURN STANDALONE");
					}
					else
					{
						throw std::runtime_error( "missing root element as argument for RETURN STANDALONE");
					}
				}
				else if (return_skip)
				{
					if (toklist.size() > 2) throw std::runtime_error( "to many arguments for RETURN SKIP");
					if (toklist.size() > 1)
					{
						return_root = toklist.at(1);
						if (return_root.empty()) throw std::runtime_error( "expected nonempty document root element as second argument for RETURN SKIP");
					}
					if (toklist.empty()) throw std::runtime_error( "expected nonempty document type name (with optional root element) as argument for RETURN SKIP");
					return_arg = toklist.at(0);
					return_arg_set = true;
				}
				else
				{
					if (toklist.size() > 1) throw std::runtime_error( "to many arguments for RETURN");
					if (toklist.empty()) throw std::runtime_error( "expected nonempty document type name (with optional root element) as argument for RETURN");
					return_arg = toklist.at(0);
					return_arg_set = true;
				}
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
				if (toklist.empty()) throw std::runtime_error( "expected nonempty argument for FILTER");
				input_filter_set = true;
				rt.inputfilter = toklist.at(0);
				output_filter_set = true;
				rt.outputfilter = toklist.at(0);
				state = ParseAttribute;
				continue;

			case ParseInputFilter:
				if (lexem != IDENTIFIER) throw std::runtime_error("identifier expected as argument of FILTER INPUT");
				if (toklist.size() > 1) throw std::runtime_error( "to many arguments for FILTER INPUT");
				if (input_filter_set) throw std::runtime_error("duplicate definition of FILTER INPUT");
				if (toklist.empty()) throw std::runtime_error( "expected nonempty argument for FILTER INPUT");
				input_filter_set = true;
				rt.inputfilter = toklist.at(0);
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
				if (toklist.empty()) throw std::runtime_error( "expected nonempty argument for FILTER OUTPUT");
				output_filter_set = true;
				rt.outputfilter = toklist.at(0);
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
	if (validate_input)
	{
		rt.inputform = in_doctype;
	}
	if (return_arg_set)
	{
		rt.outputform = return_arg;
	}
	rt.outputrootelem = return_root;
	rt.skipvalidation_output = return_skip;
	rt.output_doctype_standalone = return_standalone;
	rt.command_has_result = (return_arg_set || return_standalone);
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


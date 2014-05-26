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

static void parseMetaData( types::DocMetaData& metadata, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	static const utils::CharTable keytab( "a..zA..Z_0..9.:-");
	static const utils::CharTable optab( "=,}");
	char ch = utils::gotoNextToken( si, se);
	if (ch != '{') throw std::logic_error("illegal call of parseMetaData");
	++si;
	std::map<std::string,bool> defmap;
	for (;;)
	{
		ch = utils::gotoNextToken( si, se);
		if (ch == '}') break;
		if (!ch) throw std::runtime_error("unexpected end of file parsing return document meta data");
		++si;
		std::string key;
		ch = utils::parseNextToken( key, si, se, optab, keytab);
		if (optab[ch]) throw std::runtime_error("identifier expected, key of document meta data attribute");
		if (key.empty()) throw std::runtime_error("non empty key of document meta data attribute expected");
		ch = utils::gotoNextToken( si, se);
		if (!ch) throw std::runtime_error("unexpected end of file parsing return document meta data");
		if (ch != '=') throw std::runtime_error("unexpected token, assignment operator '=' of document meta data attribute expected");
		++si;
		std::string val;
		ch = utils::parseNextToken( val, si, se, optab, keytab);
		if (!ch) throw std::runtime_error("unexpected end of file parsing return document meta data");
		if (optab[ch]) throw std::runtime_error("string or identifier expected as value of document meta data attribute");
		if (defmap.find(key) != defmap.end())
		{
			throw std::runtime_error( std::string("duplicate definition of attribute '") + key + "'");
		}
		defmap[ key] = true;
		if (ch != '"' && ch != '\'' && val == "NULL")
		{
			metadata.deleteAttribute( key);
		}
		else
		{
			metadata.setAttribute( key, val);
		}
		ch = utils::gotoNextToken( si, se);
		if (ch == '}') break;
		if (!ch) throw std::runtime_error("unexpected end of file parsing return document meta data");
		if (ch != ',') std::runtime_error("unexpected token, separating comma ',' or end structure '}' of document meta data attribute expected");
		++si;
	}
	++si;
}

static void skipEndOfCommand( std::string::const_iterator& si, const std::string::const_iterator& se)
{
	char ch;
	while ((ch = utils::gotoNextToken( si, se)) != 0)
	{
		if (ch == '#')
		{
			utils::parseNextLine( si, se);
			continue;
		}
		if (ch == ';')
		{
			++si;
			break;
		}
	}
}

static bool isAlphaNum( char ch)
{
	return (((ch|32) >= 'a' && (ch|32) <= 'z') || ch == '_' || (ch >= '0' && ch <= '9'));
}

static bool isString( char ch)
{
	return (ch == '\'' || ch == '"');
}

static std::string errorToken( char ch, const std::string& tok)
{
	if (isAlphaNum(ch)) return tok;
	if (isString(ch)) return std::string(1,ch) + tok + ch;
	return std::string("'") + std::string(1,ch) + "'";
}

static const utils::CharTable g_fchartab( "a..zA..Z_0..9.");
static const utils::CharTable g_fcharoptab( ";#()");

static const char* g_directmap_ids[] = {"SKIP","FILTER","RESULT","CALL","AUTHORIZE",0};
enum DirectmapKeyword{ d_NONE, d_SKIP,d_FILTER,d_RESULT,d_CALL,d_AUTHORIZE };
static const utils::IdentifierTable g_directmap_idtab( false, g_directmap_ids);


static char gotoNextToken( std::string::const_iterator& si, const std::string::const_iterator& se)
{
	char ch = utils::gotoNextToken( si, se);
	while (ch == '#')
	{
		utils::parseNextLine( si, se);
		ch = utils::gotoNextToken( si, se);
	}
	return ch;
}

static char fetchNextToken( std::string& tok, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	char ch = utils::parseNextToken( tok, si, se, g_fcharoptab, g_fchartab);
	while (ch == '#')
	{
		utils::parseNextLine( si, se);
		ch = utils::parseNextToken( tok, si, se, g_fcharoptab, g_fchartab);
	}
	if (!ch) throw std::runtime_error( "unexpected end of file in command definition");
	return ch;
}

static std::string parseName( std::string::const_iterator& si, const std::string::const_iterator& se, const char* statemsg)
{
	std::string rt;
	char ch = fetchNextToken( rt, si, se);
	if (!isAlphaNum(ch) && !isString(ch))
	{
		throw std::runtime_error( std::string( "identifier or string expected ") + statemsg);
	}
	return rt;
}

bool parseKeyword( std::string::const_iterator& si, std::string::const_iterator se, const char* keyword)
{
	gotoNextToken( si, se);
	std::string::const_iterator start = si;
	std::string tok;
	char ch = fetchNextToken( tok, si, se);
	if (isAlphaNum(ch) && boost::algorithm::iequals( tok, keyword))
	{
		return true;
	}
	si = start;
	return false;
}

struct CommandHeader
{
	std::string prefix;
	std::string doctype;

	CommandHeader(){}
	CommandHeader( const CommandHeader& o)
		:prefix(o.prefix),doctype(o.doctype){}

	static CommandHeader parse( std::string::const_iterator& si, const std::string::const_iterator& se)
	{
		CommandHeader rt;
		std::string tok;
		char ch = 0;
	
		ch = fetchNextToken( rt.prefix, si, se);
		if (ch == '(')
		{
			rt.prefix = parseName( si, se, "after COMMAND (");
			ch = gotoNextToken(si,se);
			if (isAlphaNum( ch))
			{
				rt.doctype = parseName( si, se, "after COMMAND ( prefix");
				ch = gotoNextToken(si,se);
				if (ch != ')')
				{
					std::runtime_error( "expected ')' after COMMAND ( prefix doctype");
				}
				++si;
			}
			else if (ch == ')')
			{
				rt.doctype = rt.prefix;
				rt.prefix.clear();
				++si;
			}
			else
			{
				throw std::runtime_error( "expected ')' or doctype identifier after COMMAND ( prefix");
			}
		}
		else if (isAlphaNum(ch) || isString(ch))
		{
			std::string::const_iterator prev_si = si;

			if (d_NONE != (DirectmapKeyword)utils::parseNextIdentifier( si, se, g_directmap_idtab))
			{
				rt.doctype = rt.prefix;
				rt.prefix.clear();
				si = prev_si;
			}
			else
			{
				rt.doctype = parseName( si, se, "after COMMAND ( prefix");
			}
		}
		else
		{
			throw std::runtime_error( std::string( "unexpected token, expected identifier or string or '(' instead of ") + errorToken( ch, rt.prefix));
		}
		return rt;
	}
};

struct AuthFunction
{
	std::string name;
	std::string resource;

	AuthFunction(){}
	AuthFunction( const AuthFunction& o)
		:name(o.name),resource(o.resource){}

	static AuthFunction parse( std::string::const_iterator& si, const std::string::const_iterator& se)
	{
		AuthFunction rt;
		char ch = 0;
	
		ch = fetchNextToken( rt.name, si, se);
		if (ch == '(')
		{
			rt.name = parseName( si, se, "after AUTHORIZE (");
			rt.resource = parseName( si, se, "after AUTHORIZE ( funcname");
			ch = gotoNextToken(si,se);
			if (ch != ')')
			{
				std::runtime_error( "expected ')' after AUTHORIZE ( funcname resource");
			}
			++si;
		}
		else if (isAlphaNum(ch) || isString(ch))
		{
			rt.resource = parseName( si, se, "after AUTHORIZE funcname");
		}
		else
		{
			throw std::runtime_error( std::string( "unexpected token, expected identifier or string or '(' instead of ") + errorToken( ch, rt.name));
		}
		return rt;
	}
};

static DirectmapCommandDescriptionR parseCommandDescription( std::string::const_iterator& si, const std::string::const_iterator& se, utils::FileLineInfo& posinfo, const proc::ProcessorProviderInterface* provider)
{
	DirectmapCommandDescriptionR rt;
	std::string::const_iterator posinfo_si = si;
	std::string tok;
	char ch = 0;

	try
	{
		CommandHeader hdr = CommandHeader::parse( si, se);
		rt.reset( new DirectmapCommandDescription());
		rt->cmdname = hdr.prefix + hdr.doctype;

		bool skipvalidation_input = false;	//... true if input is not validated
		bool skipvalidation_output = false;	//... true if output is not validated

		while ((ch = gotoNextToken( si, se)) != 0 && ch != ';')
		{
			switch ((DirectmapKeyword)utils::parseNextIdentifier( si, se, g_directmap_idtab))
			{
				case d_NONE:
				{
					ch = fetchNextToken( tok, si, se);
					throw std::runtime_error( std::string( "unexpected token in definition (") + g_directmap_idtab.tostring() + ") expected instead of " + errorToken( ch, tok));
				}
				case d_SKIP:
				{
					if (skipvalidation_input) throw std::runtime_error( "SKIP specified twice in command");
					skipvalidation_input = true;
					break;
				}
				case d_FILTER:
				{
					std::string filtername;
					if (parseKeyword( si, se, "INPUT"))
					{
						if (rt->inputfilterdef.filtertype) throw std::runtime_error( "input filter specified twice in command");
						rt->inputfilterdef = langbind::FilterDef::parse( si, se, provider);
						if (parseKeyword( si, se, "OUTPUT"))
						{
							if (rt->outputfilterdef.filtertype) throw std::runtime_error( "output filter specified twice in command");
							rt->outputfilterdef = langbind::FilterDef::parse( si, se, provider);
						}
					}
					else if (parseKeyword( si, se, "OUTPUT"))
					{
						if (rt->outputfilterdef.filtertype) throw std::runtime_error( "output filter specified twice in command");
						rt->outputfilterdef = langbind::FilterDef::parse( si, se, provider);
						if (parseKeyword( si, se, "INPUT"))
						{
							if (rt->inputfilterdef.filtertype) throw std::runtime_error( "input filter specified twice in command");
							rt->inputfilterdef = langbind::FilterDef::parse( si, se, provider);
						}
					}
					else
					{
						if (rt->outputfilterdef.filtertype || rt->inputfilterdef.filtertype) throw std::runtime_error( "filter specified twice in command");
						rt->inputfilterdef = rt->outputfilterdef = langbind::FilterDef::parse( si, se, provider);
					}
					break;
				}
				case d_RESULT:
				{
					std::string doctype;
					if (parseKeyword( si, se, "SKIP"))
					{
						if (skipvalidation_output) std::runtime_error( "SKIP after RESULT specified twice in command");
						skipvalidation_output = true;
						doctype = parseName( si, se, "(doctype) after RESULT SKIP");
						const types::FormDescription* fd = provider->formDescription( doctype);
						if (fd)
						{
							// ... take meta data from form if defined, even with SKIP
							rt->outputmetadata = fd->metadata();
						}
						rt->outputmetadata.setDoctype( doctype);
					}
					else
					{
						doctype = parseName( si, se, "(doctype) after RESULT");
						rt->outputform = provider->formDescription( doctype);
						if (!rt->outputform) throw std::runtime_error( std::string("referenced undefined form for output in result '") + doctype + "'");
						rt->outputmetadata = rt->outputform->metadata();
						// ... take meta data from form defined
						rt->outputmetadata.setDoctype( doctype);
					}
					if (gotoNextToken( si, se) == '{')
					{
						// ... parse command definition meta data (as highest priority)
						parseMetaData( rt->outputmetadata, si, se);
					}
					break;
				}
				case d_CALL:
				{
					if (rt->function) throw std::runtime_error( "duplicate defintion of the form function to implement the request");
					rt->functionname = parseName( si, se, "(form function name) after CALL");
					
					rt->function = provider->formFunction( rt->functionname);
					if (!rt->function) throw std::runtime_error( std::string("referenced undefined form function to implement the request'") + rt->functionname + "'");
					break;
				}
				case d_AUTHORIZE:
				{
					AuthFunction authfunc = AuthFunction::parse( si, se);
					rt->authfunction = authfunc.name;
					rt->authresource = authfunc.resource;
					break;
				}
			}
		}
		if (ch == ';')
		{
			if (!skipvalidation_input)
			{
				rt->inputform = provider->formDescription( hdr.doctype);
				if (!rt->inputform) throw std::runtime_error( std::string("referenced undefined form for input in result '") + hdr.doctype + "'");
			}
			if (!rt->function)
			{
				rt->functionname = rt->cmdname;
				rt->function = provider->formFunction( rt->functionname);
				if (!rt->function) throw std::runtime_error( std::string("referenced undefined form function to implement the request'") + rt->functionname + "'");
			}
			++si;
		}
		else
		{
			throw std::runtime_error("unexpected end of file");
		}
	}
	catch (const std::runtime_error& e)
	{
		posinfo.update( posinfo_si, si);
		LOG_ERROR << "error in standard command handler " << posinfo.logtext() << ": " << e.what();
		skipEndOfCommand( si, se);
		posinfo.update( posinfo_si, si);
		rt.reset();
	}
	posinfo.update( posinfo_si, si);
	return rt;
}


bool DirectmapProgram::loadProgram( const std::string& filename, const proc::ProcessorProviderInterface* provider)
{
	static const char* g_main_ids[] = {"COMMAND",0};
	enum MainKeyword{ k_NONE, k_COMMAND };
	static const utils::IdentifierTable g_main_idtab( false, g_main_ids);

	bool rt = true;
	utils::FileLineInfo posinfo;
	std::string source;
	try
	{
		posinfo = utils::FileLineInfo( utils::getFileStem( filename) + utils::getFileExtension( filename));
		source = utils::readSourceFileContent( filename);
	}
	catch (const std::runtime_error& e)
	{
		LOG_ERROR << "failed to load standard command handler definitions from file: " << e.what();
		return false;
	}
	std::string::const_iterator si = source.begin(), se = source.end();
	std::string::const_iterator posinfo_si = si;

	try
	{
		std::string tok;
		char ch;

		while ((ch = gotoNextToken( si, se)) != 0)
		{
			switch ((MainKeyword)utils::parseNextIdentifier( si, se, g_main_idtab))
			{
				case k_NONE:
				{
					ch = fetchNextToken( tok, si, se);
					throw std::runtime_error( std::string( "unexpected token (") + g_main_idtab.tostring() + ") expected instead of " + errorToken( ch, tok));
					break;
				}
				case k_COMMAND:
				{
					posinfo.update( posinfo_si, si);
					DirectmapCommandDescriptionR dr = parseCommandDescription( si, se, posinfo, provider);
					if (dr.get())
					{
						try
						{
							Parent::insert( dr->cmdname, dr);
							LOG_TRACE << "loaded standard command handler command " << dr->cmdname << " as " << dr->tostring();
						}
						catch (const std::runtime_error& e)
						{
							LOG_ERROR << "failed to define standard command handler command '" << dr->cmdname << "':" << e.what();
						}
					}
					break;
				}
			}
		}
	}
	catch (const std::bad_alloc& e)
	{
		posinfo.update( posinfo_si, si);
		LOG_ERROR << "unrecoverable error in standard command handler " << posinfo.logtext() << ": out of memory";
		rt = false;
	}
	catch (const std::runtime_error& e)
	{
		posinfo.update( posinfo_si, si);
		LOG_ERROR << "unrecoverable error in standard command handler " << posinfo.logtext() << ": " << e.what();
		rt = false;
	}
	return rt;
}


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
///\brief Implementation of loading programs for token normalization
///\file prgbind_normalizeProgram.cpp

#include "prgbind/normalizeProgram.hpp"
#include "langbind/normalizeFunction.hpp"
#include "utils/parseUtils.hpp"
#include "utils/fileUtils.hpp"
#include "config/programBase.hpp"
#include <string>

using namespace _Wolframe;
using namespace _Wolframe::prgbind;

static types::NormalizeFunction* createBaseFunction( const std::string& domain, const std::string& name, const std::string& arg, const types::keymap<module::NormalizeFunctionConstructorR>& constructormap)
{
	if (domain.empty()) throw std::runtime_error( "namespace of function not defined");
	types::keymap<module::NormalizeFunctionConstructorR>::const_iterator bi = constructormap.find( domain);
	if (bi == constructormap.end()) throw std::runtime_error( std::string("no constructor for namespace '") + domain + "' defined");
	try
	{
		types::NormalizeFunction* rt = bi->second->object( name, arg);
		if (!rt) throw std::runtime_error( std::string("could not find normalize function '") + domain + ":" + name + "(" + arg + ")'");
		return rt;
	}
	catch (const std::runtime_error& err)
	{
		throw std::runtime_error( std::string("could not build normalize function '") + domain + ":" + name + "(" + arg + ")' :" + err.what());
	}
}

class CombinedNormalizeFunction :public types::NormalizeFunction
{
public:
	CombinedNormalizeFunction( const std::string& name_)
		:m_name(name_){}
	CombinedNormalizeFunction( const CombinedNormalizeFunction& o)
		:types::NormalizeFunction(), m_name(o.m_name), m_steps(o.m_steps){}

	void define( const types::NormalizeFunctionR& f)
	{
		m_steps.push_back(f);
	}
	virtual const char* name() const
	{
		return m_name.c_str();
	}
	virtual types::Variant execute( const types::Variant& i) const
	{
		if (m_steps.empty()) return i;
		std::vector<types::NormalizeFunctionR>::const_iterator fi = m_steps.begin(), fe = m_steps.end();
		types::Variant rt = (*fi++)->execute( i);
		for (; fi != fe; ++fi)
		{
			rt = (*fi)->execute( rt);
		}
		return rt;
	}

	std::size_t nofSteps() const
	{
		return m_steps.size();
	}

	const types::NormalizeFunctionR& operator[]( std::size_t i) const
	{
		return m_steps[i];
	}

private:
	std::string m_name;
	std::vector<types::NormalizeFunctionR> m_steps;
};


static std::vector<std::pair<std::string,types::NormalizeFunctionR> > loadSource( const std::string& source, const types::keymap<module::NormalizeFunctionConstructorR>& constructormap)
{
	std::vector<std::pair<std::string,types::NormalizeFunctionR> > rt;
	config::PositionalErrorMessageBase ERROR(source);
	config::PositionalErrorMessageBase::Message MSG;
	static const utils::CharTable optab( "=;)(,");
	std::string prgname,tok,funcname;
	std::string::const_iterator argstart;
	std::string::const_iterator si = source.begin(), se = source.end();
	char ch;

	try
	{
		while ((ch = utils::gotoNextToken( si, se)) != 0)
		{
			switch ((ch=utils::parseNextToken( prgname, si, se, optab)))
			{
				case '#': utils::parseLine( si, se); continue; //... comment
				case ';': throw ERROR( si, "empty statement");
				case '\'':
				case '\"': throw ERROR( si, "identifier expected instead of string at start of statement");
				default:
					if (optab[ ch]) throw ERROR( si, "identifier expected at start of statement");
			}
			CombinedNormalizeFunction funcdef( prgname);
			std::string domain;

			switch ((ch=utils::parseNextToken( tok, si, se, optab)))
			{
				case '\0': throw ERROR( si, "unexpected end of program");
				case '=': break;
				default:
					if (optab[ ch]) throw ERROR( si, MSG << "'=' expected instead of '" << ch << "'");
					throw ERROR( si, MSG << "'=' expected instead of '" << tok << "'");
			}
			while (ch != ';')
			{
				switch ((ch=utils::parseNextToken( funcname, si, se, optab)))
				{
					case '\0': throw ERROR( si, "unexpected end of program");
					default:
						if (optab[ ch]) throw ERROR( si, MSG << "function name identifier expected instead of '" << ch << "'");
				}
				if ((':' == utils::gotoNextToken( si, se)))
				{
					domain = funcname;
					funcname.clear();
					++si;
					switch ((ch=utils::parseNextToken( funcname, si, se, optab)))
					{
						case '\0': throw ERROR( si, "unexpected end of program");
						default:
							if (optab[ ch]) throw ERROR( si, MSG << "function name identifier expected instead of '" << ch << "'");
					}
				}
				switch ((ch=utils::gotoNextToken( si, se)))
				{
					case '\0': throw ERROR( si, "unexpected end of program");
					case ',':
					case ';':
						funcdef.define( types::NormalizeFunctionR( createBaseFunction( domain, funcname, "", constructormap)));
						++si;
						continue;
					case '(':
						argstart = ++si;
						while ((ch=utils::parseNextToken( tok, si, se, optab)) != ')')
						{
							if (ch == '\0') throw ERROR( si, "unexpected end of program");
							if (ch == '(') throw ERROR( si, "nested expressions, bracket not closed");
							if (ch == ';') throw ERROR( si, "unexpected end of expression, bracket not closed");
						}
						funcdef.define( types::NormalizeFunctionR( createBaseFunction( domain, funcname, std::string( argstart, si-1), constructormap)));
						ch = utils::gotoNextToken( si, se);
						if (ch == ';' || ch == ',')
						{
							++si;
							continue;
						}
						if (!ch) throw ERROR( si, "unexpected end of program");
						throw ERROR( si, "unexpected token at end of expression");
					default:
						throw ERROR( si, MSG << "separator ',' or ';' expected or function arguments in '(' ')' brackets instead of '" << ch << "'");
				}
			}
			types::NormalizeFunctionR func( new CombinedNormalizeFunction( funcdef));
			rt.push_back( std::pair<std::string,types::NormalizeFunctionR>( prgname, func));
		}
		return rt;
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

bool NormalizeProgram::is_mine( const std::string& filename) const
{
	return boost::algorithm::to_lower_copy( utils::getFileExtension( filename)) == ".normalize";
}

void NormalizeProgram::loadProgram( ProgramLibrary& library, db::Database*, const std::string& filename)
{
	try
	{
		std::vector<std::pair<std::string,types::NormalizeFunctionR> > funclist
			= loadSource( utils::readSourceFileContent( filename), library.normalizeFunctionConstructorMap());

		std::vector<std::pair<std::string,types::NormalizeFunctionR> >::const_iterator ni = funclist.begin(), ne = funclist.end();
		for (; ni != ne; ++ni)
		{
			library.defineNormalizeFunction( ni->first, ni->second);
		}
	}
	catch (const config::PositionalErrorException& e)
	{
		throw config::PositionalFileErrorException( config::PositionalFileError( e, filename));
	}
	catch (const std::runtime_error& err)
	{
		throw std::runtime_error( std::string( "error in normalize program file '") + filename + "' :" + err.what());
	}
}




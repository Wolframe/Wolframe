/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
///\brief Implementation of normalization programs
///\file normalizeProgram.cpp
#include "langbind/normalizeProgram.hpp"
#include "utils/miscUtils.hpp"
#include "config/programBase.hpp"
#include <stdexcept>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

void NormalizeProgram::addConstructor( const NormalizeFunctionConstructorR& constructor)
{
	std::string domain = constructor->domain();
	m_constructormap.insert( domain, constructor);
}

bool NormalizeProgram::is_mine( const std::string& filename) const
{
	std::string ext( utils::getFileExtension( filename));
	return (boost::iequals( ext, ".normalize"));
}

void NormalizeProgram::loadProgram( const std::string& filename)
{
	try
	{
		addProgram( utils::readSourceFileContent( filename));
	}
	catch (const config::PositionalErrorException& e)
	{
		throw config::PositionalErrorException( filename, e);
	}
	catch (const std::runtime_error& err)
	{
		throw std::runtime_error( std::string( "error in normalize program file '") + filename + "' :" + err.what());
	}
}

NormalizeFunction* NormalizeProgram::createBaseFunction( const std::string& domain, const std::string& name, const std::string& arg)
{
	if (domain.empty()) throw std::runtime_error( "namespace of function not defined");
	types::keymap<NormalizeFunctionConstructorR>::const_iterator bi = m_constructormap.find( domain);
	if (bi == m_constructormap.end()) throw std::runtime_error( std::string("no constructor for namespace '") + domain + "' defined");
	try
	{
		NormalizeFunction* rt = bi->second->object( name, arg);
		if (!rt) throw std::runtime_error( std::string("could not find normalize function '") + domain + ":" + name + "(" + arg + ")'");
		return rt;
	}
	catch (const std::runtime_error& err)
	{
		throw std::runtime_error( std::string("could not build normalize function '") + domain + ":" + name + "(" + arg + ")' :" + err.what());
	}
}

class CombinedNormalizeFunction :public NormalizeFunction
{
public:
	CombinedNormalizeFunction(){}
	CombinedNormalizeFunction( const CombinedNormalizeFunction& o)
		:NormalizeFunction(), m_steps(o.m_steps){}

	void define( const NormalizeFunctionR& f)
	{
		m_steps.push_back(f);
	}
	virtual std::string execute( const std::string& i) const
	{
		if (m_steps.empty()) return i;
		std::vector<NormalizeFunctionR>::const_iterator fi = m_steps.begin(), fe = m_steps.end();
		std::string rt = (*fi++)->execute( i);
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

	const NormalizeFunctionR& operator[]( std::size_t i) const
	{
		return m_steps[i];
	}

private:
	std::vector<NormalizeFunctionR> m_steps;
};


void NormalizeProgram::addProgram( const std::string& source)
{
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
			CombinedNormalizeFunction funcdef;
			std::string domain;

			switch ((ch=utils::parseNextToken( prgname, si, se, optab)))
			{
				case ';': throw ERROR( si, "empty statement");
				case '\'':
				case '\"': throw ERROR( si, "identifier expected instead of string at start of statement");
				default:
					if (optab[ ch]) throw ERROR( si, "identifier expected at start of statement");
			}
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
						funcdef.define( NormalizeFunctionR( createBaseFunction( domain, funcname, "")));
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
						funcdef.define( NormalizeFunctionR( createBaseFunction( domain, funcname, std::string( argstart, si-1))));
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
			if (funcdef.nofSteps() == 1)
			{
				types::keymap<NormalizeFunctionR>::operator[]( prgname) = funcdef[ 0];
			}
			else
			{
				types::keymap<NormalizeFunctionR>::operator[]( prgname) = NormalizeFunctionR( new CombinedNormalizeFunction( funcdef));
			}
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


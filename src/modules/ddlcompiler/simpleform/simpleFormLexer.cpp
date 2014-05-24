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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file simpleFormLexer.cpp
///\brief Implementation of lexical analysis for the simple form DDL compiler 
#include "simpleFormLexer.hpp"
#include "utils/parseUtils.hpp"
#include "utils/sourceLineInfo.hpp"
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;
using namespace _Wolframe::langbind::simpleform;

std::string Lexem::positionLogtext() const
{
	return std::string("at line ") + boost::lexical_cast<std::string>(m_position.line()) + " column " + boost::lexical_cast<std::string>(m_position.column());
}

static const utils::CharTable g_optab( "^=@!?[]{}:");

std::string Lexer::curtoken() const
{
	if (!m_tokch)
	{
		return "end of file";
	}
	else if (m_tokch == '"' || m_tokch == '\'')
	{
		return "string";
	}
	else if (g_optab[ m_tokch])
	{
		std::string rt;
		rt.push_back( '"');
		rt.push_back( m_tokch);
		rt.push_back( '"');
		return rt;
	}
	else
	{
		return "identifier";
	}
}

Lexem Lexer::next2()
{
	Lexem rt = next();
	if (rt.id() == Lexem::FORM)
	{
		rt = Lexem( rt.position(), Lexem::Identifier, "form");
		//... 'form' is only a keyword on first level
	}
	else if (rt.id() == Lexem::STRUCT)
	{
		rt = Lexem( rt.position(), Lexem::Identifier, "struct");
		//... 'struct' is only a keyword on first level
	}
	return rt;
}

Lexem Lexer::next()
{
	enum Keywords {kw_none=0, kw_INCLUDE=1, kw_FORM=2, kw_STRUCT=3};
	static const char* kwlist[] = {"INCLUDE","FORM","STRUCT", 0};
	static const utils::IdentifierTable kwtab( false, kwlist);

	m_tokch = utils::gotoNextToken( m_itr, m_end);
	while (m_tokch == ';' || m_tokch == '#')
	{
		utils::parseNextLine( m_itr, m_end);
		m_tokch = utils::gotoNextToken( m_itr, m_end);
	}
	if (!m_tokch)
	{
		return Lexem( m_position, Lexem::EndOfFile);
	}

	m_position.update( m_itr_position, m_itr);
	m_itr_position = m_itr;

	switch ((Keywords)utils::parseNextIdentifier( m_itr, m_end, kwtab))
	{
		case kw_none: break;
		case kw_INCLUDE: return Lexem( m_position, Lexem::INCLUDE);
		case kw_FORM: return Lexem( m_position, Lexem::FORM);
		case kw_STRUCT: return Lexem( m_position, Lexem::STRUCT);
	}
	std::string tok;
	m_tokch = utils::parseNextToken( tok, m_itr, m_end, g_optab);
	switch (m_tokch)
	{
		case '\'': return Lexem( m_position, Lexem::String);
		case '\"': return Lexem( m_position, Lexem::String);
		case '{': return Lexem( m_position, Lexem::OpenStruct);
		case '}': return Lexem( m_position, Lexem::CloseStruct);
		case '[': 
		{
			if (utils::gotoNextToken( m_itr, m_end) != ']')
			{
				throw std::runtime_error("expected ']' after '['");
			}
			++m_itr;
			return Lexem( m_position, Lexem::ArrayTag);
		}
		case '!': return Lexem( m_position, Lexem::MandatoryTag);
		case '?': return Lexem( m_position, Lexem::OptionalTag);
		case '^': return Lexem( m_position, Lexem::Indirection);
		case '=': return Lexem( m_position, Lexem::Assign);
		case '@': return Lexem( m_position, Lexem::AttributeTag);
		case ':': return Lexem( m_position, Lexem::MetaDataDef);
		default:
			if (g_optab[m_tokch]) throw std::runtime_error( std::string("unexpected operator '") + (char)m_tokch + "'");
			return Lexem( m_position, Lexem::Identifier, tok);
	}
}

Lexem::Id Lexer::forwardLookup()
{
	std::string::const_iterator itr_prev = m_itr;
	utils::SourceLineInfo position_prev = m_position;
	std::string::const_iterator itr_position_prev = m_itr_position;
	char tokch_prev = m_tokch;

	Lexem lx = next();
	Lexem::Id rt = lx.id();

	m_itr_position = itr_position_prev;
	m_itr = itr_prev;
	m_position = position_prev;
	m_tokch = tokch_prev;
	return rt;
}


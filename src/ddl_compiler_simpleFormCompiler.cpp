/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
///\file ddl_compiler_SimpleFormCompiler.cpp
///\brief implementation of a compiler for a self defined form DDL called 'simple form DDL'

#include "ddl/compiler/simpleFormCompiler.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <cstring>
#include <cstddef>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace ddl;

struct Lexem
{
	enum Type
	{
		Error,
		EndOfLine,
		OpenBracket,
		CloseBracket,
		Semicolon,
		Dot,
		Form,
		Plus,
		Minus,
		End,
		As,
		Identifier,
		String,
		Int,
		Uint,
		Float,
		EndOfFile
	};
	std::string value;
	Type type;
};


static void removeComments( std::string& src)
{
	std::size_t ii;
	for (ii=0; ii<src.size(); ii++)
	{
		if (src[ii] == '#')
		{
			for (; ii<src.size() && src[ii] != '\n' && src[ii] != '\r'; ii++)
			{
				src[ii] = ' ';
			}
		}
	}
}

static std::pair<std::size_t, std::size_t> getPos( const std::string::const_iterator& start, const std::string::const_iterator& at)
{
	std::pair<std::size_t, std::size_t> pos( 1,0);
	std::string::const_iterator itr = start;
	for (; itr!=at; itr++)
	{
		if (*itr == '\n')
		{
			++pos.first;
			pos.second = 1;
		}
		else if (*itr == '\r' && *++itr != '\n')
		{
			++pos.first;
			pos.second = 2;
			if (itr == at) return pos;
		}
		else
		{
			++pos.second;
		}
	}
	return pos;
}

struct Source
{
	std::string src;
	std::string::const_iterator start;
	std::string::const_iterator at;
	Source& operator ++()	{++at; return *this;}
	char operator*()	{return *at;}
	Source( const std::string& src_) :src(src_)
	{
		removeComments( src);
		start = src.begin();
		at = src.begin();
	}
};

static bool setError( Lexem& lexem, const Source& src, const char* message)
{
	if (lexem.type == Lexem::Error) return false; ///we want only the first error
	lexem.type = Lexem::Error;
	std::pair<std::size_t, std::size_t> pos = getPos( src.start, src.at);
	lexem.value.clear();
	lexem.value.append( "[");
	lexem.value.append( boost::lexical_cast<std::string>( pos.first));
	lexem.value.append( ", ");
	lexem.value.append( boost::lexical_cast<std::string>( pos.second));
	lexem.value.append( "] ");
	lexem.value.append( message);
	return false;
}

static bool isOperator( Source& src)
{
	if (*src == '.' || *src == ';' || *src == '[' || *src == ']' || *src == '+' || *src == '-') return true;
	return false;
}

static bool parseOperator( Lexem& lexem, Source& src)
{
	switch (*src)
	{
		case '[': ++src; lexem.type = Lexem::OpenBracket; return true;
		case ']': ++src; lexem.type = Lexem::CloseBracket; return true;
		case '.': ++src; lexem.type = Lexem::Dot; return true;
		case ';': ++src; lexem.type = Lexem::Semicolon; return true;
		case '+': ++src; lexem.type = Lexem::Plus; return true;
		case '-': ++src; lexem.type = Lexem::Minus; return true;
	}
	return setError( lexem, src, "unknown operator");
}

static bool isDelimiter( Source& src)
{
	if (*src <= ' ' || isOperator(src)) return true;
	return false;
}

static bool parseString( Lexem& lexem, Source& src)
{
	char eb = *src;
	++src;
	while (*src != eb)
	{
		if (*src == '\r' || *src == '\n') return setError( lexem, src, "string not terminated");
		if (*src < ' ') return setError( lexem, src, "string contains non ascii characters");
		if (*src == '\\') ++src;
		if (*src == '\r' || *src == '\n') return setError( lexem, src, "string not terminated");
		if (*src < ' ') return setError( lexem, src, "string contains non ascii characters");
		lexem.value.push_back( *src);
		++src;
	}
	++src;
	return true;
}

static bool parseIdentifier( Lexem& lexem, Source& src)
{
	while (((*src|32) >= 'a' && (*src|32) <= 'z') || *src == '_' || (*src >= '0' && *src <= '9'))
	{
		lexem.value.push_back( *src);
		++src;
	}
	if (!isDelimiter(src))
	{
		return setError( lexem, src, "invalid identifier");
	}
	if (std::strcmp( lexem.value.c_str(), "form") == 0)
	{
		lexem.type = Lexem::Form;
	}
	else if (std::strcmp( lexem.value.c_str(), "as") == 0)
	{
		lexem.type = Lexem::As;
	}
	else if (std::strcmp( lexem.value.c_str(), "end") == 0)
	{
		lexem.type = Lexem::End;
	}
	else
	{
		lexem.type = Lexem::Identifier;
	}
	return true;
}

static bool parseNumber( Lexem& lexem, Source& src)
{
	unsigned int length = 0;
	if (*src == '-')
	{
		lexem.type = Lexem::Minus;
		lexem.value.push_back( *src);
		++src;
	}
	else if (*src == '+')
	{
		lexem.type = Lexem::Plus;
		++src;
	}
	else
	{
		lexem.type = Lexem::Plus;
	}
	while (*src >= '0' && *src <= '9')
	{
		length++;
		lexem.value.push_back( *src);
		++src;
	}
	if (length > 0)
	{
		lexem.type = (lexem.type == Lexem::Minus)?Lexem::Int:Lexem::Uint;
		if (*src == '.')
		{
			++src;
			while (*src >= '0' && *src <= '9')
			{
				lexem.value.push_back( *src);
				++src;
			}
			lexem.type = Lexem::Float;
		}
		if (!isDelimiter(src)) return setError( lexem, src, "invalid number");
	}
	return true;
}

static bool parseEndOfLine( Lexem& lexem, Source& src)
{
	if (*src == '\r')
	{
		++src;
		if (*src == '\n')
		{
			++src;
		}
		lexem.type = Lexem::EndOfLine;
		return true;
	}
	else if (*src == '\n')
	{
		++src;
		lexem.type = Lexem::EndOfLine;
		return true;
	}
	else
	{
		return setError( lexem, src, "invalid character at end of line");
	}
}

static bool nextLexem( Lexem& lexem, Source& src)
{
	lexem.value.clear();
	while (*src != 0 && *src <= ' ' && *src != '\r' && *src != '\n') ++src;
	if (*src == '\r' || *src == '\n') return parseEndOfLine( lexem, src);
	if (((*src|32) >= 'a' && (*src|32) <= 'z') || *src == '_') return parseIdentifier( lexem, src);
	if ((*src >= '0' && *src <= '9') || *src == '-' || *src == '+') return parseNumber( lexem, src);
	if (*src == '\'' || *src == '"') return parseString( lexem, src);
	if (isOperator( src)) return parseOperator( lexem, src);
	if (*src == '\0') {lexem.type=Lexem::EndOfFile; return true;}
	return setError( lexem, src, "illegal lexem");
}

static bool parseName( Source& src, Lexem& lexem, std::string& name)
{
	if (nextLexem( lexem, src))
	{
		if (lexem.type == Lexem::Identifier)
		{
			name = lexem.value;
			return true;
		}
		return setError( lexem, src, "identifier expected");
	}
	return false;
}

static bool parseElement( Source& src, Lexem& lexem, SimpleFormCompiler::Element& element)
{
	while (nextLexem( lexem, src) && lexem.type == Lexem::EndOfLine);

	if (lexem.type == Lexem::End)
	{
		return false;
	}
	if (lexem.type == Lexem::Form)
	{
		element.type = SimpleFormCompiler::Element::form_;
	}
	else if (lexem.type == Lexem::Identifier)
	{
		if (!SimpleFormCompiler::Element::getType( lexem.value.c_str(), element.type))
		{
			return setError( lexem, src, "unknown element type name");
		}
	}
	else
	{
		return setError( lexem, src, "element type name or 'ref' expected");
	}
	if (!parseName( src, lexem, element.name)) return false;

	if (nextLexem( lexem, src))
	{
		if (lexem.type == Lexem::String || lexem.type == Lexem::Int || lexem.type == Lexem::Float || lexem.type == Lexem::Uint || lexem.type == Lexem::Identifier)
		{
			element.defaultValue = lexem.value;
			if (element.defaultValue.size() == 0)
			{
				return setError( lexem, src, "empty default values are not allowed");
			}
			if (!nextLexem( lexem, src))
			{
				return setError( lexem, src, "unexpected end of file");
			}
			if (element.type == SimpleFormCompiler::Element::form_)
			{
				return setError( lexem, src, "structure can't be defined with a default value");
			}
			else if (lexem.type == Lexem::OpenBracket)
			{
				return setError( lexem, src, "array can't be defined with a default value");
			}
			else if (lexem.type != Lexem::EndOfLine)
			{
				return setError( lexem, src, "end of line expected after default value");
			}
		}
		else if (lexem.type == Lexem::OpenBracket)
		{
			if (!nextLexem( lexem, src) || lexem.type != Lexem::CloseBracket)
			{
				return setError( lexem, src, "] expected");
			}
			element.isArray = true;
			if (!nextLexem( lexem, src) || lexem.type != Lexem::EndOfLine)
			{
				return setError( lexem, src, "end of line expected");
			}
		}
		if (lexem.type == Lexem::EndOfLine)
		{
			return true;
		}
		return setError( lexem, src, "[ or end of line expected");
	}
	return setError( lexem, src, "unexpected end of file");
}

static bool parseHeader( Source& src, Lexem& lexem, std::string& name, std::string& rname)
{
	while (nextLexem( lexem, src) && lexem.type == Lexem::EndOfLine);

	if (lexem.type == Lexem::EndOfFile)
	{
		return false;
	}
	if (lexem.type == Lexem::Form)
	{
		if (!parseName( src, lexem, name)) return false;

		if (nextLexem( lexem, src))
		{
			if (lexem.type == Lexem::As)
			{
				if (!parseName( src, lexem, rname)) return false;
				if (nextLexem( lexem, src))
				{
					if (lexem.type == Lexem::EndOfLine)
					{
						return true;
					}
					return setError( lexem, src, "end of line expected");
				}
			}
			else if (lexem.type == Lexem::EndOfLine)
			{
				rname = name;
				return true;
			}
			else
			{
				return setError( lexem, src, "end of line expected");
			}
		}
	}
	return setError( lexem, src, "'struct' expected");
}

static bool parseStruct( Source& src, Lexem& lexem, SimpleFormCompiler& ds)
{
	SimpleFormCompiler::Struct st;
	std::string rname;

	if (parseHeader( src, lexem, st.name, rname))
	{
		while (lexem.type != Lexem::End && lexem.type != Lexem::Error)
		{
			SimpleFormCompiler::Element ee;
			if (parseElement( src, lexem, ee))
			{
				st.elements.push_back( ee);
			}
		}

		if (lexem.type != Lexem::Error)
		{
			if (!ds.define( rname, st))
			{
				setError( lexem, src, "duplicate struct definition");
			}
			return true;
		}
	}
	if (lexem.type==Lexem::EndOfFile)
	{
		return true;
	}
	return setError( lexem, src, "structure definition expected");
}

static bool parseDefinition( Source& src, SimpleFormCompiler& parser, Lexem& lexem)
{
	while (parseStruct( src, lexem, parser) && lexem.type != Lexem::EndOfFile);
	return lexem.type!=Lexem::Error;
}

std::size_t SimpleFormCompiler::calcElementSize( std::size_t idx, std::size_t depht)
{
	std::size_t rt = 0;
	if (depht > ar.size()) return 0;
	std::vector<Element>::iterator eitr = ar[ idx].elements.begin(), eend = ar[ idx].elements.end();
	while (eitr != eend)
	{
		if (eitr->size == 0)
		{
			if (eitr->ref == -1) return 0;
			eitr->size = calcElementSize( eitr->ref, depht+1);
			if (eitr->size) return 0;
		}
		rt += eitr->size;
		++eitr;
	}
	return rt;
}

bool SimpleFormCompiler::compile( const std::string& srcstring, StructType&, std::string& error_)
{
	Source src( srcstring);
	Lexem lexem;
	std::stringstream err;
	bool rt = true;

	if (!parseDefinition( src, *this, lexem))
	{
		err << lexem.value << std::endl;
		error_ = err.str();
		return false;
	}
	std::vector<Struct>::iterator itr = ar.begin(), end = ar.end();
	std::size_t ii = 0;
	while (itr != end)
	{
		if (linkmap.find( itr->name) != linkmap.end())
		{
			err << "duplicate definition of struct '" << itr->name << "'" << std::endl;
			rt = false;
		}
		linkmap[ itr->name] = ii;
		++itr;
		++ii;
	}
	itr = ar.begin(), end = ar.end();
	while (itr != end)
	{
		std::vector<Element>::iterator eitr = itr->elements.begin(), eend = itr->elements.end();
		while (eitr != eend)
		{
			if (eitr->type == Element::form_)
			{
				if (linkmap.find( eitr->name) == linkmap.end())
				{
					err << "unresolved reference of struct '" << eitr->name << "'" << std::endl;
					rt = false;
				}
				else
				{
					eitr->ref = linkmap[ itr->name];
				}
			}
			++eitr;
		}
		++itr;
	}
	itr = ar.begin(), end = ar.end(), ii=0;
	while (itr != end)
	{
		if (itr->size == 0)
		{
			itr->size = calcElementSize( ii);
		}
		++itr;
		++ii;
	}
	return rt;
}


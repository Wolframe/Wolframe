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

namespace
{
struct Element
{
public:
	enum Type
	{
		double_,float_,long_,ulong_,int_,uint_,short_,ushort_,char_,uchar_,string_,form_
	};

	static const char* typeName( Type tp)
	{
		static const char* ar[] = {"double","float","long","ulong","int","uint","short","ushort","char","uchar","string","form",0};
		return ar[ (int)tp];
	}

	static bool getType( const char* name, Type& tp)
	{
		const char* rt;
		unsigned int ii;
		for (ii=0,rt=typeName((Type)(ii)); rt!=0; ii++,rt=typeName((Type)(ii)))
		{
			if (std::strcmp( rt, name) == 0)
			{
				tp = (Type)ii;
				return true;
			}
		}
		return false;
	}

	Type type;
	std::string name;
	std::string defaultValue;
	int ref;
	std::size_t size;
	bool isArray;
	bool isAttribute;

	Element() :ref(-1),size(0U),isArray(false),isAttribute(false){}
};

struct Struct
{
	std::string name;
	std::vector<Element> elements;
	std::size_t size;
	bool isPublic;

	Struct() :size(0),isPublic(false){}
};

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

struct Lexem
{
	enum Type
	{
		Error,
		EndOfLine,
		At,
		OpenBracket,
		CloseBracket,
		Semicolon,
		Dot,
		Form,
		Public,
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

class SimpleFormCompilerImpl
{
public:

	explicit SimpleFormCompilerImpl( const std::string& srcstring)
		:src( srcstring){}

	bool compile( StructType& result, std::string& error);


	const Struct* get( const std::string& name) const
	{
		std::map<std::string,std::size_t>::const_iterator itr = linkmap.find( name), end = linkmap.end();
		if (itr == end) return 0;
		return &ar[ itr->second];
	}

	bool define( const std::string& name, const Struct& ee)
	{
		std::map<std::string,std::size_t>::const_iterator itr = linkmap.find( name);
		if (itr == linkmap.end()) return false;
		linkmap[ name] = ar.size();
		ar.push_back( ee);
		return true;
	}

	bool parseDefinition( Lexem& lexem);
	bool parseStruct( Lexem& lexem);
	bool parseHeader( Lexem& lexem, std::string& name, std::string& rname, bool& isPublic);
	bool nextLexem( Lexem& lexem);
	bool parseName( Lexem& lexem, std::string& name);
	bool parseElement( Lexem& lexem, Element& element);
	bool parseEndOfLine( Lexem& lexem);
	bool parseNumber( Lexem& lexem);
	bool parseIdentifier( Lexem& lexem);
	bool parseString( Lexem& lexem);
	bool isDelimiter();
	bool parseOperator( Lexem& lexem);
	bool isOperator();
	bool setError( Lexem& lexem, const char* message);
	void getStruct( std::size_t idx, StructType& st);

private:
	void error( const std::string& msg) {errors.push_back( msg);}
	std::size_t calcElementSize( std::size_t idx, std::size_t depht=0);
	std::map<std::string,std::size_t> linkmap;
	std::vector<Struct> ar;
	std::vector<std::string> errors;
	Source src;
};


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

bool SimpleFormCompilerImpl::setError( Lexem& lexem, const char* message)
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

bool SimpleFormCompilerImpl::isOperator()
{
	if (*src == '@' || *src == '.' || *src == ';' || *src == '[' || *src == ']' || *src == '+' || *src == '-') return true;
	return false;
}

bool SimpleFormCompilerImpl::parseOperator( Lexem& lexem)
{
	switch (*src)
	{
		case '@': ++src; lexem.type = Lexem::At; return true;
		case '[': ++src; lexem.type = Lexem::OpenBracket; return true;
		case ']': ++src; lexem.type = Lexem::CloseBracket; return true;
		case '.': ++src; lexem.type = Lexem::Dot; return true;
		case ';': ++src; lexem.type = Lexem::Semicolon; return true;
		case '+': ++src; lexem.type = Lexem::Plus; return true;
		case '-': ++src; lexem.type = Lexem::Minus; return true;
	}
	return setError( lexem, "unknown operator");
}

bool SimpleFormCompilerImpl::isDelimiter()
{
	if (*src <= ' ' || isOperator()) return true;
	return false;
}

bool SimpleFormCompilerImpl::parseString( Lexem& lexem)
{
	char eb = *src;
	++src;
	while (*src != eb)
	{
		if (*src == '\r' || *src == '\n') return setError( lexem, "string not terminated");
		if (*src < ' ') return setError( lexem, "string contains non ascii characters");
		if (*src == '\\') ++src;
		if (*src == '\r' || *src == '\n') return setError( lexem, "string not terminated");
		if (*src < ' ') return setError( lexem, "string contains non ascii characters");
		lexem.value.push_back( *src);
		++src;
	}
	++src;
	return true;
}

bool SimpleFormCompilerImpl::parseIdentifier( Lexem& lexem)
{
	while (((*src|32) >= 'a' && (*src|32) <= 'z') || *src == '_' || (*src >= '0' && *src <= '9'))
	{
		lexem.value.push_back( *src);
		++src;
	}
	if (!isDelimiter())
	{
		return setError( lexem, "invalid identifier");
	}
	if (std::strcmp( lexem.value.c_str(), "form") == 0)
	{
		lexem.type = Lexem::Form;
	}
	else if (std::strcmp( lexem.value.c_str(), "public") == 0)
	{
		lexem.type = Lexem::Public;
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

bool SimpleFormCompilerImpl::parseNumber( Lexem& lexem)
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
		if (!isDelimiter()) return setError( lexem, "invalid number");
	}
	return true;
}

bool SimpleFormCompilerImpl::parseEndOfLine( Lexem& lexem)
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
		return setError( lexem, "invalid character at end of line");
	}
}

bool SimpleFormCompilerImpl::nextLexem( Lexem& lexem)
{
	lexem.value.clear();
	while (*src != 0 && *src <= ' ' && *src != '\r' && *src != '\n') ++src;
	if (*src == '\r' || *src == '\n') return parseEndOfLine( lexem);
	if (((*src|32) >= 'a' && (*src|32) <= 'z') || *src == '_') return parseIdentifier( lexem);
	if ((*src >= '0' && *src <= '9') || *src == '-' || *src == '+') return parseNumber( lexem);
	if (*src == '\'' || *src == '"') return parseString( lexem);
	if (isOperator()) return parseOperator( lexem);
	if (*src == '\0') {lexem.type=Lexem::EndOfFile; return true;}
	return setError( lexem, "illegal lexem");
}

bool SimpleFormCompilerImpl::parseName( Lexem& lexem, std::string& name)
{
	if (nextLexem( lexem))
	{
		if (lexem.type == Lexem::Identifier)
		{
			name = lexem.value;
			return true;
		}
		return setError( lexem, "identifier expected");
	}
	return false;
}

bool SimpleFormCompilerImpl::parseElement( Lexem& lexem, Element& element)
{
	while (nextLexem( lexem) && lexem.type == Lexem::EndOfLine);

	if (lexem.type == Lexem::End)
	{
		return false;
	}
	if (lexem.type == Lexem::At)
	{
		element.isAttribute = true;
		while (nextLexem( lexem) && lexem.type == Lexem::EndOfLine);
	}
	if (lexem.type == Lexem::Form)
	{
		element.type = Element::form_;
		if (element.isAttribute)
		{
			return setError( lexem, "form references cannot be attributes");
		}
	}
	else if (lexem.type == Lexem::Identifier)
	{
		if (!Element::getType( lexem.value.c_str(), element.type))
		{
			return setError( lexem, "unknown element type name");
		}
	}
	else
	{
		return setError( lexem, "element type name or 'form' expected");
	}
	if (!parseName( lexem, element.name)) return false;

	if (nextLexem( lexem))
	{
		if (lexem.type == Lexem::String || lexem.type == Lexem::Int || lexem.type == Lexem::Float || lexem.type == Lexem::Uint || lexem.type == Lexem::Identifier)
		{
			element.defaultValue = lexem.value;
			if (element.defaultValue.size() == 0)
			{
				return setError( lexem, "empty default values are not allowed");
			}
			if (!nextLexem( lexem))
			{
				return setError( lexem, "unexpected end of file");
			}
			if (element.type == Element::form_)
			{
				return setError( lexem, "structure can't be defined with a default value");
			}
			else if (lexem.type == Lexem::OpenBracket)
			{
				return setError( lexem, "array can't be defined with a default value");
			}
			else if (lexem.type != Lexem::EndOfLine)
			{
				return setError( lexem, "end of line expected after default value");
			}
		}
		else if (lexem.type == Lexem::OpenBracket)
		{
			if (!nextLexem( lexem) || lexem.type != Lexem::CloseBracket)
			{
				return setError( lexem, "] expected");
			}
			element.isArray = true;
			if (!nextLexem( lexem) || lexem.type != Lexem::EndOfLine)
			{
				return setError( lexem, "end of line expected");
			}
		}
		if (lexem.type == Lexem::EndOfLine)
		{
			return true;
		}
		return setError( lexem, "[ or end of line expected");
	}
	return setError( lexem, "unexpected end of file");
}

bool SimpleFormCompilerImpl::parseHeader( Lexem& lexem, std::string& name, std::string& rname, bool& isPublic)
{
	while (nextLexem( lexem) && lexem.type == Lexem::EndOfLine);

	if (lexem.type == Lexem::EndOfFile)
	{
		return false;
	}
	if (lexem.type == Lexem::Public)
	{
		isPublic = true;
		while (nextLexem( lexem) && lexem.type == Lexem::EndOfLine);
	}
	if (lexem.type == Lexem::Form)
	{
		if (!parseName( lexem, name)) return false;

		if (nextLexem( lexem))
		{
			if (lexem.type == Lexem::As)
			{
				if (!parseName( lexem, rname)) return false;
				if (nextLexem( lexem))
				{
					if (lexem.type == Lexem::EndOfLine)
					{
						return true;
					}
					return setError( lexem, "end of line expected");
				}
			}
			else if (lexem.type == Lexem::EndOfLine)
			{
				rname = name;
				return true;
			}
			else
			{
				return setError( lexem, "end of line expected");
			}
		}
	}
	return setError( lexem, "'form' expected");
}

bool SimpleFormCompilerImpl::parseStruct( Lexem& lexem)
{
	Struct st;
	std::string rname;

	if (parseHeader( lexem, st.name, rname, st.isPublic))
	{
		while (lexem.type != Lexem::End && lexem.type != Lexem::Error)
		{
			Element ee;
			if (parseElement( lexem, ee))
			{
				st.elements.push_back( ee);
			}
		}
		if (lexem.type != Lexem::Error)
		{
			if (!define( rname, st))
			{
				setError( lexem, "duplicate struct definition");
			}
			return true;
		}
	}
	if (lexem.type==Lexem::EndOfFile)
	{
		return true;
	}
	return setError( lexem, "structure definition expected");
}

bool SimpleFormCompilerImpl::parseDefinition( Lexem& lexem)
{
	while (parseStruct( lexem) && lexem.type != Lexem::EndOfFile);
	return lexem.type!=Lexem::Error;
}

std::size_t SimpleFormCompilerImpl::calcElementSize( std::size_t idx, std::size_t depht)
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

void SimpleFormCompilerImpl::getStruct( std::size_t idx, StructType& st)
{
	std::vector<Element>::iterator eitr = ar[ idx].elements.begin(), eend = ar[ idx].elements.end();
	while (eitr != eend)
	{
		StructType elem;
		if (eitr->ref)
		{
			getStruct( eitr->ref, elem);
		}
		else
		{
			AtomicType::Type atype = AtomicType::string_;
			switch (eitr->type)
			{
				case Element::double_: atype = AtomicType::double_; break;
				case Element::float_: atype = AtomicType::float_; break;
				case Element::long_: atype = AtomicType::long_; break;
				case Element::ulong_: atype = AtomicType::ulong_; break;
				case Element::int_: atype = AtomicType::int_; break;
				case Element::uint_: atype = AtomicType::uint_; break;
				case Element::short_: atype = AtomicType::short_; break;
				case Element::ushort_: atype = AtomicType::ushort_; break;
				case Element::char_: atype = AtomicType::char_; break;
				case Element::uchar_: atype = AtomicType::uchar_; break;
				case Element::string_: atype = AtomicType::string_; break;
				case Element::form_: throw std::logic_error( "illegal atomic element form");
			}
			AtomicType atm( atype);
			if (eitr->defaultValue.size())
			{
				atm.set( eitr->defaultValue);
			}
			elem = atm;
		}
		if (eitr->isArray)
		{
			StructType vec;
			vec.defineAsVector( elem);
			elem = vec;
		}
		if (eitr->isAttribute)
		{
			st.defineAttribute( eitr->name, elem);
		}
		else
		{
			st.defineContent( eitr->name, elem);
		}
		++eitr;
	}
}

bool SimpleFormCompilerImpl::compile( StructType& result_, std::string& error_)
{
	Lexem lexem;
	std::stringstream err;
	bool rt = true;

	if (!parseDefinition( lexem))
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
			if (itr->size == 0)
			{
				err << "circular references in DDL definition (" << itr->name << ")" << std::endl;
				rt = false;
			}
		}
		++itr;
		++ii;
	}
	itr = ar.begin(), end = ar.end(), ii=0;
	while (itr != end)
	{
		if (itr->isPublic == 0)
		{
			StructType st;
			getStruct( ii, st);
			result_ = st;
		}
		++itr;
		++ii;
	}
	return rt;
}

}///anonymous namespace

bool SimpleFormCompiler::compile( const std::string& srcstring, StructType& result_, std::string& error_)
{
	SimpleFormCompilerImpl impl( srcstring);
	return impl.compile( result_, error_);
}


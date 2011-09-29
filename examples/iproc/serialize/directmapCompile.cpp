#include "directmapCompile.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <cstring>
#include <cstddef>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using namespace _Wolframe;
using namespace directmap;

static bool readFile( const char* fn, std::string& out)
{
	char buf;
	std::fstream ff;
	ff.open( fn, std::ios::in | std::ios::binary);
	while (ff.read( &buf, sizeof(buf)))
	{
		out.push_back( buf);
	}
	bool rt = ((ff.rdstate() & std::ifstream::eofbit) != 0);
	ff.close();
	return rt;
}

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
		Struct,
		Ref,
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
	std::string m_value;
	Type m_type;
};

struct Source
{
	std::string m_src;
	char* m_itr;

	Source( const std::string& src)
		:m_src(const_cast<char*>(src.c_str())),m_itr(const_cast<char*>(m_src.c_str()))
	{
		removeComments();
	}

	Source( const Source& o)
		:m_src(o.m_src),m_itr(o.m_itr) {}

	Source& operator++() {m_itr++; return *this;}
	Source operator++(int) {Source rt(*this); m_itr++; return rt;}
	char operator*() {return *m_itr;}

	void removeComments()
	{
		std::size_t ii;
		for (ii=0; ii<m_src.size(); ii++)
		{
			if (m_src[ii] == '#')
			{
				for (; ii<m_src.size() && m_src[ii] != '\n' && m_src[ii] != '\r'; ii++)
				{
					m_src[ii] = ' ';
				}
			}
		}
	}

	unsigned int line() const
	{
		unsigned int ln=1;
		std::size_t ii,nn=m_itr-m_src.c_str();
		for (ii=0; ii<nn; ii++)
		{
			if (m_src[ii] == '\n' || (m_src[ii] == '\r' && m_src[ii+1] != '\n')) ln++;
		}
		return ln;
	}

	unsigned int pos() const
	{
		char* pp;
		for (pp=const_cast<char*>(m_itr); pp!=const_cast<char*>(m_src.c_str()) && *pp != '\n' && *pp != '\r'; pp--);
		return (m_itr-pp+1);
	}
};

static bool setError( Lexem& lexem, const Source& src, const char* message)
{
		if (lexem.m_type == Lexem::Error) return false; ///we want only the first error
		lexem.m_type = Lexem::Error;
		lexem.m_value.clear();
		lexem.m_value.append( "[");
		lexem.m_value.append( boost::lexical_cast<std::string>( src.line()));
		lexem.m_value.append( ", ");
		lexem.m_value.append( boost::lexical_cast<std::string>( src.pos()));
		lexem.m_value.append( "] ");
		lexem.m_value.append( message);
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
		case '[': ++src; lexem.m_type = Lexem::OpenBracket; return true;
		case ']': ++src; lexem.m_type = Lexem::CloseBracket; return true;
		case '.': ++src; lexem.m_type = Lexem::Dot; return true;
		case ';': ++src; lexem.m_type = Lexem::Semicolon; return true;
		case '+': ++src; lexem.m_type = Lexem::Plus; return true;
		case '-': ++src; lexem.m_type = Lexem::Minus; return true;
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
	char eb = *src++;
	while (*src != eb)
	{
		if (*src == '\r' || *src == '\n') return setError( lexem, src, "string not terminated");
		if (*src < ' ') return setError( lexem, src, "string contains non ascii characters");
		if (*src == '\\') src++;
		if (*src == '\r' || *src == '\n') return setError( lexem, src, "string not terminated");
		if (*src < ' ') return setError( lexem, src, "string contains non ascii characters");
		lexem.m_value.push_back( *src);
		++src;
	}
	++src;
	return true;
}

static bool parseIdentifier( Lexem& lexem, Source& src)
{
	while (((*src|32) >= 'a' && (*src|32) <= 'z') || *src == '_' || (*src >= '0' && *src <= '9'))
	{
		lexem.m_value.push_back( *src);
		++src;
	}
	if (!isDelimiter(src))
	{
		return setError( lexem, src, "invalid identifier");
	}
	if (std::strcmp( lexem.m_value.c_str(), "struct") == 0)
	{
		lexem.m_type = Lexem::Struct;
	}
	else if (std::strcmp( lexem.m_value.c_str(), "ref") == 0)
	{
		lexem.m_type = Lexem::Ref;
	}
	else if (std::strcmp( lexem.m_value.c_str(), "as") == 0)
	{
		lexem.m_type = Lexem::As;
	}
	else if (std::strcmp( lexem.m_value.c_str(), "end") == 0)
	{
		lexem.m_type = Lexem::End;
	}
	else
	{
		lexem.m_type = Lexem::Identifier;
	}
	return true;
}

static bool parseNumber( Lexem& lexem, Source& src)
{
	unsigned int length = 0;
	if (*src == '-')
	{
		lexem.m_type = Lexem::Minus;
		lexem.m_value.push_back( *src);
		++src;
	}
	else if (*src == '+')
	{
		lexem.m_type = Lexem::Plus;
		++src;
	}
	else
	{
		lexem.m_type = Lexem::Plus;
	}
	while (*src >= '0' && *src <= '9')
	{
		length++;
		lexem.m_value.push_back( *src);
		++src;
	}
	if (length > 0)
	{
		lexem.m_type = (lexem.m_type == Lexem::Minus)?Lexem::Int:Lexem::Uint;
		if (*src == '.')
		{
			++src;
			while (*src >= '0' && *src <= '9')
			{
				lexem.m_value.push_back( *src);
				++src;
			}
			lexem.m_type = Lexem::Float;
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
		lexem.m_type = Lexem::EndOfLine;
		return true;
	}
	else if (*src == '\n')
	{
		lexem.m_type = Lexem::EndOfLine;
		return true;
	}
	else
	{
		return setError( lexem, src, "invalid character at end of line");
	}
}

static bool nextLexem( Lexem& lexem, Source& src)
{
	lexem.m_value.clear();
	while (*src != 0 && *src <= ' ' && *src != '\r' && *src != '\n') src++;
	if (*src == '\r' || *src == '\n') return parseEndOfLine( lexem, src);
	if (((*src|32) >= 'a' && (*src|32) <= 'z') || *src == '_') return parseIdentifier( lexem, src);
	if ((*src >= '0' && *src <= '9') || *src == '-' || *src == '+') return parseNumber( lexem, src);
	if (*src == '\'' || *src == '"') return parseString( lexem, src);
	if (isOperator( src)) return parseOperator( lexem, src);
	if (*src == '\0') {lexem.m_type=Lexem::EndOfFile; return true;}
	return setError( lexem, src, "illegal lexem");
}

static bool parseName( Source& src, Lexem& lexem, std::string& name)
{
	if (nextLexem( lexem, src))
	{
		if (lexem.m_type == Lexem::Identifier)
		{
			name = lexem.m_value;
			return true;
		}
		return setError( lexem, src, "identifier expected");
	}
	return false;
}

static bool parseElement( Source& src, Lexem& lexem, SymbolTable::Element& element)
{
	while (nextLexem( lexem, src) && lexem.m_type == Lexem::EndOfLine);

	if (lexem.m_type == Lexem::End)
	{
		return false;
	}
	if (lexem.m_type == Lexem::Ref)
	{
		element.m_type = SymbolTable::Element::struct_;
	}
	else if (lexem.m_type == Lexem::Identifier)
	{
		if (!SymbolTable::Element::getType( lexem.m_value.c_str(), element.m_type))
		{
			return setError( lexem, src, "unknown element type name");
		}
	}
	else
	{
		return setError( lexem, src, "element type name or 'ref' expected");
	}
	if (!parseName( src, lexem, element.m_name)) return false;

	if (nextLexem( lexem, src))
	{
		if (lexem.m_type == Lexem::String || lexem.m_type == Lexem::Int || lexem.m_type == Lexem::Float || lexem.m_type == Lexem::Uint || lexem.m_type == Lexem::Identifier)
		{
			element.m_default = lexem.m_value;
			if (element.m_default.size() == 0)
			{
				return setError( lexem, src, "empty default values are not allowed");
			}
			if (!nextLexem( lexem, src))
			{
				return setError( lexem, src, "unexpected end of file");
			}
			if (element.m_type == SymbolTable::Element::struct_)
			{
				return setError( lexem, src, "structure can't be defined with a default value");
			}
			else if (lexem.m_type == Lexem::OpenBracket)
			{
				return setError( lexem, src, "array can't be defined with a default value");
			}
			else if (lexem.m_type != Lexem::EndOfLine)
			{
				return setError( lexem, src, "end of line expected after default value");
			}
		}
		else if (lexem.m_type == Lexem::OpenBracket)
		{
			if (!nextLexem( lexem, src) || lexem.m_type != Lexem::CloseBracket)
			{
				return setError( lexem, src, "] expected");
			}
			element.m_isArray = true;
			if (!nextLexem( lexem, src) || lexem.m_type != Lexem::EndOfLine)
			{
				return setError( lexem, src, "end of line expected");
			}
		}
		if (lexem.m_type == Lexem::EndOfLine)
		{
			return true;
		}
		return setError( lexem, src, "[ or end of line expected");
	}
	return setError( lexem, src, "unexpected end of file");
}

static bool parseHeader( Source& src, Lexem& lexem, std::string& name, std::string& rname)
{
	while (nextLexem( lexem, src) && lexem.m_type == Lexem::EndOfLine);

	if (lexem.m_type == Lexem::EndOfFile)
	{
		return false;
	}
	if (lexem.m_type == Lexem::Struct)
	{
		if (!parseName( src, lexem, name)) return false;

		if (nextLexem( lexem, src))
		{
			if (lexem.m_type == Lexem::As)
			{
				if (!parseName( src, lexem, rname)) return false;
				if (nextLexem( lexem, src))
				{
					if (lexem.m_type == Lexem::EndOfLine)
					{
						return true;
					}
					return setError( lexem, src, "end of line expected");
				}
			}
			else if (lexem.m_type == Lexem::EndOfLine)
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

static bool parseStruct( Source& src, Lexem& lexem, SymbolTable& ds)
{
	SymbolTable::Struct st;
	std::string rname;

	if (parseHeader( src, lexem, st.m_name, rname))
	{
		while (lexem.m_type != Lexem::End && lexem.m_type != Lexem::Error)
		{
			SymbolTable::Element ee;
			if (parseElement( src, lexem, ee))
			{
				st.m_elements.push_back( ee);
			}
		}

		if (lexem.m_type != Lexem::Error)
		{
			if (!ds.define( rname, st))
			{
				setError( lexem, src, "duplicate struct definition");
			}
			return true;
		}
	}
	if (lexem.m_type==Lexem::EndOfFile)
	{
		return true;
	}
	return setError( lexem, src, "structure definition expected");
}

static bool parseSymbolTable( Source& src, SymbolTable& SymbolTable, Lexem& lexem)
{
	while (parseStruct( src, lexem, SymbolTable) && lexem.m_type != Lexem::EndOfFile);
	return lexem.m_type!=Lexem::Error;
}

bool SymbolTable::compile( const char* filename, std::string& error_)
{
	std::string srcstring;
	if (!readFile( filename, srcstring))
	{
		std::stringstream err;
		err << "failed to read input file " << filename << std::endl;
		error_ = err.str();
		return false;
	}
	Source src( srcstring);
	Lexem lexem;
	if (!parseSymbolTable( src, *this, lexem))
	{
		std::stringstream err;
		err << filename << ": " << lexem.m_value << std::endl;
		error_ = err.str();
		return false;
	}
	return true;
}


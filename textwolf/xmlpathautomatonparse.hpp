/*
---------------------------------------------------------------------
    The template library textwolf implements an input iterator on
    a set of XML path expressions without backward references on an
    STL conforming input iterator as source. It does no buffering
    or read ahead and is dedicated for stream processing of XML
    for a small set of XML queries.
    Stream processing in this context refers to processing the
    document without buffering anything but the current result token
    processed with its tag hierarchy information.

    Copyright (C) 2010,2011,2012 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of textwolf can be found at 'http://github.com/patrickfrey/textwolf'
	For documentation see 'http://patrickfrey.github.com/textwolf'

--------------------------------------------------------------------
*/
#ifndef __TEXTWOLF_XML_PATH_AUTOMATON_PARSE_HPP__
#define __TEXTWOLF_XML_PATH_AUTOMATON_PARSE_HPP__
#include "textwolf/xmlpathautomatonparse.hpp"
#include <limits>
#include <string>
#include <vector>
#include <cstring>
#include <cstddef>
#include <stdexcept>

namespace textwolf {

///\class XMLPathSelectAutomatonParser
///\tparam SrcCharSet_ character set of the automaton definition source
///\tparam AtmCharSet_ character set of the token defintions of the automaton
///\brief Automaton to define XML path expressions and assign types (int values) to them
template <class SrcCharSet_=charset::UTF8, class AtmCharSet_=charset::UTF8>
class XMLPathSelectAutomatonParser :public XMLPathSelectAutomaton
{
public:
	typedef XMLPathSelectAutomaton<CharSet_> ThisAutomaton;
	typedef XMLPathSelectAutomatonParser This;
	typedef TextScanner<CStringIterator,SrcCharSet_> SrcScanner;

public:
	///\brief Constructor
	XMLPathSelectAutomatonParser(){}

	int addExpression( int typeidx, const char* expr, std::size_t exprsize)
	{
		CStringIterator itr( expr, exprsize);
		SrcScanner src( itr);

		PathElement expr( this);
		for (; *src; ++src, skipSpaces( src))
		{
			switch (*src)
			{
				case '/':
				{
					++src;
					if (*src == '/')
					{
						++src;
						if (*src == '@')
						{
							std::string id = parseIdentifier( ++src);
							if (id.empty()) return src.getPosition()+1;
							expr --( id.c_str());
						}
						else
						{
							std::string id = parseIdentifier( src);
							if (id.empty()) return src.getPosition()+1;
							expr --[ id.c_str()];
						}
					}
					else
					{
						if (*src == '@')
						{
							std::string id = parseIdentifier( ++src);
							if (id.empty()) return src.getPosition()+1;
							expr( id.c_str());
						}
						else
						{
							std::string id = parseIdentifier( src);
							if (id.empty()) return src.getPosition()+1;
							expr[ id.c_str()];
						}
					}
					continue;
				}
				case '[':
				{
					// Range
					int range_start = -1;
					int range_end = -1;
					++ii; skipSpaces( expr, ii);
					range_start = parseNum( expr, ii);
					if (range_start < 0) return ii+1;
					skipSpaces( expr, ii);

					if (expr[ii] == ',')
					{
						++ii; skipSpaces( expr, ii);
						if (expr[ii] == ']')
						{
							expr.FROM( range_start);
						}
						else
						{
							range_end = parseNum( expr, ii);
							if (range_end < 0) return ii+1;
							++ii; skipSpaces( expr, ii);
							if (expr[ii] != ']') return ii+1;
							expr.RANGE( range_start, range_end);
						}
					}
					else if (expr[ii] == ']')
					{
						range_start = range_end;
						expr.INDEX( range_start);
					}
					continue;
				}
			}
		}
		expr.assignType( typeidx);
	}

private:
	static void skipSpaces( SrcScanner& src)
	{
		for (; src->control() == Space; ++src);
	}

	static int parseNum( SrcScanner& src)
	{
		std::size_t kk=ii,std::string num;
		for (; *src>='0' && *src<='9';++src) num.push_back( *src);
		if (num.size() == 0 || num.size() > 8) return -1;
		return std::atoi( num.c_str());
	}

	static std::string parseIdentifier( SrcScanner& src)
	{
		std::string rt;
		for (; src->control() == Undef || src->control() == Any; ++src)
		{
			AtmCharSet_::print( *src, rt);
		}
		AtmCharSet_::print( 0, rt);
		return rt;
	}
};

} //namespace
#endif

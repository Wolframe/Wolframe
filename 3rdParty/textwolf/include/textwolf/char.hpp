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

    Copyright (C) 2010,2011,2012,2013,2014 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3.0 of the License, or (at your option) any later version.

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
/// \file textwolf/char.hpp
/// \brief Definition of unicode characters
#ifndef __TEXTWOLF_CHAR_HPP__
#define __TEXTWOLF_CHAR_HPP__
#include <cstddef>

#ifdef BOOST_VERSION
#include <boost/cstdint.hpp>
namespace textwolf {
	/// \typedef UChar
	/// \brief Unicode character type
	typedef boost::uint32_t UChar;
	typedef boost::uint64_t EChar;
}//namespace
#else
#ifdef _MSC_VER
#pragma warning(disable:4290)
#include <BaseTsd.h>
namespace textwolf {
	/// \typedef UChar
	/// \brief Unicode character type
	typedef DWORD32 UChar;
	typedef DWORD64 EChar;
}//namespace
#else
#include <stdint.h>
namespace textwolf {
	/// \typedef UChar
	/// \brief Unicode character type
	typedef uint32_t UChar;
	typedef uint64_t EChar;
}//namespace
#endif
#endif

namespace textwolf {
/// \class CharMap
/// \brief Character map for fast typing of a character byte
/// \tparam RESTYPE result type of the map
/// \tparam nullvalue_ default intitialization value of the map
/// \tparam RANGE domain of the input values of the map
template <typename RESTYPE, RESTYPE nullvalue_, int RANGE=256>
class CharMap
{
public:
	typedef RESTYPE valuetype;
	enum Constant {nullvalue=nullvalue_};

private:
	RESTYPE ar[ RANGE];		//< the map elements
public:
	/// \brief Constructor
	CharMap()									{for (unsigned int ii=0; ii<RANGE; ii++) ar[ii]=(valuetype)nullvalue;}
	/// \brief Define the values of the elements in the interval [from,to]
	/// \param[in] from start of the input intervall (belongs also to the input)
	/// \param[in] to end of the input intervall (belongs also to the input)
	/// \param[in] value value assigned to all elements in  [from,to]
	CharMap& operator()( unsigned char from, unsigned char to, valuetype value)	{for (unsigned int ii=from; ii<=to; ii++) ar[ii]=value; return *this;}
	/// \brief Define the values of the single element at 'at'
	/// \param[in] at the input element
	/// \param[in] value value assigned to the element 'at'
	CharMap& operator()( unsigned char at, valuetype value)				{ar[at] = value; return *this;}
	/// \brief Read the element assigned to 'ii'
	/// \param[in] ii the input element queried
	/// \return the element at 'ii'
	valuetype operator []( unsigned char ii) const					{return ar[ii];}
};

/// \enum ControlCharacter
/// \brief Enumeration of control characters needed as events for XML scanner statemachine
enum ControlCharacter
{
	Undef=0,		//< not defined (beyond ascii)
	EndOfText,		//< end of data (EOF,EOD,.)
	EndOfLine,		//< end of line
	Cntrl,			//< control character
	Space,			//< space, tab, etc..
	Amp,			//< ampersant ('&')
	Lt,			//< lesser than '<'
	Equal,			//< equal '='
	Gt,			//< greater than '>'
	Slash,			//< slash '/'
	Dash,			//< en dash (minus) '-'
	Exclam,			//< exclamation mark '!'
	Questm,			//< question mark '?'
	Sq,			//< single quote
	Dq,			//< double quote
	Osb,			//< open square bracket '['
	Csb,			//< close square bracket ']'
	Any			//< any ascii character with meaning
};
enum {NofControlCharacter=18};	//< total number of control characters

/// \class ControlCharacterM
/// \brief Map of the enumeration of control characters to their names for debug messages
struct ControlCharacterM
{
	/// \brief Get the name of a control character as string
	/// \param [in] c the control character to map
	static const char* name( ControlCharacter c)
	{
		static const char* name[ NofControlCharacter] = {"Undef", "EndOfText", "EndOfLine", "Cntrl", "Space", "Amp", "Lt", "Equal", "Gt", "Slash", "Dash", "Exclam", "Questm", "Sq", "Dq", "Osb", "Csb", "Any"};
		return name[ (unsigned int)(unsigned char)c];
	}
};

}//namespace
#endif


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
//
// base64.hpp - c++ wrapper for base64 functions
//

#ifndef _BASE64_HPP_INCLUDED
#define _BASE64_HPP_INCLUDED

#include <iostream>
#include "types/base64.h"

namespace _Wolframe	{
namespace base64	{

inline size_t encodedSize( size_t dataSize, unsigned short lineLength )
{
	return base64_encodedSize( dataSize, lineLength );
}

inline int encode( const void* data, size_t dataSize,
		   char* encoded, size_t encodedMaxSize, unsigned short lineLength = 72 )
{
	return base64_encode( data, dataSize, encoded, encodedMaxSize, lineLength );
}

std::string encode( const void* data, size_t dataSize, unsigned short lineLength );

/// \class Encoder
/// \brief Base64 encoder class for chunkwise encoding
class Encoder
{
	static const size_t BUFFERSIZE = 512;
public:
	Encoder( size_t bufferSize = BUFFERSIZE,
		 unsigned short lineLength = DEFAULT_BASE64_LINE_LENGTH )
		: m_bufferSize( bufferSize )
	{
		base64_initEncodeState( &m_state, lineLength );
	}

	size_t encodedSize( size_t dataSize )
	{
		return base64_encodedSize( dataSize, m_state.lineLength );
	}

	int encodeChunk( const void* data, size_t dataSize,
			 char* encoded, size_t encodedMaxSize )
	{
		return base64_encodeChunk( &m_state, data, dataSize, encoded, encodedMaxSize );
	}

	int encodeEndChunk( char* encoded, size_t encodedMaxSize )
	{
		return base64_encodeEndChunk( &m_state, encoded, encodedMaxSize );
	}

	void encode( std::istream& input, std::ostream& output );

private:
	base64_EncodeState	m_state;
	const size_t		m_bufferSize;

#ifdef _WIN32
// prevents C4512 on Windows (the m_bufferSize is const in the class)
private:
	Encoder& operator=( const Encoder &o );
#endif
};


inline int decode( const char* encoded, size_t encodedSize, void* data, size_t dataMaxSize )
{
	return base64_decode( encoded, encodedSize, data, dataMaxSize );
}

inline int decode( const std::string encoded, void* data, size_t dataMaxSize )
{
	return base64_decode( encoded.data(), encoded.length(), data, dataMaxSize );
}


/// \class Decoder
/// \brief Base64 decoder class for chunkwise decoding
class Decoder
{
	static const size_t BUFFERSIZE = 512;
public:
	Decoder() : m_bufferSize( BUFFERSIZE )
	{
		base64_initDecodeState( &m_state );
	}

	Decoder( size_t bufferSize ) : m_bufferSize( bufferSize )
	{
		base64_initDecodeState( &m_state );
	}

	int decode( const char* encoded, size_t encodedSize,
		    void* data, size_t dataMaxSize )
	{
		return base64_decodeChunk( &m_state, encoded, encodedSize, data, dataMaxSize );
	}

	void decode( std::istream& input, std::ostream& output );

private:
	base64_DecodeState	m_state;
	const size_t		m_bufferSize;

#ifdef _WIN32
// prevents C4512 on Windows (the m_bufferSize is const in the class)
private:
	Decoder& operator=( const Decoder &o );
#endif
};

}} // namespace _Wolframe::base64

#endif // _BASE64_HPP_INCLUDED


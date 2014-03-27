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

#include <string>
#include <sstream>
#include "types/base64.hpp"

namespace _Wolframe	{
namespace base64	{

static const size_t LOCAL_BUFFERSIZE = 512;

std::string encode( const void* data, size_t dataSize, unsigned short lineLength )
{
	base64_EncodeState	state;
	std::ostringstream	output;
	int			codedSize;
	char* encoded = new char[ 2 * LOCAL_BUFFERSIZE ];

	base64_initEncodeState( &state, lineLength );
	const unsigned char* plain = (const unsigned char*)data;
	while ( dataSize > LOCAL_BUFFERSIZE )	{
		codedSize = base64_encodeChunk( &state, plain, LOCAL_BUFFERSIZE,
						encoded, 2 * LOCAL_BUFFERSIZE );
		output.write( encoded, codedSize );
		plain += LOCAL_BUFFERSIZE;
		dataSize -= LOCAL_BUFFERSIZE;
	}
	codedSize = base64_encodeChunk( &state, plain, dataSize,
					encoded, 2 * LOCAL_BUFFERSIZE );
	output.write( encoded, codedSize );
	codedSize = base64_encodeEndChunk( &state, encoded, 2 * LOCAL_BUFFERSIZE );
	output.write( encoded, codedSize );

	return output.str();
}


void Encoder::encode( std::istream& input, std::ostream& output )
{
	unsigned char* plain = new unsigned char[ m_bufferSize ];
	char* encoded = new char[ 2 * m_bufferSize ];
	int dataSize;
	int codedSize;

	do	{
		input.read( (char *)plain, m_bufferSize );
		dataSize = (int)input.gcount();
		//
		codedSize = encodeChunk( plain, dataSize, encoded, 2 * m_bufferSize );
		output.write( encoded, codedSize );
	} while ( input.good() && dataSize > 0 );

	codedSize = encodeEndChunk( encoded, 2 * m_bufferSize );
	output.write( encoded, codedSize );
	//
	base64_resetEncodeState( &m_state );

	delete [] encoded;
	delete [] plain;
}


void Decoder::decode( std::istream& input, std::ostream& output )
{
	char* encoded = new char[ m_bufferSize ];
	unsigned char* data = new unsigned char[ m_bufferSize ];
	int encodedSize;
	int dataSize;

	do
	{
		input.read( encoded, m_bufferSize );
		encodedSize = (int)input.gcount();
		dataSize = decode( encoded, encodedSize, data, m_bufferSize );
		output.write( (const char*)data, dataSize );
	} while ( input.good() && encodedSize > 0 );

	base64_initDecodeState( &m_state );

	delete [] encoded;
	delete [] data;
}

}} // namespace _Wolframe::base64


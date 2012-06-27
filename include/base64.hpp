// base64.hpp - c++ wrapper for base64 functions
//
// Adapted from the libb64 project (http://sourceforge.net/projects/libb64)
// for the Wolframe project

#ifndef _BASE64_HPP_INCLUDED
#define _BASE64_HPP_INCLUDED

#include <iostream>
#include "base64.h"

namespace _Wolframe	{
namespace base64	{

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

	int encode( const unsigned char* plain, const int plainLength,
		    char* encoded, size_t encodedMaxSize )
	{
		return base64_encodeBlock( &m_state, plain, plainLength, encoded, encodedMaxSize );
	}

	int encodeEnd( char* encoded, size_t encodedMaxSize )
	{
		return base64_encodeEnd( &m_state, encoded, encodedMaxSize );
	}

	void encode( std::istream& input, std::ostream& output )
	{
		unsigned char* plain = new unsigned char[ m_bufferSize ];
		char* encoded = new char[ 2 * m_bufferSize ];
		int plainLength;
		int encodedLength;

		do	{
			input.read( (char *)plain, m_bufferSize );
			plainLength = input.gcount();
			//
			encodedLength = encode( plain, plainLength, encoded, 2 * m_bufferSize );
			output.write( encoded, encodedLength );
		} while ( input.good() && plainLength > 0 );

		encodedLength = encodeEnd( encoded, 2 * m_bufferSize );
		output.write( encoded, encodedLength );
		//
		base64_resetEncodeState( &m_state );

		delete [] encoded;
		delete [] plain;
	}

private:

	base64_EncodeState	m_state;
	const size_t		m_bufferSize;
};


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

	int decode( const char* encoded, size_t encodedLength,
		    unsigned char* plain, size_t plainMaxSize )
	{
		return base64_decodeBlock( &m_state, encoded, encodedLength, plain, plainMaxSize );
	}

	void decode( std::istream& input, std::ostream& output )
	{
		char* encoded = new char[ m_bufferSize ];
		unsigned char* plain = new unsigned char[ m_bufferSize ];
		int encodedLength;
		int plainLength;

		do
		{
			input.read( encoded, m_bufferSize );
			encodedLength = input.gcount();
			plainLength = decode( encoded, encodedLength, plain, m_bufferSize );
			output.write( (const char*)plain, plainLength );
		} while ( input.good() && encodedLength > 0 );

		base64_initDecodeState( &m_state );

		delete [] encoded;
		delete [] plain;
	}
private:
	base64_DecodeState	m_state;
	const size_t		m_bufferSize;
};

}} // namespace _Wolframe::base64

#endif // _BASE64_HPP_INCLUDED


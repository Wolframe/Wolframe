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

	int encode( const void* data, size_t dataSize,
		    char* encoded, size_t encodedMaxSize )
	{
		return base64_encodeBlock( &m_state, data, dataSize, encoded, encodedMaxSize );
	}

	int encodeEnd( char* encoded, size_t encodedMaxSize )
	{
		return base64_encodeEnd( &m_state, encoded, encodedMaxSize );
	}

	void encode( std::istream& input, std::ostream& output )
	{
		unsigned char* plain = new unsigned char[ m_bufferSize ];
		char* encoded = new char[ 2 * m_bufferSize ];
		int dataSize;
		int encodedSize;

		do	{
			input.read( (char *)plain, m_bufferSize );
			dataSize = input.gcount();
			//
			encodedSize = encode( plain, dataSize, encoded, 2 * m_bufferSize );
			output.write( encoded, encodedSize );
		} while ( input.good() && dataSize > 0 );

		encodedSize = encodeEnd( encoded, 2 * m_bufferSize );
		output.write( encoded, encodedSize );
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

	int decode( const char* encoded, size_t encodedSize,
		    void* data, size_t dataMaxSize )
	{
		return base64_decodeBlock( &m_state, encoded, encodedSize, data, dataMaxSize );
	}

	void decode( std::istream& input, std::ostream& output )
	{
		char* encoded = new char[ m_bufferSize ];
		unsigned char* data = new unsigned char[ m_bufferSize ];
		int encodedSize;
		int dataSize;

		do
		{
			input.read( encoded, m_bufferSize );
			encodedSize = input.gcount();
			dataSize = decode( encoded, encodedSize, data, m_bufferSize );
			output.write( (const char*)data, dataSize );
		} while ( input.good() && encodedSize > 0 );

		base64_initDecodeState( &m_state );

		delete [] encoded;
		delete [] data;
	}
private:
	base64_DecodeState	m_state;
	const size_t		m_bufferSize;
};

}} // namespace _Wolframe::base64

#endif // _BASE64_HPP_INCLUDED


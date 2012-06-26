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

struct decoder
{
	static const size_t BUFFERSIZE = 512;

	base64_DecodeState m_state;
	int _buffersize;

	decoder(int buffersize_in = BUFFERSIZE)
		: _buffersize(buffersize_in)
	{}

	int decode( const char* encoded, size_t encodedLength, unsigned char* plain )
	{
		return base64_decodeBlock( &m_state, encoded, encodedLength, plain );
	}

	void decode( std::istream& encoded, std::ostream& plain )
	{
		base64_initDecodeState( &m_state );
		//
		const int N = _buffersize;
		char* code = new char[N];
		unsigned char* plaintext = new unsigned char[N];
		int encodedLength;
		int plainLength;

		do
		{
			encoded.read((char*)code, N);
			encodedLength = encoded.gcount();
			plainLength = decode( code, encodedLength, plaintext );
			plain.write((const char*)plaintext, plainLength );
		}
		while (encoded.good() && encodedLength > 0);
		//
		base64_initDecodeState(&m_state);

		delete [] code;
		delete [] plaintext;
	}
};

struct encoder
{
	static const size_t BUFFERSIZE = 512;

	base64_EncodeState	m_state;
	int			m_buffersize;

	encoder( int buffersize_in = BUFFERSIZE, unsigned short lineLength = DEFAULT_BASE64_LINE_LENGTH )
		: m_buffersize( buffersize_in )
	{
		base64_initEncodeState( &m_state, lineLength );
	}

	int encode( const unsigned char* plain, const int plainLength, char* encoded )
	{
		return base64_encodeBlock( &m_state, plain, plainLength, encoded );
	}

	int encodeEnd( char* output )
	{
		return base64_encodeEnd( &m_state, output );
	}

	void encode( std::istream& plain, std::ostream& encoded )
	{
		const int N = m_buffersize;
		char* plaintext = new char[N];
		char* code = new char[2*N];
		int plainlength;
		int codelength;

		do	{
			plain.read( plaintext, N );
			plainlength = plain.gcount();
			//
			codelength = encode((unsigned char *)plaintext, plainlength, code );
			encoded.write( code, codelength );
		} while ( plain.good() && plainlength > 0 );

		codelength = encodeEnd( code );
		encoded.write( code, codelength );
		//
		base64_resetEncodeState( &m_state );

		delete [] code;
		delete [] plaintext;
	}
};

}} // namespace _Wolframe::base64

#endif // _BASE64_HPP_INCLUDED


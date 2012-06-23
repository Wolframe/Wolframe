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

	int decode(const char* code_in, const int length_in, unsigned char* plaintext_out)
	{
		return base64_decodeBlock( &m_state, code_in, length_in, plaintext_out );
	}

	void decode( std::istream& in, std::ostream& out )
	{
		base64_initDecodeState( &m_state );
		//
		const int N = _buffersize;
		char* code = new char[N];
		unsigned char* plaintext = new unsigned char[N];
		int codelength;
		int plainlength;

		do
		{
			in.read((char*)code, N);
			codelength = in.gcount();
			plainlength = decode( code, codelength, plaintext );
			out.write((const char*)plaintext, plainlength );
		}
		while (in.good() && codelength > 0);
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

	int encode(const unsigned char* code_in, const int length_in, char* plaintext_out)
	{
		return base64_encodeBlock( &m_state, code_in, length_in, plaintext_out );
	}

	int encodeEnd( char* output )
	{
		return base64_encodeEnd( &m_state, output );
	}

	void encode( std::istream& in, std::ostream& out )
	{
		const int N = m_buffersize;
		char* plaintext = new char[N];
		char* code = new char[2*N];
		int plainlength;
		int codelength;

		do	{
			in.read( plaintext, N );
			plainlength = in.gcount();
			//
			codelength = encode((unsigned char *)plaintext, plainlength, code );
			out.write( code, codelength );
		} while ( in.good() && plainlength > 0 );

		codelength = encodeEnd( code );
		out.write( code, codelength );
		//
		base64_resetEncodeState( &m_state );

		delete [] code;
		delete [] plaintext;
	}
};

}} // namespace _Wolframe::base64

#endif // _BASE64_HPP_INCLUDED


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
	base64_decodestate _state;
	int _buffersize;

	decoder(int buffersize_in = BUFFERSIZE)
		: _buffersize(buffersize_in)
	{}

	int decode(char value_in)
	{
		return base64_decode_value(value_in);
	}

	int decode(const char* code_in, const int length_in, char* plaintext_out)
	{
		return base64_decode_block(code_in, length_in, plaintext_out, &_state);
	}

	void decode(std::istream& istream_in, std::ostream& ostream_in)
	{
		base64_init_decodestate(&_state);
		//
		const int N = _buffersize;
		char* code = new char[N];
		char* plaintext = new char[N];
		int codelength;
		int plainlength;

		do
		{
			istream_in.read((char*)code, N);
			codelength = istream_in.gcount();
			plainlength = decode(code, codelength, plaintext);
			ostream_in.write((const char*)plaintext, plainlength);
		}
		while (istream_in.good() && codelength > 0);
		//
		base64_init_decodestate(&_state);

		delete [] code;
		delete [] plaintext;
	}
};

struct encoder
{
	base64_encodestate _state;
	int _buffersize;

	encoder(int buffersize_in = BUFFERSIZE)
		: _buffersize(buffersize_in)
	{}

	int encode(char value_in)
	{
		return base64_encode_value(value_in);
	}

	int encode(const char* code_in, const int length_in, char* plaintext_out)
	{
		return base64_encode_block(code_in, length_in, plaintext_out, &_state);
	}

	int encode_end(char* plaintext_out)
	{
		return base64_encode_blockend(plaintext_out, &_state);
	}

	void encode(std::istream& istream_in, std::ostream& ostream_in)
	{
		base64_init_encodestate(&_state);
		//
		const int N = _buffersize;
		char* plaintext = new char[N];
		char* code = new char[2*N];
		int plainlength;
		int codelength;

		do
		{
			istream_in.read(plaintext, N);
			plainlength = istream_in.gcount();
			//
			codelength = encode(plaintext, plainlength, code);
			ostream_in.write(code, codelength);
		}
		while (istream_in.good() && plainlength > 0);

		codelength = encode_end(code);
		ostream_in.write(code, codelength);
		//
		base64_init_encodestate(&_state);

		delete [] code;
		delete [] plaintext;
	}
};

}} // namespace _Wolframe::base64

#endif // _BASE64_HPP_INCLUDED


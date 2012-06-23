/*
base64.cc - c++ source to a base64 reference encoder and decoder

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/

#include <base64.hpp>

#include <iostream>
#include <fstream>
#include <string>

using namespace _Wolframe;

// Function which prints the usage of this executable
void usage()
{
	std::cerr << "base64Utility: encodes and decodes files using base64\n"
		     "Usage: base64Utility [-e|-d] [input] [output]\n"
		     "         [-e] encode the input file into the output file,\n"
		     "         [-d] decode the input file into the output file.\n";
}

// Function which prints the usage of this executable, plus a short message
void usage( const std::string& message )
{
	usage();
	std::cerr << "Incorrect invocation of base64Utility:\n";
	std::cerr << message << std::endl;
}

int main(int argc, char** argv)
{
	// Quick check for valid arguments
	if ( argc == 1 )	{
		usage();
		return -1;
	}
	if ( argc != 4 )	{
		usage( "Wrong number of arguments!" );
		return -1;
	}

	// open the input file in binary mode.
	std::string input = argv[2];
	std::ifstream instream( input.c_str(), std::ios_base::in | std::ios_base::binary );
	if ( !instream.is_open() )	{
		usage( "Could not open input file!" );
		return -1;
	}

	// open the output file in binary mode.
	std::string output = argv[3];
	std::ofstream outstream( output.c_str(), std::ios_base::out | std::ios_base::binary );
	if ( !outstream.is_open() )	{
		usage( "Could not open output file!" );
		return -1;
	}

	// encode or decode
	std::string choice = argv[1];
	if ( choice == "-d" )	{
		base64::decoder D;
		D.decode( instream, outstream );
	}
	else if ( choice == "-e" )	{
		base64::encoder E;
		E.encode( instream, outstream );
	}
	else	{
		std::cout << "[" << choice << "]" << std::endl;
		usage( "Please specify -d or -e as first argument!" );
	}

	return 0;
}

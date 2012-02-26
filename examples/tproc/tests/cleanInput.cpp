#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

/*
* This file is not part of Wolframe. It is used by the Wolframe Team to generate test documents out from a description.
*/

static void removeBOM( std::string& input)
{
	std::string rt;
	enum {FE,FF,SRC} state = SRC;
	std::string::const_iterator itr = input.begin(),end = input.end();
	for (;itr != end; ++itr)
	{
		if (*itr == (char)0xFF)
		{
			if (state == FE)
			{
				state = SRC;
			}
			else if (state == FF)
			{
				rt.push_back( *itr);
				state = FF;
			}
			else
			{
				state = FF;
			}
		}
		else if (*itr == (char)0xFE)
		{
			if (state == FF)
			{
				state = SRC;
			}
			else if (state == FE)
			{
				rt.push_back( *itr);
				state = FE;
			}
			else
			{
				state = FE;
			}
		}
		else
		{
			if (state == FF)
			{
				rt.push_back( (char)0xFF);
			}
			else if (state == FE)
			{
				rt.push_back( (char)0xFE);
			}
			rt.push_back( *itr);
			state = SRC;
		}
	}
	if (state == FF)
	{
		rt.push_back( (char)0xFF);
	}
	else if (state == FE)
	{
		rt.push_back( (char)0xFE);
	}
	input = rt;
}

static void removeTrailingEOLN( std::string& input)
{
	int nn = input.size()-1;
	if (nn >= 0 && input[ nn] == '\n')
	{
		--nn;
		while (nn >= 0 && input[ nn] == '\0')
		{
			--nn;
		}
		input.resize( nn+1);
	}
	else
	{
		nn = input.size()-1;
		while (nn > 0 && input[ nn] == '\0')
		{
			--nn;
		}
		if (nn >= 0 && input[ nn] == '\n')
		{
			input.resize( nn);
		}
	}
}

int main( int argc, char **argv )
{
	std::string tt = "BLA\n\n";
	removeTrailingEOLN(tt);

	char ch;
	std::string input;

	while (1 == fread( &ch, sizeof(char), 1, stdin))
	{
		input.push_back( ch);
	}

	int ii=1;
	for (; ii<argc; ++ii)
	{
		if (std::strcmp( argv[ii], "BOM") == 0)
		{
			removeBOM( input);
		}
		if (std::strcmp( argv[ii], "EOLN") == 0)
		{
			removeTrailingEOLN( input);
		}
	}
	std::string::const_iterator itr=input.begin(),end=input.end();
	for (; itr != end; ++itr)
	{
		ch = *itr;
		if (1 > fwrite( &ch, sizeof(char), 1, stdout)) return 1;
	}
	return 0;
}


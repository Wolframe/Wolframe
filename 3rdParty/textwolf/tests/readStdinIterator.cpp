#include "textwolf.hpp"
#include <iostream>
#include <map>
#include <stdio.h>
#include <errno.h>

//build gcc
//compile: g++ -c -o readStdinIterator.o -g -I../include/ -pedantic -Wall -O4 readStdinIterator.cpp
//link: g++ -lc -o readStdinIterator readStdinIterator.o
//build windows
//compile: cl.exe /wd4996 /Ob2 /O2 /EHsc /MT /W4 /nologo /I..\include /D "WIN32" /D "_WINDOWS" /Fo"readStdinIterator.obj" readStdinIterator.cpp 
//link: link.exe /out:.\readStdinIterator  readStdinIterator.obj

using namespace textwolf;

namespace
{
struct Input
{
	enum {maxbufsize=8192};
	char buf[ maxbufsize];
	unsigned int bufsize;
	unsigned int bufpos;
	FILE* file;

	Input()				:bufsize(0),bufpos(0),file(stdin) {};
	Input( const char* fname)	:bufsize(0),bufpos(0),file(fopen(fname,"rb")) {};
	~Input()			{if (file != stdin) fclose(file);};

	char getchar()
	{
		if (bufpos == bufsize)
		{
			bufsize = fread( buf, 1, maxbufsize, file);
			if (bufsize == 0)
			{
				int ee = errno;
				if (ee != 0) fprintf( stderr, "error %d reading input\n", ee);
				return 0;
			}
			bufpos = 0;
		}
		return buf[ bufpos++];
	};

	struct iterator
	{
		Input* input;
		int ch;

		iterator( Input* p_input)	:input(p_input),ch(0) {};
		iterator()			:input(0),ch(0) {};
		iterator( const iterator& o)	:input(o.input),ch(o.ch) {};

		void skip()			{ch=input->getchar();};
		iterator& operator++()		{skip(); return *this;};
		iterator operator++(int)	{iterator tmp(*this); skip(); return tmp;};
		char operator*()		{return ch;};
	};
	iterator begin()			{iterator rt(this); rt.skip(); return rt;};
	iterator end()				{return iterator();};
};
}//anynomous namespace



int main( int argc, const char** argv)
{ 
	Input* input = 0;
	try
	{
		//[1] define the source iterator
		if (argc == 2)
		{
			input = new Input( argv[1]);
		}
		else if (argc > 2)
		{
			fprintf( stderr, "too many arguments");
			return 1;
		}
		else
		{
			input = new Input();
		}

		//[4] iterating through the produced elements and printing them
		Input::iterator itr=input->begin();
		for (; *itr!=0; itr++);
	}
	catch (exception ee)
	{
		std::cerr << "ERROR " << ee.what() << std::endl;
		return 1;
	}
	if (input) delete input;
	return 0;
}


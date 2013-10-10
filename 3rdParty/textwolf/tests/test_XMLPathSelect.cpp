#include "textwolf.hpp"
#include <iostream>
#include <map>
#include <string>

//build gcc
//compile: g++ -c -o test_XMLPathSelect.o -g -I../include/ -pedantic -Wall -O4 test_XMLPathSelect.cpp
//link: g++ -lc -o test_XMLPathSelect test_XMLPathSelect.o
//build windows
//compile: cl.exe /wd4996 /Ob2 /O2 /EHsc /MT /W4 /nologo /I..\include /D "WIN32" /D "_WINDOWS" /Fo"test_XMLPathSelect.obj" test_XMLPathSelect.cpp 
//link: link.exe /out:.\test_XMLPathSelect test_XMLPathSelect.obj

using namespace textwolf;

class ProtocolCharMap
{
	char state;
	ProtocolCharMap() :state('\n'){};

	char operator[]( char ch)
	{
		if (ch == state)
		{
			if (state == '\n') state='.';
			else if (state == '.') state='\r';
			else if (state == '\r') {state='\n'; return 0;}
		}
		state = '\n';
		return ch;
	}
};

int main( int, const char**)
{
	try
	{
		//[1] define the source iterator
		char* src = const_cast<char*>
		(
			"<?xml charset=utf-8?>\r\n"
			"<TT c='6'>7</TT>"
			"<TT i='56'>8</TT>"
			"<TT i='9'><v>9</v></TT>"
			"<TT><AA><BB>10</BB></AA></TT>"
			"<TT><AA>11</AA></TT>"
			"<TT><AA>&#65;Z&amp;&lt;&gt;&apos;&nbsp;&quot;Z</AA></TT>"
			"<AA z='4' t='4'>12 12 12</AA>"
			"<BB>13 13</BB>"
			"<CC z='4'>14</CC>"
			"<X><CC>15</CC></X><X><z><CC>15</CC></z></X>"
			"<Y><mm u='8'>16</mm></Y><Y><z><zz e='6' u='8' z='4'>16</zz></z></Y>"
			"<Y><mm q='1'>17</mm></Y><Y><z><zz q='1'>17</zz></z></Y>"
			"<Y><mm q='2'>18</mm></Y><Y><z><zz e='2'>18</zz></z></Y>"
		);
		//[2] creating the automaton
		typedef XMLPathSelectAutomaton<charset::UTF8> Automaton;
		Automaton atm;
		(*atm)["TT"]("c") = 6;
		(*atm)["TT"]("c")() = 7;
		(*atm)["TT"]("i","56")() = 8;
		(*atm)["TT"]("i","9")--() = 9;
		(*atm)["TT"]["AA"]["BB"] = 10;
		(*atm)["TT"]["AA"] = 11;
		(*atm)["AA"]() = 12;
		(*atm)["BB"] = 13;
		(*atm)--["CC"]() = 14;
		(*atm)["X"]--["CC"] = 15;
		(*atm)["Y"]--("u") = 16;
		(*atm)["Y"]--("q","1")() = 17;
		(*atm)["Y"]--(0,"2")() = 18;

		//[3] define the XML Path selection by the automaton over the source iterator
		typedef XMLPathSelect<charset::UTF8> MyXMLPathSelect;
		typedef XMLScanner<char*,charset::IsoLatin1,charset::IsoLatin1,std::string> MyXMLScanner;

		MyXMLScanner xc( src);
		MyXMLPathSelect xs( &atm);
		xc.doTokenize(true);

		//[4] iterating through the produced elements and printing them
		MyXMLScanner::iterator ci,ce;
		for (ci=xc.begin(),ce=xc.end(); ci!=ce; ci++)
		{
			MyXMLPathSelect::iterator itr=xs.find( ci->type(), ci->content(), ci->size()),end=xs.end();
			for (; itr!=end; itr++)
			{
				std::cout << "Element " << *itr << ": " << ci->content() << std::endl;
			}
		}
		//[5] handle a possible error
		if ((int)ci->type() == MyXMLScanner::ErrorOccurred)
		{
			std::cerr << "FAILED " << ci->content() << std::endl;
			return 1;
		}
		else
		{
			std::cerr << "OK" << std::endl;
			return 0;
		}
	}
	catch (exception ee)
	{
		std::cerr << "ERROR " << ee.what() << std::endl;
		return 1;
	}
}

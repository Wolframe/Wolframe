// WolfCLR.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "comauto_utils.hpp"
#include "comauto_record.hpp"
#include "comauto_typelib.hpp"
#include "comauto_clr.hpp"
#include <iostream>

int _tmain( int argc, _TCHAR* argv[])
{
	try
	{
		//comauto::TypeLib typelib( "C:\\Users\\patrick\\Projects\\Wolframe\\Functions.tlb");
		//typelib.print( std::cout);
		comauto::CommonLanguageRuntime clr;
		std::wstring asmname( L"C:\\Users\\patrick\\Documents\\Visual Studio 2010\\Projects\\Functions\\Functions\\bin\\Release\\Functions.dll");
		std::wstring classname( L"Functions");
		std::wstring methodname( L"Add");
		std::vector<VARIANT> param;
		param.push_back( comauto::createVariantType( (int)13));
		param.push_back( comauto::createVariantType( (int)2));
		VARIANT RESULT = clr.call( asmname, classname, methodname, param);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception " << e.what() << std::endl;
		return -1;
	}
	catch (const _com_error& e)
	{
		std::cerr << "Uncaught system exception: " << comauto::utf8string( comauto::tostring(e)) << std::endl;
		return -2;
	}
    return 0;
}


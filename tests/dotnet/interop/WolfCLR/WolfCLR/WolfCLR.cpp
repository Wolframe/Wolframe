// WolfCLR.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "comauto_utils.hpp"
#include "comauto_record.hpp"
#include "comauto_typelib.hpp"
#include "comauto_clr.hpp"
#include <iostream>

int main( int argc, const char* argv[])
{
	try
	{
		WRAP( ::CoInitializeEx( NULL, COINIT_MULTITHREADED))
		std::string path( "C:\\Users\\patrick\\Projects\\Wolframe\\tests\\dotnet\\csharp\\Functions\\bin\\Release\\");
		comauto::TypeLib typelib( path + "Functions.tlb");
		typelib.print( std::cout);
		comauto::CommonLanguageRuntime clr;

		std::vector<VARIANT> param;
		param.push_back( comauto::createVariantType( (int)13));
		param.push_back( comauto::createVariantType( (int)2));
		VARIANT RESULT = clr.call( path + "Functions.dll", "Functions", "Add", param);
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


// WolfCLR.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "comauto_utils.hpp"
#include "comauto_record.hpp"
#include "comauto_typelib.hpp"
#include "comauto_clr.hpp"
#include "ddl_form.hpp"
#include <iostream>

using namespace _Wolframe;

int main( int argc, const char* argv[])
{
	try
	{
		WRAP( ::CoInitializeEx( NULL, COINIT_MULTITHREADED))
		std::string path( "C:\\Users\\patrick\\Projects\\Wolframe\\tests\\dotnet\\csharp\\Functions\\bin\\Release\\");
		std::string assembly( "Functions, Version=1.0.0.0, Culture=neutral, PublicKeyToken=1c1d731dc6e1cbe1, processorArchitecture=MSIL");
		comauto::TypeLib typelib( path + "Functions.tlb");
		typelib.print( std::cout);
		comauto::CommonLanguageRuntime clr( "v4.0.30319");

		std::vector<comauto::FunctionR> funcs = typelib.loadFunctions( &clr, assembly);
		std::map<std::string,comauto::FunctionR> funcmap;
		std::vector<comauto::FunctionR>::const_iterator fi = funcs.begin(), fe = funcs.end();
		for (; fi != fe; ++fi)
		{
			std::cout << "FUNCTION " << (*fi)->assemblyname() << " " << (*fi)->classname() << "::" << (*fi)->methodname() << "[" << (*fi)->nofParameter() << "]" << std::endl;
			funcmap[ (*fi)->classname() + "." + (*fi)->methodname()] = *fi;
		}
		VARIANT param[ 2] = {comauto::createVariantType( (int)13), comauto::createVariantType( (int)2)};
		VARIANT result = clr.call( assembly, "Functions", "Add", 2, param);

		langbind::Form formparam;
		formparam( "i", langbind::Form("17"))( "j", langbind::Form("14"));

		std::map<std::string,comauto::FunctionR>::const_iterator xi = funcmap.find( "Functions.Add");
		if (xi == funcmap.end()) throw std::runtime_error( "function not defined");

		langbind::FormFunctionClosure* closure = xi->second ->createClosure();
		closure->init( 0, formparam.get());
		if (!closure->call()) throw std::runtime_error( "function call failed");

		langbind::TypedInputFilterR funcres = closure->result();
		comauto::FunctionResult::ElementType elemtype;
		comauto::FunctionResult::Element elem;

		std::cout << "RESULT:" << std::endl;
		while (funcres->getNext( elemtype, elem))
		{
			std::cout << comauto::FunctionResult::elementTypeName( elemtype) << " '" << elem.tostring() << "'" << std::endl;
		}
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


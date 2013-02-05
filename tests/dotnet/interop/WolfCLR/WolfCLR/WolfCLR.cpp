// WolfCLR.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "comauto_utils.hpp"
#include "comauto_typelib.hpp"
#include "comauto_clr.hpp"
#include "comauto_function.hpp"
#include "ddl_form.hpp"
#include <iostream>

using namespace _Wolframe;

static void test_atomic_param_clr_call( const comauto::CommonLanguageRuntime& clr, const std::string& assembly)
{
	VARIANT param[ 2] = {comauto::createVariantType( (int)13), comauto::createVariantType( (int)2)};
	VARIANT result = clr.call( assembly, "Functions", "Add", 2, param);

	std::cout << std::endl << "RESULT call CLR: 13 + 2 = " << std::endl;
	std::string buf;
	langbind::TypedFilterBase::Element elem = comauto::getAtomicElement( result, buf);
	std::cout << elem.tostring() << std::endl;
}

static void test_function_call( const std::map<std::string,comauto::FunctionR>& funcmap, const char* name, const langbind::Form& param, const char* title)
{
	std::map<std::string,comauto::FunctionR>::const_iterator xi = funcmap.find( name);
	if (xi == funcmap.end()) throw std::runtime_error( std::string("function not defined: '") + name + "'");

	langbind::FormFunctionClosure* closure = xi->second ->createClosure();
	closure->init( 0, param.get());
	if (!closure->call()) throw std::runtime_error( std::string("function call failed: '") + name + "'");

	langbind::TypedInputFilterR funcres = closure->result();
	langbind::TypedFilterBase::ElementType elemtype;
	langbind::TypedFilterBase::Element elem;

	std::cout << std::endl << title << std::endl;
	while (funcres->getNext( elemtype, elem))
	{
		std::cout << langbind::TypedFilterBase::elementTypeName( elemtype) << " '" << elem.tostring() << "'" << std::endl;
	}
}

static void test_atomic_param_fun_call( const std::map<std::string,comauto::FunctionR>& funcmap)
{
	langbind::Form param;
	param( "i", langbind::Form("17"))( "j", langbind::Form("14"));
	test_function_call( funcmap, "Functions.Add", param, "RESULT Add (17 + 14):");
}

static void test_struct_param_fun_call( const std::map<std::string,comauto::FunctionR>& funcmap)
{
	langbind::Form user, place, pair;
	langbind::Form param_AddIdPair, param_GetUser_p, param_GetAddress_p, param_StoreUser, param_StoreUsers;
	pair
		( "a", langbind::Form( "51"))
		( "b", langbind::Form( "3"))
		;
	place
		( "street", langbind::Form( "Vogelsangstrasse 5 8006 Zurich"))
		( "country", langbind::Form( "Switzerland"))
		;
	user
		( "id", langbind::Form( "123"))
		( "name", langbind::Form( "Hans Muster"))
		( "place", place)
		;
	param_AddIdPair
		( "p", pair)
		;
	param_GetUser_p
		( "id", langbind::Form( "123"))
		( "name", langbind::Form( "Hans Muster"))
		( "street", langbind::Form( "Vogelsangstrasse 5 8006 Zurich"))
		( "country", langbind::Form( "Switzerland"))
		;
	param_GetAddress_p
		( "street", langbind::Form( "Vogelsangstrasse 5 8006 Zurich"))
		( "country", langbind::Form( "Switzerland"))
		;
	param_StoreUser
		( "usr", user)
		;
	test_function_call( funcmap, "Functions.AddIdPair", param_AddIdPair, "RESULT AddIdPair:");
	test_function_call( funcmap, "Functions.GetAddress_p", param_GetAddress_p, "RESULT GetAddress:");
	test_function_call( funcmap, "Functions.StoreUser", param_StoreUser, "RESULT StoreUser:");
};


int main( int , const char**)
{
	try
	{
		WRAP( ::CoInitializeEx( NULL, COINIT_MULTITHREADED))
		std::string path( "C:\\Users\\patrick\\Projects\\Wolframe\\tests\\dotnet\\csharp\\Functions\\bin\\Release\\");
		std::string assembly( "Functions, Version=1.0.0.13, Culture=neutral, PublicKeyToken=1c1d731dc6e1cbe1, processorArchitecture=MSIL");
		comauto::TypeLib typelib( path + "Functions.tlb");
		typelib.print( std::cout);
		comauto::CommonLanguageRuntime clr( "v4.0.30319");

		std::vector<comauto::FunctionR> funcs = comauto::loadFunctions( &typelib, &clr, assembly);
		std::vector<comauto::FunctionR>::const_iterator fi = funcs.begin(), fe = funcs.end();
		std::map<std::string,comauto::FunctionR> funcmap;

		std::string asmname;
		for (; fi != fe; ++fi)
		{
			if (asmname != (*fi)->assemblyname())
			{
				asmname = (*fi)->assemblyname();
				std::cout << "ASSEMBLY " << asmname << std::endl;
			}
			std::cout << "FUNCTION " << (*fi)->classname() << "." << (*fi)->methodname() << "[" << (*fi)->nofParameter() << "]" << std::endl;
			funcmap[ (*fi)->classname() + "." + (*fi)->methodname()] = *fi;
		}
		test_atomic_param_clr_call( clr, assembly);
		test_atomic_param_fun_call( funcmap);
		test_struct_param_fun_call( funcmap);
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


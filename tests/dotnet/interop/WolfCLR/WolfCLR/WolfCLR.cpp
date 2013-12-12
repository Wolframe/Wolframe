// WolfCLR.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "comauto/utils.hpp"
#include "comauto/typelib.hpp"
#include "comauto/clr.hpp"
#include "comauto/function.hpp"
#include "processor/procProvider.hpp"
#include "testFormFunction.hpp"
#include "ddl_form.hpp"
#include <iostream>

using namespace _Wolframe;

static proc::ProcessorProvider g_provider;

static void initProcessorProvider()
{
	test::Form result_GetAddress;
	result_GetAddress
		( "street", test::Form( "Gukenweg 5 8006 Zurich"))
		( "country", test::Form( "Schweiz"))
		;
	langbind::FormFunctionR function_GetAddress( new test::FormFunction( "GetAddress", result_GetAddress));
	g_provider.defineFormFunction( "GetAddress", function_GetAddress);
}

static void test_atomic_param_clr_call( const comauto::CommonLanguageRuntime& clr, const std::string& assembly)
{
	VARIANT param[ 2] = {comauto::createVariantType( (int)13), comauto::createVariantType( (int)2)};
	VARIANT result;
	clr.call( &result, assembly, "Functions", "Add", 2, param);

	std::cout << std::endl << "RESULT call CLR: 13 + 2 = " << std::endl;
	std::string buf;
	types::VariantConst elem = comauto::getAtomicElement( result, buf);
	std::cout << elem.tostring() << std::endl;
}

static void test_function_call( const std::map<std::string,comauto::DotnetFunctionR>& funcmap, const char* name, const test::Form& param, const char* title)
{
	std::map<std::string,comauto::DotnetFunctionR>::const_iterator xi = funcmap.find( name);
	if (xi == funcmap.end()) throw std::runtime_error( std::string("function not defined: '") + name + "'");

	langbind::FormFunctionClosureR closure( xi->second->createClosure());
	closure->init( &g_provider, param.get());
	if (!closure->call()) throw std::runtime_error( std::string("function call failed: '") + name + "'");

	langbind::TypedInputFilterR funcres = closure->result();
	langbind::FilterBase::ElementType elemtype;
	types::VariantConst elem;

	std::cout << std::endl << title << std::endl;
	while (funcres->getNext( elemtype, elem))
	{
		std::cout << langbind::FilterBase::elementTypeName( elemtype) << " '" << elem.tostring() << "'" << std::endl;
	}
}

static void test_atomic_param_fun_call( const std::map<std::string,comauto::DotnetFunctionR>& funcmap)
{
	test::Form param;
	param( "i", test::Form("17"))( "j", test::Form("14"));
	test_function_call( funcmap, "Functions.Add", param, "RESULT Add (17 + 14):");
}

static void test_struct_param_fun_call( const std::map<std::string,comauto::DotnetFunctionR>& funcmap)
{
	test::Form pair;
	pair
		( "a", test::Form( "1"))
		( "b", test::Form( "1"))
		;
	test::Form pair2;
	pair2
		( "a", test::Form( "2"))
		( "b", test::Form( "2"))
		;
	test::Form pair3;
	pair3
		( "a", test::Form( "3"))
		( "b", test::Form( "3"))
		;
	test::Form pair4;
	pair4
		( "a", test::Form( "4"))
		( "b", test::Form( "4"))
		;
	test::Form pairs(test::Form::Array);
	pairs
		(pair)
		(pair2)
		(pair3)
		(pair4)
		;
	test::Form place;
	place
		( "street", test::Form( "Vogelsangstrasse 5 8006 Zurich"))
		( "country", test::Form( "Switzerland"))
		;
	test::Form place2;
	place2
		( "street", test::Form( "Feldweg 3 8136 Gattikon"))
		( "country", test::Form( "Switzerland"))
		;
	test::Form user;
	user
		( "id", test::Form( "123"))
		( "name", test::Form( "Hans Muster"))
		( "place", place)
		;
	test::Form user2;
	user2
		( "id", test::Form( "234"))
		( "name", test::Form( "Peter Kernel"))
		( "place", place)
		;
	test::Form users(test::Form::Array);
	users
		( user)
		( user2)
		;
	test::Form usergroup;
	usergroup
		("name", test::Form( "Spielgruppe"))
		("usr", users)
		;
	test::Form intarr(test::Form::Array);
	intarr
		( test::Form( "7"))
		( test::Form( "13"))
		( test::Form( "11"))
		( test::Form( "1000"))
		;
	test::Form param_AddIdPair;
	param_AddIdPair
		( "p", pair)
		;
	test::Form param_AddIdPairs;
	param_AddIdPairs
		( "p", pairs)
		;
	test::Form param_GetUser_p;
	param_GetUser_p
		( "id", test::Form( "123"))
		( "name", test::Form( "Hans Muster"))
		( "street", test::Form( "Vogelsangstrasse 5 8006 Zurich"))
		( "country", test::Form( "Switzerland"))
		;
	test::Form param_GetAddress_p;
	param_GetAddress_p
		( "street", test::Form( "Vogelsangstrasse 5 8006 Zurich"))
		( "country", test::Form( "Switzerland"))
		;
	test::Form param_StoreUser;
	param_StoreUser
		( "usr", user)
		;
	test::Form param_StoreUsers;
	param_StoreUsers
		( "usr", users)
		;
	test::Form param_Sum;
	param_Sum
		( "aa", intarr)
		;
	test::Form sar(test::Form::Array);
	sar
		( test::Form( "guru "))
		( test::Form( "kaeng "))
		( test::Form( "paeng "))
		( test::Form( "gulu "))
		;
	test::Form param_ConcatStrings;
	param_ConcatStrings
		("sar" , sar)
		;
	test::Form param_GetIdPairs;
	param_GetIdPairs
		( "len", test::Form( "4"))
		;
	test::Form param_StoreUserGroup;
	param_StoreUserGroup
		( "usr", usergroup)
		;
	test::Form param_Sub;
	param_Sub
		( "i", test::Form( "41"))
		( "j", test::Form( "12"))
		;
	test::Form param_GetUserAddress;
	param_GetUserAddress
		( "usr", user)
		;
	test::Form param_GetUserXYZ;

	test_function_call( funcmap, "Functions.Sub", param_Sub, "RESULT Sub:");
	test_function_call( funcmap, "Functions.AddIdPair", param_AddIdPair, "RESULT AddIdPair:");
	test_function_call( funcmap, "Functions.GetIdPairs", param_GetIdPairs, "RESULT GetIdPairs:");
	test_function_call( funcmap, "Functions.ConcatStrings", param_ConcatStrings, "RESULT ConcatStrings:");
	test_function_call( funcmap, "Functions.AddIdPairs", param_AddIdPairs, "RESULT AddIdPairs:");
	test_function_call( funcmap, "Functions.GetAddress_p", param_GetAddress_p, "RESULT GetAddress_p:");
	test_function_call( funcmap, "Functions.StoreUser", param_StoreUser, "RESULT StoreUser:");
	test_function_call( funcmap, "Functions.Sum", param_Sum, "RESULT Sum:");
	test_function_call( funcmap, "Functions.StoreUsers", param_StoreUsers, "RESULT StoreUsers:");
	test_function_call( funcmap, "Functions.StoreUserGroup", param_StoreUserGroup, "RESULT StoreUserGroup:");
	test_function_call( funcmap, "Functions.GetUserXYZ", param_GetUserXYZ, "RESULT GetUserXYZ:");
	test_function_call( funcmap, "Functions.GetUserAddress", param_GetUserAddress, "RESULT GetUserAddress:");
};


int main( int , const char**)
{
	try
	{
		initProcessorProvider();

		WRAP( ::CoInitializeEx( NULL, COINIT_MULTITHREADED));
		std::string path( "C:\\Github\\Wolframe\\tests\\dotnet\\csharp\\Functions\\bin\\Release\\");
		std::string assembly( "Functions, Version=1.0.0.47, Culture=neutral, PublicKeyToken=1c1d731dc6e1cbe1, processorArchitecture=MSIL");
		comauto::TypeLib typelib( path + "Functions.tlb");
		typelib.print( std::cout);
		comauto::CommonLanguageRuntime clr( "v4.0.30319");

		std::vector<comauto::DotnetFunctionR> funcs = comauto::loadFunctions( &typelib, &clr, assembly);
		std::vector<comauto::DotnetFunctionR>::const_iterator fi = funcs.begin(), fe = funcs.end();
		std::map<std::string,comauto::DotnetFunctionR> funcmap;

		std::string asmname;
		for (; fi != fe; ++fi)
		{
			if (asmname != (*fi)->assemblyname())
			{
				asmname = (*fi)->assemblyname();
				std::cout << "ASSEMBLY " << asmname << std::endl;
			}
			std::cout << "FUNCTION " << (*fi)->classname() << "." << (*fi)->methodname() << std::endl;
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


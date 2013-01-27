#include "comauto_clr.hpp"
#include "comauto_utils.hpp"
#include <mscoree.h>
#include <metahost.h>
#pragma comment(lib,"MSCorEE.lib") 
#import "mscorlib.tlb" raw_interfaces_only \
high_property_prefixes("_get","_put","_putref") \
rename("ReportEvent",  "InteropServices_ReportEvent") \
rename("value",  "InteropServices_value")

using namespace _Wolframe;
using namespace mscorlib;

comauto::CommonLanguageRuntime::~CommonLanguageRuntime()
{
	m_runtimehost->Stop();	///NOTE: CLR Runtime host cannot be created again in the same process after a Stop
    if (m_metahost) m_metahost->Release();
    if (m_runtimeinfo) m_runtimeinfo->Release();
    if (m_runtimehost) m_runtimehost->Release();
}

comauto::CommonLanguageRuntime::CommonLanguageRuntime( const std::string& version)
	:m_metahost(0),m_runtimeinfo(0),m_runtimehost(0)
{
	std::wstring version_w( version.begin(), version.end());
	PCWSTR clrversion = version_w.c_str();
	try
	{
		WRAP( ::CLRCreateInstance( CLSID_CLRMetaHost, IID_PPV_ARGS( &m_metahost)))
		WRAP( m_metahost->GetRuntime( clrversion, IID_PPV_ARGS(&m_runtimeinfo)))

		BOOL fLoadable;
		WRAP( m_runtimeinfo->IsLoadable( &fLoadable))
		if (!fLoadable)
		{
			throw std::runtime_error( ".NET runtime cannot be loaded");
		}
		WRAP( m_runtimeinfo->GetInterface( CLSID_CorRuntimeHost,  IID_PPV_ARGS( &m_runtimehost)))
		WRAP( m_runtimehost->Start())
	}
	catch (const std::runtime_error& e)
	{
		if (m_metahost) m_metahost->Release();
		if (m_runtimeinfo) m_runtimeinfo->Release();
	    if (m_runtimehost) m_runtimehost->Release();
		throw e;
	}
}

VARIANT comauto::CommonLanguageRuntime::call( const std::wstring& assembly_, const std::wstring& class_, const std::wstring& method_, LONG argc, const VARIANT* argv)
{
    SAFEARRAY* methodArgs = NULL;
	 IUnknownPtr spAppDomainThunk = NULL; 
	_AppDomainPtr spDefaultAppDomain = NULL; 
	_AssemblyPtr spAssembly = NULL; 
	_TypePtr spType = NULL; 
	try
	{
		// The static method in the .NET class to invoke.
		bstr_t bstrAssemblyName( assembly_.c_str()); 
		bstr_t bstrClassName( class_.c_str()); 
		bstr_t bstrMethodName( method_.c_str()); 
 
		WRAP( m_runtimehost->GetDefaultDomain( &spAppDomainThunk))
		WRAP( spAppDomainThunk->QueryInterface( IID_PPV_ARGS( &spDefaultAppDomain)))
		WRAP( spDefaultAppDomain->Load_2( bstrAssemblyName, &spAssembly))
		WRAP( spAssembly->GetType_2( bstrClassName, &spType))

		variant_t vtObject;
		variant_t vtReturnVal; 
		WRAP( spAssembly->CreateInstance(bstrClassName, &vtObject))

		methodArgs = ::SafeArrayCreateVector( VT_VARIANT, 0, argc);
		for (LONG aidx=0; aidx != argc; ++aidx)
		{
			VARIANT vt = argv[aidx];
			WRAP( ::SafeArrayPutElement( methodArgs, &aidx, &vt))
		}

		BindingFlags bindingFlags = static_cast<BindingFlags>( BindingFlags_InvokeMethod | BindingFlags_Instance | BindingFlags_Public);
		WRAP( spType->InvokeMember_3( bstrMethodName, bindingFlags, NULL, vtObject, methodArgs, &vtReturnVal))

		return vtReturnVal;
	}
	catch (const std::runtime_error& e)
	{
		if (methodArgs) SafeArrayDestroy(methodArgs);
		if (spType) spType->Release();
		if (spAssembly) spAssembly->Release();
		if (spDefaultAppDomain) spDefaultAppDomain->Release();
		throw e;
	}
}

VARIANT comauto::CommonLanguageRuntime::call( const std::string& assembly_utf8_, const std::string& class_utf8_, const std::string& method_utf8_, LONG argc, const VARIANT* argv)
{
	return call( comauto::utf16string( assembly_utf8_), comauto::utf16string( class_utf8_), comauto::utf16string( method_utf8_), argc, argv);
}


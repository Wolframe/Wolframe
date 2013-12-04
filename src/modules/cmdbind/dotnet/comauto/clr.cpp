#include "comauto/clr.hpp"
#include "comauto/utils.hpp"
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

VARIANT comauto::CommonLanguageRuntime::call( const std::wstring& assembly_, const std::wstring& class_, const std::wstring& method_, LONG argc, const VARIANT* argv, LCID lcid) const
{
	struct Local
	{
		Local() {::memset( this, 0, sizeof( *this)); varResult.vt = VT_EMPTY;}
		~Local()
		{
			comauto::wrapVariantClear( &varResult);
			if (spType) spType->Release();
			if (spAssembly) spAssembly->Release();
			if (spDefaultAppDomain) spDefaultAppDomain->Release();
		}
		EXCEPINFO excepInfo;
		VARIANT varResult; 
		DISPPARAMS dispParams;
		IUnknownPtr spAppDomainThunk;
		_AppDomainPtr spDefaultAppDomain; 
		_AssemblyPtr spAssembly; 
		_TypePtr spType;

		VARIANT getReturnVal()
		{
			VARIANT rt = varResult;
			varResult.vt = VT_EMPTY;
			return rt;
		}
	};
	Local local;

	// The identifiers of the method in the .NET class to invoke:
	bstr_t bstrAssemblyName( assembly_.c_str()); 
	bstr_t bstrClassName( class_.c_str()); 
	bstr_t bstrMethodName( method_.c_str()); 
 
	WRAP( m_runtimehost->GetDefaultDomain( &local.spAppDomainThunk))
	WRAP( local.spAppDomainThunk->QueryInterface( IID_PPV_ARGS( &local.spDefaultAppDomain)))
	WRAP( local.spDefaultAppDomain->Load_2( bstrAssemblyName, &local.spAssembly))
	WRAP( local.spAssembly->GetType_2( bstrClassName, &local.spType))

	// Create an instance of the object to invoke the method:
	variant_t vtObject;
	WRAP( local.spAssembly->CreateInstance( bstrClassName, &vtObject))

	// Initialize the method arguments structure:
	local.dispParams.cNamedArgs = 0;
	local.dispParams.cArgs = argc;
	local.dispParams.rgvarg = const_cast<VARIANT*>(argv);

	// Get the method invoker interface (IDispatch):
	IDispatch* dispatch = NULL;
	WRAP( vtObject.punkVal->QueryInterface( IID_IDispatch, (void**)&dispatch))

	// Get the method handle:
	DISPID gDispId = 0;
	LPOLESTR bstrMethodName_ = bstrMethodName;
	WRAP( dispatch->GetIDsOfNames( IID_NULL, &bstrMethodName_, 1, lcid, &gDispId))

	// Call that method:
	UINT puArgErr;
	HRESULT hr = dispatch->Invoke( gDispId, IID_NULL, lcid, DISPATCH_METHOD, &local.dispParams, &local.varResult, &local.excepInfo, &puArgErr);
	if (hr != S_OK)
	{
		std::string methodname = comauto::utf8string(class_) + "." + comauto::utf8string(method_);
		if (hr == DISP_E_EXCEPTION)
		{
			throw std::runtime_error( std::string("Error calling ") + methodname + " " + comauto::tostring( local.excepInfo));
		}
		else
		{
			_com_error error(hr);
			std::ostringstream errcode; 
			errcode << std::hex << " [0x" << hr << "]";
			throw std::runtime_error( std::string("Error calling ") + methodname + ": '" + comauto::utf8string(error.ErrorMessage()) + errcode.str());
		}
	}
	return local.getReturnVal();
}

VARIANT comauto::CommonLanguageRuntime::call( const std::string& assembly_utf8_, const std::string& class_utf8_, const std::string& method_utf8_, LONG argc, const VARIANT* argv, LCID lcid) const
{
	return call( comauto::utf16string( assembly_utf8_), comauto::utf16string( class_utf8_), comauto::utf16string( method_utf8_), argc, argv, lcid);
}


/************************************************************************
Copyright (C) 2011 - 2013 Project Wolframe.
All rights reserved.

This file is part of Project Wolframe.

Commercial Usage
Licensees holding valid Project Wolframe Commercial licenses may
use this file in accordance with the Project Wolframe
Commercial License Agreement provided with the Software or,
alternatively, in accordance with the terms contained
in a written agreement between the licensee and Project Wolframe.

GNU General Public License Usage
Alternatively, you can redistribute this file and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Wolframe is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
#include "comauto/clr.hpp"
#include "comauto/utils.hpp"
#include <mscoree.h>
#include <metahost.h>
#include <oaidl.h>
#include <comdef.h>
#include <atlbase.h>
#include <atlcom.h>
#pragma comment(lib,"MSCorEE.lib")
#import "mscorlib.tlb" raw_interfaces_only \
high_property_prefixes("_get","_put","_putref") \
rename("ReportEvent",  "InteropServices_ReportEvent") \
rename("value",  "InteropServices_value")

using namespace _Wolframe;
using namespace mscorlib;

class comauto::CommonLanguageRuntime::Impl
{
public:
	ICLRMetaHost *m_metahost;
	ICLRRuntimeInfo *m_runtimeinfo;
	ICorRuntimeHost *m_runtimehost;

	Impl()
		:m_metahost(0),m_runtimeinfo(0),m_runtimehost(0){}
};

comauto::CommonLanguageRuntime::~CommonLanguageRuntime()
{
	m_impl->m_runtimehost->Stop();	//... NOTE: CLR Runtime host cannot be created again in the same process after a Stop
	if (m_impl->m_metahost) m_impl->m_metahost->Release();
	if (m_impl->m_runtimeinfo) m_impl->m_runtimeinfo->Release();
	if (m_impl->m_runtimehost) m_impl->m_runtimehost->Release();
}

comauto::CommonLanguageRuntime::CommonLanguageRuntime( const std::string& version)
	:m_impl( new Impl())
{
	std::wstring version_w( version.begin(), version.end());
	PCWSTR clrversion = version_w.c_str();
	try
	{
		WRAP( ::CLRCreateInstance( CLSID_CLRMetaHost, IID_PPV_ARGS( &m_impl->m_metahost)));
		WRAP( m_impl->m_metahost->GetRuntime( clrversion, IID_PPV_ARGS(&m_impl->m_runtimeinfo)));

		BOOL fLoadable;
		WRAP( m_impl->m_runtimeinfo->IsLoadable( &fLoadable));
		if (!fLoadable)
		{
			throw std::runtime_error( ".NET runtime cannot be loaded");
		}
		WRAP( m_impl->m_runtimeinfo->GetInterface( CLSID_CorRuntimeHost,  IID_PPV_ARGS( &m_impl->m_runtimehost)));
		WRAP( m_impl->m_runtimehost->Start());
	}
	catch (const std::runtime_error& e)
	{
		if (m_impl->m_metahost) m_impl->m_metahost->Release();
		if (m_impl->m_runtimeinfo) m_impl->m_runtimeinfo->Release();
		if (m_impl->m_runtimehost) m_impl->m_runtimehost->Release();
		throw e;
	}
}

void comauto::CommonLanguageRuntime::call( tagVARIANT* res, const std::wstring& assembly_, const std::wstring& class_, const std::wstring& method_, unsigned int argc, const tagVARIANT* argv, unsigned int lcid) const
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

		void getReturnVal( VARIANT* res)
		{
			std::memcpy( res, &varResult, sizeof(varResult));
			varResult.vt = VT_EMPTY;
			return;
		}
	};
	Local local;

	// The identifiers of the method in the .NET class to invoke:
	bstr_t bstrAssemblyName( assembly_.c_str()); 
	bstr_t bstrClassName( class_.c_str()); 
	bstr_t bstrMethodName( method_.c_str()); 
 
	WRAP( m_impl->m_runtimehost->GetDefaultDomain( &local.spAppDomainThunk));
	WRAP( local.spAppDomainThunk->QueryInterface( IID_PPV_ARGS( &local.spDefaultAppDomain)));
	WRAP( local.spDefaultAppDomain->Load_2( bstrAssemblyName, &local.spAssembly));
	WRAP( local.spAssembly->GetType_2( bstrClassName, &local.spType));

	// Create an instance of the object to invoke the method:
	variant_t vtObject;
	WRAP( local.spAssembly->CreateInstance( bstrClassName, &vtObject));

	// Initialize the method arguments structure:
	local.dispParams.cNamedArgs = 0;
	local.dispParams.cArgs = argc;
	local.dispParams.rgvarg = const_cast<VARIANT*>(argv);

	// Get the method invoker interface (IDispatch):
	IDispatch* dispatch = NULL;
	WRAP( vtObject.punkVal->QueryInterface( IID_IDispatch, (void**)&dispatch));

	// Get the method handle:
	DISPID gDispId = 0;
	LPOLESTR bstrMethodName_ = bstrMethodName;
	WRAP( dispatch->GetIDsOfNames( IID_NULL, &bstrMethodName_, 1, lcid, &gDispId));

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
	local.getReturnVal( res);
}

void comauto::CommonLanguageRuntime::call( tagVARIANT* res, const std::string& assembly_utf8_, const std::string& class_utf8_, const std::string& method_utf8_, unsigned int argc, const tagVARIANT* argv, unsigned int lcid) const
{
	call( res, comauto::utf16string( assembly_utf8_), comauto::utf16string( class_utf8_), comauto::utf16string( method_utf8_), argc, argv, lcid);
}


#ifndef _Wolframe_COM_AUTOMATION_CLR_HPP_INCLUDED
#define _Wolframe_COM_AUTOMATION_CLR_HPP_INCLUDED
#include "comauto/utils.hpp"
#include <cstring>
#include <string>
#include <vector>
#include <oaidl.h>
#include <comdef.h>
#include <atlbase.h>
#include <atlcom.h>
#include <metahost.h>

namespace _Wolframe {
namespace comauto {

///\remark This object cann only be created and destroyed once (because runtime host cannot be created again in the same process after a stop)
class CommonLanguageRuntime
{
public:
	explicit CommonLanguageRuntime( const std::string& version);
	virtual ~CommonLanguageRuntime();

	VARIANT call( const std::wstring& assembly_, const std::wstring& class_, const std::wstring& method_, LONG argc, const VARIANT* argv, LCID lcid=LOCALE_USER_DEFAULT) const;
	VARIANT call( const std::string& assembly_utf8_, const std::string& class_utf8_, const std::string& method_utf8_, LONG argc, const VARIANT* argv, LCID lcid=LOCALE_USER_DEFAULT) const;

private:
	ICLRMetaHost *m_metahost;
	ICLRRuntimeInfo *m_runtimeinfo;
	ICorRuntimeHost *m_runtimehost;
};

}} //namespace
#endif

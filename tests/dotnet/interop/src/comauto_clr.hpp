#ifndef _Wolframe_COM_AUTOMATION_CLR_HPP_INCLUDED
#define _Wolframe_COM_AUTOMATION_CLR_HPP_INCLUDED
#include "comauto_utils.hpp"
#include <cstring>
#include <string>
#include <vector>
#include <oaidl.h>
#include <comdef.h>
#include <atlbase.h>
#include <atlcom.h>
#include <metahost.h>

namespace comauto {

///\remark This object cann only be created and destroyed once (because runtime host cannot be created again in the same process after a stop)
class CommonLanguageRuntime
{
public:
	explicit CommonLanguageRuntime();
	virtual ~CommonLanguageRuntime();

	VARIANT call( const std::wstring& assembly_, const std::wstring& class_, const std::wstring& method_, std::vector<VARIANT> args);
	VARIANT call( const std::string& assembly_utf8_, const std::string& class_utf8_, const std::string& method_utf8_, std::vector<VARIANT> args);

private:
    ICLRMetaHost *m_metahost;
    ICLRRuntimeInfo *m_runtimeinfo;
    ICorRuntimeHost *m_runtimehost;
};

} //namespace
#endif

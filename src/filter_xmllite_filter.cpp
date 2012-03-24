#ifndef WITH_XMLLITE
#error Compiling a XmlLite module without XmlLite support enabled
#endif
#include "filter/xmllite_filter.hpp"
#include "filter/bufferingFilterBase.hpp"
#include <cstddef>
#include <cstring>
#include <vector>
#include <string>
#include <atlbase.h>
#include "xmllite.h"
#include <strsafe.h>
// see: http://msdn.microsoft.com/en-us/magazine/cc163436.aspx
// TODO: make it streamable

using namespace _Wolframe;
using namespace langbind;

namespace {

struct XmlLiteInit
{
	XmlLiteInit()
	{
	}

	~XmlLiteInit()
	{
	}
};
static XmlLiteInit XmlLiteInit;

class Content
{
public:
	Content( const CountedReference<std::string>& e, bool withEmpty=true) :m_encoding(e){}

	bool end() const
	{
		return true;
	}

	///\brief Get the last error, if the filter got into an error state
	///\return the last error as string or 0
	const char* getLastError() const
	{
		return m_error.size()?m_error.c_str():0;
	}

	bool open( const void* content, std::size_t size )
	{
		HRESULT h;

		h = CreateStreamOnHGlobal( 0, TRUE, &m_s );
		
		return false;
	}

	~Content()
	{
	}

	bool fetch( protocol::InputFilter::ElementType* type, void* buffer, std::size_t buffersize, std::size_t* bufferpos)
	{
		return false;
	}

private:

private:
	friend struct InputFilterImpl;
	CountedReference<std::string> m_encoding;
	std::string m_error;
	CComPtr<IStream> m_s;
};

}//end anonymous namespace

class XmlLiteFilter :public Filter
{
public:
	XmlLiteFilter( const char*){}
};

Filter XmlLiteFilterFactory::create( const char*) const
{
	return XmlLiteFilter();
}



#ifndef WITH_MSXML
#error Compiling a MSXML module without MSXML support enabled
#endif
#include "filter/msxml_filter.hpp"
#include "filter/bufferingFilterBase.hpp"
#include <cstddef>
#include <cstring>
#include <vector>
#include <string>
//#include <whateverMSXML...>

using namespace _Wolframe;
using namespace langbind;

namespace {

struct MSXMLInit
{
	MSXMLInit()
	{
	}

	~MSXMLInit()
	{
	}
};
static MSXMLInit MSXMLInit;

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

	bool open( const void* content, std::size_t size)
	{
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
};

}//end anonymous namespace

class MSXMLFilter :public Filter
{
public:
	MSXMLFilter( ){}
};

Filter MSXMLFilterFactory::create( const char*) const
{
	return MSXMLFilter();
}



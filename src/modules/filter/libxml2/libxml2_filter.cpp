#include "libxml2_filter.hpp"
#include "inputfilterImpl.hpp"
#include "outputfilterImpl.hpp"
#include "xsltMapper.hpp"
#include "filter/bufferingfilter.hpp"
#include "types/doctype.hpp"
#include "types/countedReference.hpp"
#include <cstddef>
#include <cstring>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/encoding.h"
#include "libxml/xmlwriter.h"
#include "libxml/xmlsave.h"
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

namespace {

struct LibXml2Init
{
	LibXml2Init()
	{
		LIBXML_TEST_VERSION;
	}

	~LibXml2Init()
	{
		xmlCleanupParser();
	}
};
static LibXml2Init libXml2Init;
}//end anonymous namespace

struct Libxml2Filter :public Filter
{
	Libxml2Filter( const char* encoding=0)
	{
		InputFilterImpl impl;
		m_inputfilter.reset( new BufferingInputFilter( &impl,"libxml2"));
		OutputFilterImpl* oo = new OutputFilterImpl( m_inputfilter.get());
		m_outputfilter.reset( oo);
		if (encoding)
		{
			oo->setEncoding( encoding);
		}
	}
};


class Libxml2FilterType :public FilterType
{
public:
	Libxml2FilterType(){}
	virtual ~Libxml2FilterType(){}

	virtual Filter* create( const std::vector<FilterArgument>& arg) const
	{
		const char* encoding = 0;
		std::vector<FilterArgument>::const_iterator ai = arg.begin(), ae = arg.end();
		for (; ai != ae; ++ai)
		{
			if (ai->first.empty() || boost::algorithm::iequals( ai->first, "encoding"))
			{
				encoding = ai->second.c_str();
				break;
			}
		}
		return encoding?(new Libxml2Filter( encoding)):(new Libxml2Filter());
	}
};

FilterType* _Wolframe::langbind::createLibxml2FilterType()
{
	return new Libxml2FilterType();
}


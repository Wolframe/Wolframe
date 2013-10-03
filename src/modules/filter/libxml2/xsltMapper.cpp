#include "xsltMapper.hpp"
#include "utils/parseUtils.hpp"
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

static const utils::CharTable g_op( ",=");

static boost::shared_ptr<const char*> stylesheetParams( std::vector<langbind::FilterArgument>& arg)
{
	char const** cc = (char const**)std::malloc( (arg.size()*2+1) * sizeof( char*));
	if (!cc) throw std::bad_alloc();
	boost::shared_ptr<const char*> rt( cc, std::free);
	std::vector<langbind::FilterArgument>::iterator gi = arg.begin(), ge = arg.end();
	int idx = 0;
	for (; gi != ge; ++gi)
	{
		if (gi->first.empty())
		{
			gi->first = std::string("_") + boost::lexical_cast<std::string>( ++idx);
		}
		*cc++ = gi->first.c_str();
		*cc++ = gi->second.c_str();
	}
	*cc = 0;
	return rt;
}

XsltMapper::XsltMapper( const xsltStylesheetPtr stylesheet_, const std::vector<langbind::FilterArgument>& arg)
	:m_stylesheet(stylesheet_)
	,m_stylesheet_params_ar(arg)
{
	m_stylesheet_params = stylesheetParams( m_stylesheet_params_ar);
}

XsltMapper::XsltMapper( const XsltMapper& o)
	:m_stylesheet(o.m_stylesheet)
	,m_stylesheet_params_ar(o.m_stylesheet_params_ar)
{
	m_stylesheet_params = stylesheetParams( m_stylesheet_params_ar);
}

DocumentReader XsltMapper::apply( const DocumentReader& o) const
{
	xmlDocPtr res = 0;
	if (!m_stylesheet->encoding && o.encoding())
	{
		xsltStylesheet l_stylesheet;
		std::memcpy( &l_stylesheet, m_stylesheet, sizeof(l_stylesheet));
		l_stylesheet.encoding = const_cast<xmlChar*>(o.encoding());
		if (l_stylesheet.standalone < 0)
		{
			l_stylesheet.standalone = 1;
		}
		res = xsltApplyStylesheet( &l_stylesheet, o.get(), m_stylesheet_params.get());
		res->standalone = l_stylesheet.standalone;
	}
	else if (m_stylesheet->standalone < 0)
	{
		xsltStylesheet l_stylesheet;
		std::memcpy( &l_stylesheet, m_stylesheet, sizeof(l_stylesheet));
		l_stylesheet.standalone = 1;
		res = xsltApplyStylesheet( &l_stylesheet, o.get(), m_stylesheet_params.get());
		res->standalone = l_stylesheet.standalone;
	}
	else
	{
		res = xsltApplyStylesheet( m_stylesheet, o.get(), m_stylesheet_params.get());
		res->standalone = m_stylesheet->standalone;
	}
	if (!res) return DocumentReader();
	return DocumentReader( res);
}

std::string XsltMapper::apply( const std::string& o) const
{
	DocumentReader reader( o.c_str(), o.size());
	DocumentReader mapped( apply( reader));
	return mapped.getContent();
}



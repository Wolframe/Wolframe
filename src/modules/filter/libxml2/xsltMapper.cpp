#include "xsltMapper.hpp"
#include "utils/parseUtils.hpp"
#include <cstdlib>
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

static const utils::CharTable g_comma( ",");

static boost::shared_ptr<const char*> stylesheetParams( std::vector<std::string> arg)
{
	char const** cc = (char const**)std::malloc( (arg.size()+1) * sizeof( char*));
	if (!cc) throw std::bad_alloc();
	boost::shared_ptr<const char*> rt( cc, std::free);
	std::vector<std::string>::const_iterator gi = arg.begin(), ge = arg.end();
	for (; gi != ge; ++gi,++cc)
	{
		*cc = gi->c_str();
	}
	*cc = 0;
	return rt;
}

XsltMapper::XsltMapper( const xsltStylesheetPtr stylesheet_, const std::string& arg)
	:m_stylesheet(stylesheet_)
{
	std::string tok;

	std::string::const_iterator ai = arg.begin(), ae = arg.end();
	char ch;
	while ((ch=utils::parseNextToken( tok, ai, ae, g_comma)) != 0)
	{
		if (ch == ',')
		{
			m_stylesheet_params_mem.push_back( "");
		}
		else
		{
			m_stylesheet_params_mem.push_back( tok);
			ch = utils::parseNextToken( tok, ai, ae, g_comma);
			if (ch == ',') continue;
			if (ch == '\0') break;
			throw std::runtime_error( "comma or argument expected as token separator in XSLT filter argument list");
		}
	}
	m_stylesheet_params = stylesheetParams( m_stylesheet_params_mem);
}

XsltMapper::XsltMapper( const XsltMapper& o)
	:m_stylesheet(o.m_stylesheet)
	,m_stylesheet_params_mem(o.m_stylesheet_params_mem)
{
	m_stylesheet_params = stylesheetParams( m_stylesheet_params_mem);
}

DocumentReader XsltMapper::apply( const DocumentReader& o) const
{
	xmlDocPtr res = xsltApplyStylesheet( m_stylesheet, o.get(), m_stylesheet_params.get());
	if (!res) return DocumentReader();
	return DocumentReader( res);
}

std::string XsltMapper::apply( const std::string& o) const
{
	DocumentReader reader( o.c_str(), o.size());
	DocumentReader mapped( apply( reader));
	return mapped.getContent();
}



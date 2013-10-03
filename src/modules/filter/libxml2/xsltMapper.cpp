#include "xsltMapper.hpp"
#include "utils/parseUtils.hpp"
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

static const utils::CharTable g_op( ",=");

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
	int idx;

	std::string::const_iterator ai = arg.begin(), ae = arg.end();
	char ch;
	while ((ch=utils::parseNextToken( tok, ai, ae, g_op)) != 0)
	{
		if (ch == ',' || ch == '=')
		{
			throw std::runtime_error( std::string("argument expected instead of '") + ch + "'");
		}
		else
		{
			std::string val;
			ch = utils::parseNextToken( val, ai, ae, g_op);
			if (ch == '=')
			{
				m_stylesheet_params_mem.push_back( tok);
				ch = utils::parseNextToken( val, ai, ae, g_op);
				if (ch == ',' || ch == '=')
				{
					throw std::runtime_error( std::string("argument expected instead of '") + ch + "'");
				}
				m_stylesheet_params_mem.push_back( val);
				ch = utils::parseNextToken( val, ai, ae, g_op);
			}
			else if (ch == ',')
			{
				val = tok;
				tok = std::string("_") + boost::lexical_cast<std::string>( ++idx);
				m_stylesheet_params_mem.push_back( tok);
				m_stylesheet_params_mem.push_back( val);
			}
			else
			{
				throw std::runtime_error( "comma (',') or equal ('=') expected");
			}
			if (ch == ',') continue;
			if (ch == '\0') break;
			throw std::runtime_error( "comma expected as token separator in XSLT filter argument list");
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
	xmlDocPtr res = 0;
	if (!m_stylesheet->encoding && o.encoding())
	{
		xsltStylesheet l_stylesheet;
		std::memcpy( &l_stylesheet, m_stylesheet, sizeof(l_stylesheet));
		l_stylesheet.encoding = const_cast<xmlChar*>(o.encoding());
		res = xsltApplyStylesheet( &l_stylesheet, o.get(), m_stylesheet_params.get());
	}
	else
	{
		res = xsltApplyStylesheet( m_stylesheet, o.get(), m_stylesheet_params.get());
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



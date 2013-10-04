#include "xsltProgramType.hpp"
#include "xsltMapper.hpp"
#include "inputfilterImpl.hpp"
#include "outputfilterImpl.hpp"
#include "processor/procProvider.hpp"
#include "module/filterBuilder.hpp"
#include "utils/fileUtils.hpp"
#include "logger-v1.hpp"
#include <boost/shared_ptr.hpp>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

namespace {

struct XsltFilter :public Filter
{
	XsltFilter( const xsltStylesheetPtr stylesheet_, const std::vector<langbind::FilterArgument>& arg)
	{
		XsltMapper xsltmapper( stylesheet_, arg);
		InputFilterImpl impl( xsltmapper);
		m_inputfilter.reset( new BufferingInputFilter( &impl));
		m_outputfilter.reset( new OutputFilterImpl( xsltmapper));
	}
};


class XsltFilterConstructor
	:public module::FilterConstructor
{
public:
	XsltFilterConstructor( const std::string& sourcefile_)
		:module::FilterConstructor( utils::getFileStem(sourcefile_), 0)
	{
		xsltStylesheetPtr pp = xsltParseStylesheetFile( (const xmlChar *)sourcefile_.c_str());
		if (!pp)
		{
			xmlError* err = xmlGetLastError();
			throw std::runtime_error( std::string("error loading XSLT program '") + sourcefile_ + "': '" + (err?err->message:"unspecified XSLT error") + "'");
		}
		m_ptr = boost::shared_ptr<xsltStylesheet>( pp, xsltFreeStylesheet);
	}

	virtual ~XsltFilterConstructor(){}

	virtual langbind::Filter* object( const std::vector<langbind::FilterArgument>& arg) const
	{
		return new XsltFilter( m_ptr.get(), arg);
	}

private:
	boost::shared_ptr<xsltStylesheet> m_ptr;
};


class XsltProgramType
	:public prgbind::Program
{
public:
	XsltProgramType()
		:prgbind::Program( prgbind::Program::Function){}

	virtual ~XsltProgramType(){}

	virtual bool is_mine( const std::string& filename) const
	{
		boost::filesystem::path p( filename);
		return p.extension().string() == ".xslt";
	}

	virtual void loadProgram( prgbind::ProgramLibrary& library, db::Database* /*transactionDB*/, const std::string& filename)
	{
		module::FilterConstructorR fc( new XsltFilterConstructor( filename));
		library.defineFilterConstructor( fc);
	}
};
}//anonymous namespace

prgbind::Program* langbind::createXsltProgramType()
{
	return new XsltProgramType;
}


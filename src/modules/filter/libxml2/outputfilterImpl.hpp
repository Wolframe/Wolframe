/************************************************************************
Copyright (C) 2011 - 2014 Project Wolframe.
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
/// \file outputfilterImpl.hpp
/// \brief Output filter abstraction for the libxml2 library

#ifndef _Wolframe_LIBXML2_OUTPUT_FILTER_HPP_INCLUDED
#define _Wolframe_LIBXML2_OUTPUT_FILTER_HPP_INCLUDED
#include "documentWriter.hpp"
#include "xsltMapper.hpp"
#include "types/countedReference.hpp"
#include "types/docmetadata.hpp"
#include "filter/outputfilter.hpp"
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/encoding.h"
#include "libxml/xmlwriter.h"
#include "libxml/xmlsave.h"
#include <cstdlib>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

class OutputFilterImpl
	:public OutputFilter
{
public:
	typedef OutputFilter Parent;
	/// \brief Constructor
	OutputFilterImpl( const XsltMapper& xsltMapper_, const types::DocMetaDataR& inheritMetaData_);

	/// \brief Constructor
	explicit OutputFilterImpl( const types::DocMetaDataR& inheritMetaData_);

	/// \brief Copy constructor
	OutputFilterImpl( const OutputFilterImpl& o);

	/// \brief Destructor
	virtual ~OutputFilterImpl(){}

	/// \brief Implementation of OutputFilter::copy()
	virtual OutputFilterImpl* copy() const
	{
		return new OutputFilterImpl( *this);
	}

	/// \brief Implementation of OutputFilter::print( ElementType, const void*,std::size_t)
	virtual bool print( ElementType type, const void* element, std::size_t elementsize);

	/// \brief Implementation of OutputFilter::getOutput( const void*&,std::size_t&)
	virtual void getOutput( const void*& buf, std::size_t& bufsize);

	/// \brief Implementation of OutputFilter::close()
	virtual bool close();

	/// \brief Implementation of FilterBase::getValue( const char*, std::string&)
	virtual bool getValue( const char* name, std::string& val) const;

	/// \brief Implementation of FilterBase::setValue( const char*, const std::string&)
	virtual bool setValue( const char* name, const std::string& value);

private:
	bool printHeader();
	bool printTrailer();

	static const xmlChar* getXmlString( const std::string& aa)
	{
		return (const xmlChar*)aa.c_str();
	}

	const xmlChar* getElement( const void* element, std::size_t elementsize)
	{
		m_valuestrbuf.clear();
		m_valuestrbuf.append( (const char*)element, elementsize);
		return (const xmlChar*)m_valuestrbuf.c_str();
	}

	void setXmlError( const char* msg);

private:
	DocumentWriter m_doc;					///< document writer structure
	XsltMapper m_xsltMapper;				///< optional XSLT mapper
	int m_taglevel;						///< tag hierarchy level
	bool m_trailerPrinted;					///< true if the trailer has been printed
	std::string m_attribname;				///< attribute name buffer
	std::string m_valuestrbuf;				///< value buffer
	std::string m_elembuf;					///< buffer for current element
	std::size_t m_elemitr;					///< iterator on current element
};

}}//namespace
#endif


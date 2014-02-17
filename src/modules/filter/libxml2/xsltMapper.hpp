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
///\file xsltMapper.hpp
///\brief Mapper for constructing an XSLT filter and applying it on an XML content

#ifndef _Wolframe_LIBXML2_XSLT_MAPPER_HPP_INCLUDED
#define _Wolframe_LIBXML2_XSLT_MAPPER_HPP_INCLUDED
#include "documentReader.hpp"
#include "filter/filter.hpp"
#include <string>
#include <vector>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

namespace _Wolframe {
namespace langbind {

class XsltMapper
{
public:
	XsltMapper()
		:m_stylesheet(0){}
	XsltMapper( const xsltStylesheetPtr stylesheet_, const std::vector<langbind::FilterArgument>& arg);
	XsltMapper( const XsltMapper& o);

	DocumentReader apply( const DocumentReader& o) const;
	std::string apply( const std::string& o) const;

	bool defined() const
		{return m_stylesheet != 0;}

private:
	const xsltStylesheetPtr m_stylesheet;				//< optional XSLT mapper
	boost::shared_ptr<const char*> m_stylesheet_params;		//< parameter for XSLT mapper
	std::vector<langbind::FilterArgument> m_stylesheet_params_ar;	//< memory for parameter for XSLT mapper
};

}}//namespace
#endif


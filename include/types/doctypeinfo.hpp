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
/// \file types/doctypeinfo.hpp
/// \brief Structure for document type and format

#ifndef _Wolframe_TYPES_DOCTYPE_INFO_HPP_INCLUDED
#define _Wolframe_TYPES_DOCTYPE_INFO_HPP_INCLUDED
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace types {

/// \class DoctypeInfo
/// \brief Document type information representation created by a document type recognizer.
class DoctypeInfo
{
public:
	/// \brief Copy constructor
	DoctypeInfo( const DoctypeInfo& o)
		:m_docformat(o.m_docformat),m_doctype(o.m_doctype){}
	/// \brief Constructor
	DoctypeInfo( const std::string& docformat_, const std::string& doctype_)
		:m_docformat(docformat_),m_doctype(doctype_){}
	/// \brief Default constructor
	DoctypeInfo(){}

	/// \brief Get the document format
	const std::string& docformat() const
	{
		return m_docformat;
	}

	/// \brief Get the document type
	const std::string& doctype() const
	{
		return m_doctype;
	}

private:
	std::string m_docformat;	///< format of the document (e.g. 'XML','JSON','CSV',etc...)
	std::string m_doctype;		///< type of the document (doctype identifier)
};

typedef boost::shared_ptr<DoctypeInfo> DoctypeInfoR;

}}//namespace
#endif



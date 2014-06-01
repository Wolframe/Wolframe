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
/// \file cmdbind/doctypeDetector.hpp
/// \brief Interface for document type and format recognition

#ifndef _Wolframe_CMDBIND_DOCTYPE_DETECTOR_HPP_INCLUDED
#define _Wolframe_CMDBIND_DOCTYPE_DETECTOR_HPP_INCLUDED
#include "types/doctypeinfo.hpp"
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace cmdbind {

/// \class DoctypeDetector
/// \brief Interface to document type and format detection.
struct DoctypeDetector
{
public:
	/// \brief Try to detect the document type format
	/// \param[in] data document to process as UTF-8 string
	/// \return true, if the detection process has come to a result (negative or positive), false if the detection process needs more data
	virtual bool detect( const std::string& data)=0;

	/// \brief Get the result of document type and format recognition
	/// \return a doctype info reference in case of successful recognition, null in case of document format not recognized
	const boost::shared_ptr<types::DoctypeInfo>& info() const=0;
};

}}//namespace
#endif


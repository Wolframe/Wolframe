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
	/// \brief Destructor
	virtual ~DoctypeDetector(){}

	/// \brief Feed document type detection with more data
	/// \param[in] chunk first/next chunk of data of the document to process
	/// \param[in] chunksize size of chunk in bytes
	/// \remark The implementation does not have to buffer the input because run is called when references to chunk are still valid
	virtual void putInput( const char* chunk, std::size_t chunksize, bool eof)=0;

	/// \brief Start or continue running the document type and format detection
	/// \return true, if the detection process has come to a result (negative or positive), false if the detection process needs more data or there was an error (check with 'lastError()'
	virtual bool run()=0;

	/// \brief Get the last error occurred
	/// \note Use this function to check for an error if 'run()' returned false
	/// \return the message or NULL if there was no error
	virtual const char* lastError() const=0;

	/// \brief Get the result of document type and format recognition
	/// \return a doctype info reference in case of successful recognition, null in case of document format not recognized
	virtual const boost::shared_ptr<types::DoctypeInfo>& info() const=0;
};

/// \brief Shared doctype detector reference
typedef boost::shared_ptr<DoctypeDetector> DoctypeDetectorR;

/// \brief Constructor function for doctype detector instance
typedef DoctypeDetector* (*CreateDoctypeDetector)();


/// \class DoctypeDetectorType
/// \brief Constructor as class
class DoctypeDetectorType
{
public:
	/// \brief Default constructor
	DoctypeDetectorType()
		:m_create(0){}
	/// \brief Constructor
	DoctypeDetectorType( const std::string& name_, CreateDoctypeDetector create_)
		:m_name(name_),m_create(create_){}

	DoctypeDetectorType( const DoctypeDetectorType& o)
		:m_name(o.m_name),m_create(o.m_create){}

	/// \brief Create an instance
	DoctypeDetector* create() const
	{
		return m_create();
	}

	/// \brief Get the doctype detector name (usually docformat name)
	const std::string& name() const
	{
		return m_name;
	}

private:
	std::string m_name;
	CreateDoctypeDetector m_create;
};

}}//namespace
#endif


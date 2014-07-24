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
/// \file appdevel/module/doctypeDetectorBuilder.hpp
/// \brief Interface template for object builder of filters
#ifndef _Wolframe_MODULE_DOCYPE_DETECTOR_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#define _Wolframe_MODULE_DOCYPE_DETECTOR_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#include "cmdbind/doctypeDetector.hpp"
#include "types/countedReference.hpp"
#include "module/moduleInterface.hpp"
#include "module/constructor.hpp"

namespace _Wolframe {
namespace module {

/// \class DoctypeDetectorConstructor
/// \brief Document type and format detector constructor
class DoctypeDetectorConstructor
	:public SimpleObjectConstructor<cmdbind::DoctypeDetectorType>
{
public:
	/// \brief Constructor
	DoctypeDetectorConstructor( const std::string& name_, cmdbind::CreateDoctypeDetector createDoctypeDetector_)
		:m_name(name_)
		,m_createDoctypeDetector(createDoctypeDetector_) {}

	/// \brief Destructor
	virtual ~DoctypeDetectorConstructor(){}

	/// \brief Get the object type of the module object
	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return DOCTYPE_DETECTOR_OBJECT;
	}

	/// \brief Get the object class name of the module object
	virtual const char* objectClassName() const
	{
		return m_name.c_str();
	}

	/// \brief Construct an object instance
	virtual cmdbind::DoctypeDetectorType* object() const
	{
		return new cmdbind::DoctypeDetectorType( m_name, m_createDoctypeDetector);
	}

	/// \brief Get the name of the constructor object
	const std::string& name() const
	{
		return m_name;
	}

private:
	std::string m_name;
	cmdbind::CreateDoctypeDetector m_createDoctypeDetector;
};

/// \brief Shared doctype detector constructor reference
typedef types::CountedReference<DoctypeDetectorConstructor> DoctypeDetectorConstructorR;


/// \brief Document type and format detector builder
class DoctypeDetectorBuilder
	:public SimpleBuilder
{
public:
	/// \brief Constructor
	DoctypeDetectorBuilder( const char* className_, const char* name_, cmdbind::CreateDoctypeDetector createDoctypeDetector_)
		:SimpleBuilder( className_)
		,m_name(name_)
		,m_createDoctypeDetector(createDoctypeDetector_)
		{}

	/// \brief Destructor
	virtual ~DoctypeDetectorBuilder(){}

	/// \brief Get the object type of the module object
	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return ObjectConstructorBase::DOCTYPE_DETECTOR_OBJECT;
	}
	/// \brief Create an object constructor
	virtual ObjectConstructorBase* constructor()
	{
		return new DoctypeDetectorConstructor( objectClassName(), m_createDoctypeDetector);
	}

	const std::string& name() const
	{
		return m_name;
	}

private:
	std::string m_name;
	cmdbind::CreateDoctypeDetector m_createDoctypeDetector;
};

}}//namespace

#endif


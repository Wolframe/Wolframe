/************************************************************************
Copyright (C) 2011 - 2013 Project Wolframe.
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
///\file types/customDataType.hpp
///\brief Implementation of custom data type for variant
#include "types/customDataType.hpp"
#include "types/variant.hpp"

using namespace _Wolframe;
using namespace _Wolframe::types;

CustomDataType::CustomDataType(
			const std::string& name_,
			CustomDataValueConstructor constructor_,
			CustomDataValueCopyConstructor copyconstructor_,
			CreateCustomDataInitializer initializerconstructor_)
	:m_name(name_),m_id(0)
{
	std::memset( &m_vmt, 0, sizeof( m_vmt));
	m_vmt.opInitializerConstructor = initializerconstructor_;
	m_vmt.opConstructor = constructor_;
	m_vmt.opCopyConstructor = copyconstructor_;
}

CustomDataType::CustomDataType( const CustomDataType& o)
	:m_name(o.m_name),m_id(o.m_id)
{
	std::memcpy( &m_vmt, &o.m_vmt, sizeof( m_vmt));
}

void CustomDataType::define( UnaryOperatorType type, UnaryOperator op)
{
	if (m_vmt.opUnary[ (int)type]) throw std::logic_error("duplicate definition of unary operator");
	m_vmt.opUnary[ (int)type] = op;
}

void CustomDataType::define( BinaryOperatorType type, BinaryOperator op)
{
	if (m_vmt.opBinary[ (int)type]) throw std::logic_error("duplicate definition of binary operator");
	m_vmt.opBinary[ (int)type] = op;
}

void CustomDataType::define( DimensionOperatorType type, DimensionOperator op)
{
	if (m_vmt.opDimension[ (int)type]) throw std::logic_error("duplicate definition of dimension operator");
	m_vmt.opDimension[ (int)type] = op;
}

CustomDataType::UnaryOperator CustomDataType::getOperator( CustomDataType::UnaryOperatorType type) const
{
	return m_vmt.opUnary[ (int)type];
}

CustomDataType::BinaryOperator CustomDataType::getOperator( CustomDataType::BinaryOperatorType type) const
{
	return m_vmt.opBinary[ (int)type];
}

CustomDataType::DimensionOperator CustomDataType::getOperator( CustomDataType::DimensionOperatorType type) const
{
	return m_vmt.opDimension[ (int)type];
}

CustomDataInitializer* CustomDataType::createInitializer( const std::string& d) const
{
	return (*m_vmt.opInitializerConstructor)( d);
}

CustomDataValue* CustomDataType::createValue( const CustomDataInitializer* i) const
{
	CustomDataValue* rt = (*m_vmt.opConstructor)( i);
	rt->m_type = this;
	rt->m_initializer = i;
	return rt;
}

CustomDataValue* CustomDataType::copyValue( const CustomDataValue& o) const
{
	CustomDataValue* rt = (*m_vmt.opCopyConstructor)( &o);
	rt->m_type = this;
	rt->m_initializer = o.m_initializer;
	return rt;
}



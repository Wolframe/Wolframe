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
///\file types/abstractDataType.hpp
///\brief Abstract data type (ADT) interface for variant

#ifndef _Wolframe_TYPES_ABSTRACT_DATA_TYPE_HPP_INCLUDED
#define _Wolframe_TYPES_ABSTRACT_DATA_TYPE_HPP_INCLUDED
#include <string>
#include <cstring>
#include "types/countedReference.hpp"
#include "types/normalizeFunction.hpp"

namespace _Wolframe {
namespace proc
{
	//\brief Forward declaration
	class ProcessorProvider;
}
namespace types {

//\brief Forward declaration
class Variant;
//\brief Forward declaration
class AbstractDataType;
//\brief Forward declaration
class AbstractDataInitializer;


class AbstractDataValue
{
public:
	AbstractDataValue()
		:m_type(0),m_initializer(0){}
	virtual ~AbstractDataValue(){};

	const AbstractDataType* type() const			{return m_type;}
	const AbstractDataInitializer* initializer() const	{return m_initializer;}

	virtual int compare( const AbstractDataValue& o) const=0;
	virtual std::string tostring() const=0;
	virtual void assign( const Variant& o)=0;

private:
	friend class AbstractDataType;
	const AbstractDataType* m_type;
	const AbstractDataInitializer* m_initializer;
};

typedef types::CountedReference<AbstractDataValue> AbstractDataValueR;


class AbstractDataInitializer
{
public:
	AbstractDataInitializer(){}
	virtual ~AbstractDataInitializer(){};
};

typedef types::CountedReference<AbstractDataInitializer> AbstractDataInitializerR;


typedef AbstractDataInitializer* (*CreateAbstractDataInitializer)( const std::string& description);
typedef AbstractDataValue* (*AbstractDataValueConstructor)( const AbstractDataInitializer* initializer);
typedef AbstractDataValue* (*AbstractDataValueCopyConstructor)( const AbstractDataValue* o);


class AbstractDataType
{
public:
	typedef unsigned int ID;
	enum {NofUnaryOperators=3};
	enum UnaryOperatorType {Increment,Decrement,Negation};
	enum {NofBinaryOperators=6};
	enum BinaryOperatorType {Add,Subtract,Multiply,Divide,Power,Concat};
	enum {NofDimensionOperators=1};
	enum DimensionOperatorType {Length};

	typedef types::Variant (*UnaryOperator)( const AbstractDataValue& operand);
	typedef types::Variant (*BinaryOperator)( const AbstractDataValue& operand, const Variant& arg);
	typedef std::size_t (*DimensionOperator)( const AbstractDataValue& arg);

public:
	AbstractDataType()
		:m_name("null"),m_id(0)
	{
		std::memset( &m_vmt, 0, sizeof( m_vmt));
	}

	AbstractDataType( const std::string& name_,
				AbstractDataValueConstructor constructor_,
				AbstractDataValueCopyConstructor copyconstructor_,
				CreateAbstractDataInitializer initializerconstructor_);

	AbstractDataType( const AbstractDataType& o);

	void define( UnaryOperatorType type, UnaryOperator op);
	void define( BinaryOperatorType type, BinaryOperator op);
	void define( DimensionOperatorType type, DimensionOperator op);

	UnaryOperator getOperator( UnaryOperatorType type) const;
	BinaryOperator getOperator( BinaryOperatorType type) const;
	DimensionOperator getOperator( DimensionOperatorType type) const;

	AbstractDataInitializer* createInitializer( const std::string& d) const;
	AbstractDataValue* createValue( const AbstractDataInitializer* i=0) const;
	AbstractDataValue* copyValue( const AbstractDataValue& o) const;

	const ID& id() const			{return m_id;}
	const std::string& name() const		{return m_name;}

private:
	std::string m_name;
	friend class proc::ProcessorProvider;
	ID m_id;

	struct
	{
		UnaryOperator opUnary[ NofUnaryOperators];
		BinaryOperator opBinary[ NofBinaryOperators];
		DimensionOperator opDimension[ NofDimensionOperators];
		CreateAbstractDataInitializer opInitializerConstructor;
		AbstractDataValueConstructor opConstructor;
		AbstractDataValueCopyConstructor opCopyConstructor;
	}
	m_vmt;
};

typedef types::CountedReference<AbstractDataType> AbstractDataTypeR;


typedef AbstractDataType (*CreateAbstractDataType)( const std::string& name);



class AbstractDataNormalizer
	:public types::NormalizeFunction
{
public:
	AbstractDataNormalizer( const std::string& name_, const AbstractDataType* type_, const AbstractDataInitializer* initializer_)
		:m_name(name_)
		,m_type(type_)
		,m_initializer(initializer_){}

	virtual ~AbstractDataNormalizer(){}

	virtual const char* name() const
	{
		return m_name.c_str();
	}

	virtual Variant execute( const Variant& i) const;

private:
	std::string m_name;
	const AbstractDataType* m_type;
	const AbstractDataInitializer* m_initializer;
};

}}//namespace
#endif

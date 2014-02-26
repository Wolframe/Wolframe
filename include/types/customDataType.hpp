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
///\file types/customDataType.hpp
///\brief Custom data type interface for variant

#ifndef _Wolframe_TYPES_CUSTOM_DATA_TYPE_HPP_INCLUDED
#define _Wolframe_TYPES_CUSTOM_DATA_TYPE_HPP_INCLUDED
#include <string>
#include <vector>
#include <cstring>
#include <boost/shared_ptr.hpp>

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
class CustomDataType;
//\brief Forward declaration
class CustomDataInitializer;
//\brief Forward declaration
class DateTime;
//\brief Forward declaration
class BigNumber;


class CustomDataValue
{
public:
	CustomDataValue()
		:m_type(0),m_initializer(0){}
	CustomDataValue( const CustomDataValue& o)
		:m_type(o.m_type),m_initializer(o.m_initializer){}
	virtual ~CustomDataValue(){};

	const CustomDataType* type() const				{return m_type;}
	const CustomDataInitializer* initializer() const		{return m_initializer;}

	virtual int compare( const CustomDataValue& o) const=0;
	virtual std::string tostring() const=0;
	virtual void assign( const Variant& o)=0;
	virtual CustomDataValue* copy() const=0;
	virtual void getBaseTypeValue( Variant& dest) const;

private:
	friend class CustomDataType;
	const CustomDataType* m_type;
	const CustomDataInitializer* m_initializer;
};

typedef boost::shared_ptr<CustomDataValue> CustomDataValueR;


class CustomDataInitializer
{
public:
	CustomDataInitializer(){}
	virtual ~CustomDataInitializer(){};
};

typedef boost::shared_ptr<CustomDataInitializer> CustomDataInitializerR;


typedef CustomDataInitializer* (*CreateCustomDataInitializer)( const std::vector<types::Variant>& arg);
typedef CustomDataValue* (*CustomDataValueConstructor)( const CustomDataInitializer* initializer);


class CustomDataType
{
public:
	typedef unsigned int ID;
	enum {NofUnaryOperators=3};
	enum UnaryOperatorType {Increment,Decrement,Negation};
	static const char* unaryOperatorTypeName( UnaryOperatorType i)
	{
		static const char* ar[] = {"increment","decrement","negation"};
		return ar[(int)i];
	}
	enum {NofBinaryOperators=6};
	enum BinaryOperatorType {Add,Subtract,Multiply,Divide,Power,Concat};
	static const char* binaryOperatorTypeName( BinaryOperatorType i)
	{
		static const char* ar[] = {"add","subtract","multiply","divide","power","concat"};
		return ar[(int)i];
	}
	enum {NofConversionOperators=5};
	enum ConversionOperatorType {ToString,ToInt,ToUInt,ToDouble,ToTimestamp};
	static const char* conversionOperatorTypeName( ConversionOperatorType i)
	{
		static const char* ar[] = {"tostring","toint","touint","todouble","totimestamp"};
		return ar[(int)i];
	}
	enum {NofDimensionOperators=1};
	enum DimensionOperatorType {Length};
	static const char* dimensionOperatorTypeName( DimensionOperatorType i)
	{
		static const char* ar[] = {"length"};
		return ar[(int)i];
	}

	typedef types::Variant (*ConversionOperator)( const CustomDataValue& operand);
	typedef types::Variant (*UnaryOperator)( const CustomDataValue& operand);
	typedef types::Variant (*BinaryOperator)( const CustomDataValue& operand, const Variant& arg);
	typedef std::size_t (*DimensionOperator)( const CustomDataValue& arg);

public:
	CustomDataType()
		:m_id(0)
	{
		std::memset( &m_vmt, 0, sizeof( m_vmt));
	}

	CustomDataType( const std::string& name_,
			CustomDataValueConstructor constructor_,
			CreateCustomDataInitializer initializerconstructor_=0);

	CustomDataType( const CustomDataType& o);

	void define( UnaryOperatorType type, UnaryOperator op);
	void define( BinaryOperatorType type, BinaryOperator op);
	void define( ConversionOperatorType type, ConversionOperator op);
	void define( DimensionOperatorType type, DimensionOperator op);

	ConversionOperator getOperator( ConversionOperatorType type) const;
	UnaryOperator getOperator( UnaryOperatorType type) const;
	BinaryOperator getOperator( BinaryOperatorType type) const;
	DimensionOperator getOperator( DimensionOperatorType type) const;

	CustomDataInitializer* createInitializer( const std::vector<types::Variant>& arg) const;
	CustomDataValue* createValue( const CustomDataInitializer* i=0) const;

	bool hasInitializer() const		{return !!m_vmt.opInitializerConstructor;}
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
		ConversionOperator opConversion[ NofConversionOperators];
		CreateCustomDataInitializer opInitializerConstructor;
		CustomDataValueConstructor opConstructor;
	}
	m_vmt;
};

typedef boost::shared_ptr<CustomDataType> CustomDataTypeR;


typedef CustomDataType* (*CreateCustomDataType)( const std::string& name);


}}//namespace
#endif

#include "appdevel/moduleFrameMacros.hpp"
#include "appdevel/customDataTypeModuleMacros.hpp"
#include "types/customDataType.hpp"
#include "types/variant.hpp"

class MyInitializer
	:public _Wolframe::types::CustomDataInitializer
{
public:
	MyInitializer( const std::vector<_Wolframe::types::Variant>& arg)
	{
		// ... construct your custom data type initializer object here
	}

	static _Wolframe::types::CustomDataInitializer* create( const std::vector<_Wolframe::types::Variant>& arg)
	{
		return new MyInitializer( arg);
	}
};

class MyValue
	:public _Wolframe::types::CustomDataValue
{
public:
	explicit MyValue( const std::string& dt, const MyInitializer* ini=0)
	{
		// ... put your value constructor from string here
	}

	virtual int compare( const _Wolframe::types::CustomDataValue& o) const
	{
		// ... put your compare function returning -1,0 or +1 here
	}

	virtual std::string tostring() const
	{
		// ... put your conversion of the value to a string here
	}

	virtual void assign( const _Wolframe::types::Variant& o)
	{
		// ... put the assignment from a variant data type here
	}

	virtual bool getBaseTypeValue( _Wolframe::types::Variant& dest) const
	{
		// ... try to convert this value to a variant data type returned in dest
		// ... return false, if not possible
	}

	virtual _Wolframe::types::CustomDataValue* copy() const
	{
		// ... return an exact copy of this here
	}

	static _Wolframe::types::CustomDataValue* create( const _Wolframe::types::CustomDataInitializer* ini)
	{
		// ... create a default value from initializer (can be NULL) here
	}
};

class MyType
	:public _Wolframe::types::CustomDataType
{
public:
	MyType( const std::string& name_)
		:_Wolframe::types::CustomDataType(name_,&MyValue::create,&MyInitializer::create)
	{
		define( Increment, &increment);
		define( Add, &add);
		define( "mymethod", &myMethod);
	}

	static _Wolframe::types::Variant myMethod( const _Wolframe::types::CustomDataValue& operand, const std::vector<_Wolframe::types::Variant>& arg )
	{
		// ... the implementation of the method "mymethod" operating on operand with arg as list of arguments comes here
	}
	static _Wolframe::types::Variant increment( const _Wolframe::types::CustomDataValue& operand)
	{
		// ... the increment operator implementation follows here
	}
	static _Wolframe::types::Variant add( const _Wolframe::types::CustomDataValue& operand, const _Wolframe::types::Variant& arg )
	{
		// ... the implementation of the addition operator operating on operand with arg as argument comes here
	}

	static _Wolframe::types::CustomDataType* create( const std::string& name)
	{
		return new MyType( name);
	}
};

WF_MODULE_BEGIN( "mydatatype", "my custom data type module")
 WF_CUSTOM_DATATYPE( "mydatatype", MyType::create)
WF_MODULE_END

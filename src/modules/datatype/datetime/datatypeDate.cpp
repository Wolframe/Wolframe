#include "datatypeDate.hpp"
#include "types/datetime.hpp"

using namespace _Wolframe;
using namespace _Wolframe::types;

types::Variant DateDataType::increment( const CustomDataValue& operand)
{
	const DateDataValue* op = dynamic_cast<const DateDataValue*>(&operand);
	types::Variant rt( op->type(), op->initializer());
	DateDataValue* res = dynamic_cast<DateDataValue*>( rt.data().value.Custom);
	res->Date::operator=( dynamic_cast<const DateDataValue&>(operand).operator+( 1));
	return rt;
}

types::Variant DateDataType::decrement( const CustomDataValue& operand)
{
	const DateDataValue* op = dynamic_cast<const DateDataValue*>(&operand);
	types::Variant rt( op->type(), op->initializer());
	DateDataValue* res = dynamic_cast<DateDataValue*>( rt.data().value.Custom);
	res->Date::operator=( dynamic_cast<const DateDataValue&>(operand).operator-( 1));
	return rt;
}

types::Variant DateDataType::add( const CustomDataValue& operand, const Variant& arg)
{
	const DateDataValue* op = dynamic_cast<const DateDataValue*>(&operand);
	types::Variant rt( op->type(), op->initializer());
	DateDataValue* res = dynamic_cast<DateDataValue*>( rt.data().value.Custom);
	res->Date::operator=( dynamic_cast<const DateDataValue&>(operand).operator - ( arg.toint()));
	return rt;
}

types::Variant DateDataType::subtract( const CustomDataValue& operand, const Variant& arg)
{
	if (arg.type() == types::Variant::Custom)
	{
		if (arg.data().value.Custom->type() == operand.type())
		{
			types::Variant::Data::Int daydiff = dynamic_cast<const DateDataValue&>(operand).operator-( *(const Date*)dynamic_cast<const DateDataValue*>(arg.data().value.Custom));
			return types::Variant( daydiff);
		}
		else
		{
			throw std::runtime_error("illegal argument for date subtraction");
		}
	}
	else 
	{
		const DateDataValue* op = dynamic_cast<const DateDataValue*>(&operand);
		types::Variant rt( op->type(), op->initializer());
		DateDataValue* res = dynamic_cast<DateDataValue*>( rt.data().value.Custom);
		res->Date::operator=( dynamic_cast<const DateDataValue&>(operand).operator - ( arg.toint()));
		return rt;
	}
}

types::Variant DateDataType::toInt( const CustomDataValue& operand)
{
	const DateDataValue* op = dynamic_cast<const DateDataValue*>(&operand);
	types::Variant::Data::Int value = Date(*op) - Date( 1970, 1, 1);
	return types::Variant( value);
}

types::Variant DateDataType::toTimestamp( const CustomDataValue& operand)
{
	const DateDataValue* op = dynamic_cast<const DateDataValue*>(&operand);
	types::DateTime dt( op->year(), op->month()-1, op->day()-1, 0, 0, 0);
	types::Variant::Data::Int value = dt.value();
	return types::Variant( value);
}


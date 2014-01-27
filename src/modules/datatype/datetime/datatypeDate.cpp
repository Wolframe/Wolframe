#include "datatypeDate.hpp"
#include "types/datetime.hpp"

using namespace _Wolframe;
using namespace _Wolframe::types;

types::Variant DateDataType::increment( const CustomDataValue& operand)
{
	const DateDataValue* op = reinterpret_cast<const DateDataValue*>(&operand);
	types::Variant rt( op->type(), op->initializer());
	DateDataValue* res = reinterpret_cast<DateDataValue*>( rt.data().value.Custom);
	res->Date::operator=( op->operator+( 1));
	return rt;
}

types::Variant DateDataType::decrement( const CustomDataValue& operand)
{
	const DateDataValue* op = reinterpret_cast<const DateDataValue*>(&operand);
	types::Variant rt( op->type(), op->initializer());
	DateDataValue* res = reinterpret_cast<DateDataValue*>( rt.data().value.Custom);
	res->Date::operator=( op->operator-( 1));
	return rt;
}

types::Variant DateDataType::add( const CustomDataValue& operand, const Variant& arg)
{
	const DateDataValue* op = reinterpret_cast<const DateDataValue*>(&operand);
	types::Variant rt( op->type(), op->initializer());
	DateDataValue* res = reinterpret_cast<DateDataValue*>( rt.data().value.Custom);
	res->Date::operator=( op->operator + ( arg.toint()));
	return rt;
}

types::Variant DateDataType::subtract( const CustomDataValue& operand, const Variant& arg)
{
	const DateDataValue* op = reinterpret_cast<const DateDataValue*>(&operand);
	if (arg.type() == types::Variant::Custom
	&&  arg.data().value.Custom->type() == operand.type())
	{
		const DateDataValue* datearg = reinterpret_cast<const DateDataValue*>(arg.data().value.Custom);
		types::Variant::Data::Int daydiff = op->Date::operator-( *datearg);
		return types::Variant( daydiff);
	}
	else 
	{
		types::Variant rt( op->type(), op->initializer());
		DateDataValue* res = reinterpret_cast<DateDataValue*>( rt.data().value.Custom);
		res->Date::operator=( op->operator-( arg.toint()));
		return rt;
	}
}

types::Variant DateDataType::toInt( const CustomDataValue& operand)
{
	const DateDataValue* op = reinterpret_cast<const DateDataValue*>(&operand);
	types::Variant::Data::Int value = op->Date::operator-( Date( 1970, 1, 1));
	return types::Variant( value);
}

types::Variant DateDataType::toTimestamp( const CustomDataValue& operand)
{
	const DateDataValue* op = reinterpret_cast<const DateDataValue*>(&operand);
	types::DateTime dt( op->year(), op->month()-1, op->day()-1, 0, 0, 0);
	types::Variant::Data::Int value = dt.value();
	return types::Variant( value);
}


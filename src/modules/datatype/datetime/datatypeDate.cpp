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

types::Variant DateDataType::toUnixDate( const CustomDataValue& operand, const std::vector<types::Variant>& arg)
{
	const DateDataValue* op = reinterpret_cast<const DateDataValue*>(&operand);
	if (!arg.empty()) throw std::runtime_error("too many arguments");
	types::Variant::Data::Int value = op->Date::operator-( Date( 1970, 1, 1));
	return types::Variant( value);
}

types::Variant DateDataType::getYear( const CustomDataValue& operand, const std::vector<types::Variant>& arg)
{
	const DateDataValue* op = reinterpret_cast<const DateDataValue*>(&operand);
	if (!arg.empty()) throw std::runtime_error("too many arguments");
	types::Variant::Data::UInt rt = op->year();
	return rt;
}

types::Variant DateDataType::getMonth( const CustomDataValue& operand, const std::vector<types::Variant>& arg)
{
	const DateDataValue* op = reinterpret_cast<const DateDataValue*>(&operand);
	if (!arg.empty()) throw std::runtime_error("too many arguments");
	types::Variant::Data::UInt rt = op->month();
	return rt;
}

types::Variant DateDataType::getDay( const CustomDataValue& operand, const std::vector<types::Variant>& arg)
{
	const DateDataValue* op = reinterpret_cast<const DateDataValue*>(&operand);
	if (!arg.empty()) throw std::runtime_error("too many arguments");
	types::Variant::Data::UInt rt = op->day();
	return rt;
}

int DateDataValue::compare( const CustomDataValue& o) const
{
	if (o.type() != type())
	{
		return ((uintptr_t)type() > (uintptr_t)o.type())?1:-1;
	}
	else
	{
		const DateDataValue* odt = reinterpret_cast<const DateDataValue*>(&o);
		int rt = -1;
		rt += (int)(Date::operator>=(*odt));
		rt += (int)(Date::operator>(*odt));
		return rt;
	}
}

bool DateDataValue::getBaseTypeValue( Variant& dest) const
{
	types::DateTime dt( year(), month(), day());
	dest = dt;
	return true;
}

void DateDataValue::assign( const Variant& o)
{
	switch (o.type())
	{
		case Variant::Null:
		case Variant::Bool:
			throw std::runtime_error( std::string("cannot convert '") + o.typeName() + "' to big bcd integer");

		case Variant::Custom:
		{
			const CustomDataValue* ref = o.customref();
			if (ref->type() != type())
			{
				throw std::runtime_error( std::string("cannot convert '") + o.typeName() + "' to big bcd integer");
			}
			else
			{
				const DateDataValue* val = reinterpret_cast<const DateDataValue*>(ref);
				types::Date::operator=( *val);
			}
			break;
		}
		case Variant::Timestamp:

		case Variant::BigNumber:
		case Variant::Double:
		case Variant::Int:
			Date::operator=( Date(1970,1,1) + o.toint());
			break;

		case Variant::UInt:
			Date::operator=( Date(1970,1,1) + o.touint());
			break;

		case Variant::String:
		{
			const CustomDataInitializer* inibase = initializer();
			if (inibase)
			{
				const DateDataInitializer* ini = reinterpret_cast<const DateDataInitializer*>( inibase);
				Date::operator=( Date( o.tostring(), ini->format()));
			}
			else
			{
				Date::operator=( Date( o.tostring()));
			}
			break;
		}
	}
}




#include "serialize/struct/filtermapDescription.hpp"

using namespace _Wolframe;

namespace {
struct CustomerDescription :public serialize::StructDescription<Customer>
{
	CustomerDescription()
	{
		//
		(*this)
		("ID", &Customer::ID)
		--
		("name", &Customer::name)
		("canonical_Name", &Customer::canonical_Name)
		("country", &Customer::country)
		("locality", &Customer::locality)
		;
	}
};

const serialize::StructDescriptionBase* Customer::getStructDescription()
{
	static CustomerDescription rt;
	return &rt;
}


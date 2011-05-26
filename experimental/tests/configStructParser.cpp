///
/// \brief configuration lexem parser unit tests using google test framework (gTest)
///

#include "config/description.hpp"

using namespace _Wolframe;
using namespace config;


/// INTERFACE
struct Substruct
{
	std::string x;
	std::vector<float> y;

	static const config::DescriptionBase* description();
};

struct Struct
{
	int a;
	char b;
	Substruct substruct;
	std::vector<Substruct> bla;

	static const config::DescriptionBase* description();
};



/// IMPLEMENTATION
const config::DescriptionBase* Substruct::description()
{
	struct ThisDescription :public config::Description<Substruct>
	{
	ThisDescription()
	{
		(*this)
		( "x", &Substruct::x)
		( "y", &Substruct::y);
	}
	};
	static ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* Struct::description()
{
	struct ThisDescription :public config::Description<Struct>
	{
	ThisDescription()
	{
		(*this)
		( "a", &Struct::a)
		( "b", &Struct::b)
		( "bla", &Struct::bla)
		( "substruct", &Struct::substruct);
	}
	};
	static ThisDescription rt;
	return &rt;
}



int main( int, const char**)
{
	return 0;
}




int main( int argc, char **argv )
{
}


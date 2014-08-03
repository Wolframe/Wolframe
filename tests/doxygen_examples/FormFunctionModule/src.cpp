#include "appdevel/moduleFrameMacros.hpp"
#include "appdevel/cppFormFunctionModuleMacros.hpp"
#include "serialize/struct/structDescription.hpp"

struct MyInput
{
	std::string data;
	int id;
	// ... your data structures are here

	static const _Wolframe::serialize::StructDescriptionBase *getStructDescription();
};

class MyInputDescription :public _Wolframe::serialize::StructDescription<MyInput>
{
public:
	MyInputDescription()
	{
		// here you define introspection for your data:
		(*this)
			("data", &MyInput::data)
			("id", &MyInput::id)
		;
	}
};

const _Wolframe::serialize::StructDescriptionBase* MyInput::getStructDescription()
{
	static MyInputDescription rt;
	return &rt;
}

struct MyOutput
{
	// ... your data structures are here as in MyInput

	static const _Wolframe::serialize::StructDescriptionBase *getStructDescription()
	{
		// ... your data structure description is here as for MyInput
		return 0;
	}
};


int myFunction( _Wolframe::proc::ExecContext* /*ctx*/, MyOutput& /*res*/, const MyInput& /*param*/)
{
	// ... your function implementation is here
	// ... it returns 0 on success or else an error code
	return 0;
}

WF_MODULE_BEGIN( "MyFunctions", "my functions short description")
 WF_FORM_FUNCTION( "myfunc", myFunction, MyOutput, MyInput)
WF_MODULE_END

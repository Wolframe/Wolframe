/// \file docs/cpp/toc.hpp
/// \brief File used by doxygen to generate the source documentation main page

/*! \mainpage Wolframe, a modular, 3-tier application server written in C++
 *
 * \section FilterModule Writing a content filter module for another format than XML or JSON
 *
 * \code

#include "appdevel/moduleFrameMacros.hpp"
#include "appdevel/filterModuleMacros.hpp"
#include "filter/filter.hpp"

static _Wolframe::langbind::FilterType* createMyFilterType()
{
	// ... here we create our own filter type object
}

WF_MODULE_BEGIN( "MyFilter", "my content filter")
 WF_FILTER_TYPE( "myfilter", MyFilterType::create)
WF_MODULE_END

 * \endcode
 *
 * \section FormFunctionModule Writing a module for a form function (struct in, struct out) as function, that is callable by Wolframe and any language binding in your application
 *
 * \code

#include "appdevel/moduleFrameMacros.hpp"
#include "appdevel/cppFormFunctionModuleMacros.hpp"

struct MyInput
{
	std::string data;
	int id;
	// ... your data structures are here

	static const _Wolframe::serialize::StructDescriptionBase *getStructDescription()
};

class MyInputDescription :public serialize::StructDescription<MyInput>
{
	MyInputDescription()
	{
		// here you define introspection for your data:
		(*this)
			("data", &MyInput::data)
			("id", &MyInput::id)
		;
	}
}

static const _Wolframe::serialize::StructDescriptionBase* MyInput::getStructDescription()
{
	static MyInputDescription rt;
	return &rt;
}

struct MyOutput
{
	// ... your data structures are here as in MyInput

	static const serialize::StructDescriptionBase *getStructDescription()
	{
		// ... your data structure description is here as for MyInput
	}
};


static int myFunction( proc::ExecContext* ctx, MyOutput& res, const MyInput& param)
{
	// ... your function implementation is here
}

WF_MODULE_BEGIN( "MyFunctions", "my functions short description")
 WF_FORM_FUNCTION( "myfunc", myFunction, MyOutput, MyInput)
WF_MODULE_END

* \endcode
*
*/


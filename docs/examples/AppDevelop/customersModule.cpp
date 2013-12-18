
#include "appDevel.hpp"
// ... PUT THE INCLUDES FOR THE "ProcessCustomer" FUNCTION DECLARATION HERE !

CPP_APPLICATION_FORM_FUNCTION_MODULE("process customer function")

using namespace _Wolframe;

static module::createBuilderFunc objdef[] =
{
    appdevel::CppFormFunction<ProcessCustomer>::constructor
};

CPP_APPLICATION_FORM_FUNCTION_MODULE_END(1,objdef)



#include "appDevel.hpp"
// ... PUT THE INCLUDES FOR THE "ProcessCustomer" FUNCTION DECLARATION HERE !

#include "customersFunction.hpp"

using namespace _Wolframe;

CPP_APPLICATION_FORM_FUNCTION_MODULE("process customer function")
CPP_APPLICATION_FORM_FUNCTION("process_customer",ProcessCustomer::exec,Customer,Customer)
CPP_APPLICATION_FORM_FUNCTION_MODULE_END



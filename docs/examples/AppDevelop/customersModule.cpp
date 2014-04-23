
#include "appDevel.hpp"
// ... PUT THE INCLUDES FOR THE "ProcessCustomer" FUNCTION DECLARATION HERE !

#include "customersFunction.hpp"

using namespace _Wolframe;

WF_MODULE_BEGIN( "ProcessCustomerFunction", "process customer function")
WF_FORM_FUNCTION("process_customer",ProcessCustomer::exec,Customer,Customer)
WF_MODULE_END


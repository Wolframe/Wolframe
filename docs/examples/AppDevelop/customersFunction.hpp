
// ... PUT THE INCLUDES FOR THE "Customer" STRUCTURE DECLARATION HERE !

struct ProcessCustomer
{
    typedef Customer InputType; 
    typedef Customer OutputType; 
    static const char* name() {return "process_customer";}

    static int exec( const proc::ProcessorProvider* provider, InputType& res, const OutputType& param);
};


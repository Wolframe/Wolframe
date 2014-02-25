#include "appDevel.hpp"

using namespace _Wolframe;

class NormalizeInt
    :public types::NormalizeFunction
{
public:
    NormalizeInt(){}
    virtual ~NormalizeInt(){}
    virtual const char* name() const
            {return "int";}
    virtual types::Variant execute( const types::Variant& i) const
            {return types::Variant( i.toint());}

    static types::NormalizeFunction* create(
            types::NormalizeResourceHandle*, const std::string&)
    {
        return new NormalizeInt();
    }
};

class NormalizeFloat
    :public types::NormalizeFunction
{
public:
    NormalizeFloat(){}
    virtual ~NormalizeFloat(){}
    virtual const char* name() const
            {return "float";}
    virtual types::Variant execute( const types::Variant& i) const
            {return types::Variant( i.todouble());}

    static types::NormalizeFunction* create(
            types::NormalizeResourceHandle*, const std::string&)
    {
        return new NormalizeFloat();
    }
};

NORMALIZER_MODULE(
    "example1",
    "normalizer module without resources")

NORMALIZER_FUNCTION( "int", NormalizeInt::create)
NORMALIZER_FUNCTION( "float", NormalizeFloat::create)

NORMALIZER_MODULE_END



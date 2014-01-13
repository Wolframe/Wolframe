#include "appDevel.hpp"

using namespace _Wolframe;

struct NormalizeInt
    :public types::NormalizeFunction
{
    NormalizeInt(){}
    virtual ~NormalizeInt(){}
    virtual const char* name() const
            {return "int";}
    virtual types::Variant execute( const types::Variant& i) const
            {return types::Variant( i.toint());}
};

struct NormalizeFloat
    :public types::NormalizeFunction
{
    NormalizeFloat(){}
    virtual ~NormalizeFloat(){}
    virtual const char* name() const
            {return "float";}
    virtual types::Variant execute( const types::Variant& i) const
            {return types::Variant( i.todouble());}
};

struct Functions
{
    static types::NormalizeFunction* createInt(
            langbind::ResourceHandle&,
            const std::string&)
        {
            return new NormalizeInt();
        }

    static types::NormalizeFunction* createFloat(
            langbind::ResourceHandle&,
            const std::string&)
    {
        return new NormalizeFloat();
    }
};

NORMALIZER_MODULE(
    "example1",
    "normalizer module without resources")

NORMALIZER_FUNCTION( "int", Functions::createInt)
NORMALIZER_FUNCTION( "float", Functions::createFloat)

NORMALIZER_MODULE_END



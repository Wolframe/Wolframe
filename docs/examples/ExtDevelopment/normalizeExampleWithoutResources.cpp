#include "appDevel.hpp"

using namespace _Wolframe;

class NormalizeInt
    :public types::NormalizeFunction
{
public:
    NormalizeInt( const std::vector<types::Variant>&){}
    virtual ~NormalizeInt(){}
    virtual const char* name() const
            {return "int";}
    virtual types::Variant execute( const types::Variant& i) const
            {return types::Variant( i.toint());}
    virtual types::NormalizeFunction* copy() const
            {return new NormalizeInt(*this);}
};

class NormalizeFloat
    :public types::NormalizeFunction
{
public:
    NormalizeFloat( const std::vector<types::Variant>&){}
    virtual ~NormalizeFloat(){}
    virtual const char* name() const
            {return "float";}
    virtual types::Variant execute( const types::Variant& i) const
            {return types::Variant( i.todouble());}
    virtual types::NormalizeFunction* copy() const
            {return new NormalizeFloat(*this);}
};

WF_MODULE_BEGIN(
    "example1",
    "normalizer module without resources")

    WF_NORMALIZER( "int", NormalizeInt)
    WF_NORMALIZER( "float", NormalizeFloat)

WF_MODULE_END



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
	virtual types::NormalizeFunction* copy() const {return new NormalizeInt(*this);}

    static types::NormalizeFunction* create(
            types::NormalizeResourceHandle*, const std::vector<types::Variant>&)
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
	virtual types::NormalizeFunction* copy() const {return new NormalizeFloat(*this);}

    static types::NormalizeFunction* create(
            types::NormalizeResourceHandle*, const std::vector<types::Variant>&)
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



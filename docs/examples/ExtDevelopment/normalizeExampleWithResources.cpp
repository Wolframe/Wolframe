#include "appDevel.hpp"

using namespace _Wolframe;

class ConversionResources
    :public types::NormalizeResourceHandle
{
public:
    ConversionResources()
    {}
    virtual ~ConversionResources()
    {}
};

class NormalizeInt
        :public types::NormalizeFunction
{
public:
    explicit NormalizeInt( const ConversionResources* res_)
        :res(res_){}
    virtual ~NormalizeInt()
        {}
    virtual const char* name() const
        {return "int";}
    virtual types::Variant execute( const types::Variant& i) const
        {return types::Variant( i.toint());}
    virtual types::NormalizeFunction* copy() const
        {return new NormalizeInt(*this);}

    static types::NormalizeFunction* create(
        types::NormalizeResourceHandle* reshnd,
        const std::vector<types::Variant>&)
    {
        ConversionResources* res
            = dynamic_cast<ConversionResources*>(reshnd);
        return new NormalizeInt( res);
    }
private:
    const ConversionResources* res;
};

WF_MODULE_BEGIN(
    "example2",
    "normalizer module with resources")

    WF_NORMALIZER_RESOURCE( ConversionResources)
    WF_NORMALIZER_WITH_RESOURCE(
        "Int", NormalizeInt::create, ConversionResources)
WF_MODULE_END




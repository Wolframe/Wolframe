#include "appDevel.hpp"

using namespace _Wolframe;

class ConversionResources
    :public langbind::ResourceHandle
{
public:
    ConversionResources()
    {}
    virtual ~ConversionResources()
    {}
};

struct Functions
{
    struct NormalizeInt
        :public types::NormalizeFunction
    {
        NormalizeInt( const ConversionResources* res_)
                :res(res_){}
        virtual ~NormalizeInt()
                {}
        virtual const char* name() const
                {return "int";}
        virtual types::Variant execute( const types::Variant& i) const
                {return types::Variant( i.toint());}
    private:
        const ConversionResources* res;
    };

    static types::NormalizeFunction* createInt(
            langbind::ResourceHandle& reshnd,
            const std::string&)
    {
        ConversionResources& res
            = dynamic_cast<ConversionResources&>(reshnd);
        return new NormalizeInt( &res);
    }
};

NORMALIZER_MODULE_WITH_RESOURCE( 
    "example2",
    "normalizer module with resources",
    ConversionResources)

NORMALIZER_FUNCTION( "Int", Functions::createInt)

NORMALIZER_MODULE_END



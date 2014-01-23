namespace _Wolframe {
namespace types {

struct NormalizeFunction
{
    virtual ~NormalizeFunction(){}
    virtual const char* name() const=0;
    virtual Variant execute( const Variant& i) const=0;
};
}}

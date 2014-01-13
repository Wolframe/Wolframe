namespace _Wolframe {
namespace langbind {

struct NormalizeFunction
{
    virtual ~NormalizeFunction(){}
    virtual const char* name() const=0;
    virtual Variant execute( const Variant& i) const=0;
};
}}

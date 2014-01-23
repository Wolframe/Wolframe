namespace _Wolframe {
namespace types {

struct VariantConst :public Variant
{
    //Null constructor:
    VariantConst();
    //Copy constructors:
    VariantConst( const Variant& o);
    VariantConst( const VariantConst& o);
    VariantConst( bool o);
    VariantConst( double o);
    VariantConst( float o);
    VariantConst( int o);
    VariantConst( unsigned int o);
    VariantConst( Data::Int o);
    VariantConst( Data::UInt o);
    VariantConst( const char* o);
    VariantConst( const char* o, std::size_t n);
    VariantConst( const std::string& o);

    //Assignment operators:
    VariantConst& operator=( const Variant& o);
    VariantConst& operator=( const VariantConst& o);
    VariantConst& operator=( bool o);
    VariantConst& operator=( double o);
    VariantConst& operator=( float o);
    VariantConst& operator=( int o);
    VariantConst& operator=( unsigned int o);
    VariantConst& operator=( Data::Int o);
    VariantConst& operator=( Data::UInt o);
    VariantConst& operator=( const char* o);
    VariantConst& operator=( const std::string& o);
};
}} //namespace



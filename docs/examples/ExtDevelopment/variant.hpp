namespace _Wolframe {
namespace types {

class Variant
{
public:
    //Different value types a variant can have:
    enum Type
    {
        Null,     // undefined
        Int,      // 64 bit signed integer value
        UInt,     // 64 bit unsigned integer value
        Bool,     // boolean value
        Double,   // double presicion floating point value
        String    // string encoded as UTF-8
    };
    //Current type enum or type name of this:
    Type type() const;
    const char* typeName() const;

    //Null constructor:
    Variant();

    //Copy constructors:
    Variant( bool o);
    Variant( double o);
    Variant( float o);
    Variant( int o);
    Variant( unsigned int o);
    Variant( Data::Int o);
    Variant( Data::UInt o);
    Variant( const char* o);
    Variant( const char* o, std::size_t n);
    Variant( const std::string& o);
    Variant( const Variant& o);

    //Assignment operators:
    Variant& operator=( const Variant& o);
    Variant& operator=( bool o);
    Variant& operator=( double o);
    Variant& operator=( float o);
    Variant& operator=( int o);
    Variant& operator=( unsigned int o);
    Variant& operator=( Data::Int o);
    Variant& operator=( Data::UInt o);
    Variant& operator=( const char* o);
    Variant& operator=( const std::string& o);

    //Initializer as constant (borrowed value reference):
    void initConstant( const char* o, std::size_t l);
    void initConstant( const std::string& o);
    void initConstant( const char* o);

    //Comparison operators:
    bool operator==( const Variant& o) const;
    bool operator!=( const Variant& o) const;
    bool operator>( const Variant& o) const;
    bool operator>=( const Variant& o) const;
    bool operator<=( const Variant& o) const;
    bool operator<( const Variant& o) const;

    //Getter functions with value conversion if needed:
    std::string tostring() const;
    double tonumber() const;
    double todouble() const;
    bool tobool() const;
    Data::Int toint() const;
    Data::UInt touint() const;

    //Base pointer in case of a string (throws if not string):
    char* charptr() const;
    //Size in case of a string (throws if not string):
    std::size_t charsize() const;

    //Evaluate if defined (not Null):
    bool defined() const;
    //Reset to Null:
    void clear();

    //Convert type:
    void convert( Type type_);
    //Move assignment from value o (o gets Null):
    void move( Variant& o);
};

}} //namespace



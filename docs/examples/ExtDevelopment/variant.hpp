namespace _Wolframe {
namespace types {

class Variant
{
public:
    //Different value types a variant can have:
    enum Type
    {
       Custom,      //< data type defined by a custom data type module
       Timestamp,   //< date and time value with a precision down to microseconds
       BigNumber,   //< big BCD fixed point number in the range of 1E-32767 to 1E+32768
       Double,      //< IEEE 754 double precision floating point number
       Int,         //< 64 bit signed integer value
       UInt,        //< 64 bit unsigned integer value
       Bool,        //< boolean value
       String       //< 0-terminated UTF-8 string
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
    Variant( const types::CustomDataType* typ,
             const types::CustomDataInitializer* dsc=0);
    Variant( const types::CustomDataValue& o);
    Variant( const types::DateTime& o);
    Variant( const types::BigNumber& o);

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
    Variant& operator=( const types::CustomDataValue& o);
    Variant& operator=( const char* o);
    Variant& operator=( const types::DateTime& o);
    Variant& operator=( const types::BigNumber& o);

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
    std::wstring towstring() const;
    double todouble() const;
    bool tobool() const;
    Data::Int toint() const;
    Data::UInt touint() const;
    Data::Timestamp totimestamp() const;

    //Base pointer in case of a string (throws if not string):
    char* charptr() const;
    //Size in case of a string (throws if not string):
    std::size_t charsize() const;
    ///\brief Get the pointer to the custom data object (throws for non custom data type)
    const CustomDataValue* customref() const;
    ///\brief Get the pointer to the custom data object (throws for non custom data type)
    CustomDataValue* customref();
    ///\brief Get the pointer to the big number object (throws for non big number data type)
    const types::BigNumber* bignumref() const;
    ///\brief Get the pointer to the big number object (throws for non big number data type)
    types::BigNumber* bignumref();

    ///\brief Getter with value conversion
    Data::Timestamp totimestamp() const;

    ///\brief Test if this value is atomic (not a structure or an indirection)
    bool atomic() const;
    //Evaluate if defined (not Null):
    bool defined() const;
    //Reset to Null:
    void clear();

    //Convert type:
    void convert( Type type_);
    //Move assignment from value o (o gets Null):
    void move( Variant& o);
    ///\brief Assigning o to this including a conversion to a defined type
    void assign( Type type_, const Variant& o);
};

}} //namespace



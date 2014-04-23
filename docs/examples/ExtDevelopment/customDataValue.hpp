
namespace _Wolframe {
namespace types {

// Custom data value interface
class CustomDataValue
{
public:
    CustomDataValue();
    CustomDataValue( const CustomDataValue& o);
    virtual ~CustomDataValue();

    const CustomDataType* type() const;
    const CustomDataInitializer* initializer() const;

    virtual int compare( const CustomDataValue& o) const=0;
    virtual std::string tostring() const=0;
    virtual void assign( const Variant& o)=0;
    virtual CustomDataValue* copy() const=0;

    // try to convert the value to one of the basic
    // variant types and return true on success:
    virtual bool getBaseTypeValue( Variant&) const;
};

}}//namespace


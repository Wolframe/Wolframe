
namespace _Wolframe {
namespace types {

// Custom Data Type Definition
class CustomDataType
{
public:
    typedef unsigned int ID;
    enum UnaryOperatorType {Increment,Decrement,Negation};
    enum BinaryOperatorType {Add,Subtract,Multiply,Divide,Power,Concat};
    enum ConversionOperatorType {ToString,ToInt,ToUInt,ToDouble,ToTimestamp};
    enum DimensionOperatorType {Length};

    typedef types::Variant (*ConversionOperator)(
        const CustomDataValue& operand);
    typedef types::Variant (*UnaryOperator)(
        const CustomDataValue& operand);
    typedef types::Variant (*BinaryOperator)(
        const CustomDataValue& operand, const Variant& arg);
    typedef std::size_t (*DimensionOperator)(
        const CustomDataValue& arg);
    typedef types::Variant (*CustomDataValueMethod)(
        const CustomDataValue& val,
        const std::vector<types::Variant>& arg);
    typedef CustomDataValue* (*CustomDataValueConstructor)(
        const CustomDataInitializer* initializer);
    typedef CustomDataInitializer* (*CreateCustomDataInitializer)(
        const std::vector<types::Variant>& arg);

public:
    CustomDataType()
        :m_id(0)
    {
        std::memset( &m_vmt, 0, sizeof( m_vmt));
    }

    CustomDataType( const std::string& name_,
            CustomDataValueConstructor constructor_,
            CreateCustomDataInitializer initializerconstructor_=0);

    void define( UnaryOperatorType type, UnaryOperator op);
    void define( BinaryOperatorType type, BinaryOperator op);
    void define( ConversionOperatorType type, ConversionOperator op);
    void define( DimensionOperatorType type, DimensionOperator op);
    void define( const char* methodname, CustomDataValueMethod method);
};

typedef CustomDataType* (*CreateCustomDataType)( const std::string& name);

}}//namespace


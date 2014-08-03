
namespace _Wolframe {
namespace langbind {

typedef std::pair<std::string,std::string> FilterArgument;

class FilterType
{
public:
    virtual ~FilterType(){}
    virtual Filter* create( const std::vector<FilterArgument>& arg) const=0;
};

}}//namespace



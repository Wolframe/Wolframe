
namespace _Wolframe {
namespace langbind {

class Filter
{
public:
    Filter( const InputFilterR& i_, const OutputFilterR& o_)
        :m_inputfilter(i_),m_outputfilter(o_)
    {
        m_outputfilter->setAttributes( m_inputfilter.get());
    }
};

typedef std::pair<std::string,std::string> FilterArgument;

class FilterType
{
public:
    virtual ~FilterType(){}
    virtual Filter* create( const std::vector<FilterArgument>& arg) const=0;
};

}}//namespace



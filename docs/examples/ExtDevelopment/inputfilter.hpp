
namespace _Wolframe {
namespace langbind {

// Input filter interface
class InputFilter
{
public:
    // State of the input filter
    enum State
    {
        Open,         // normal input processing
        EndOfMessage, // end of message reached (yield)
        Error         // an error occurred
    };

    // Default constructor
    explicit InputFilter( const char* name_);

    // Copy constructor
    ///\param[in] o input filter to copy
    InputFilter( const InputFilter& o);

    // Destructor
    virtual ~InputFilter();

    // Get a self copy
    virtual InputFilter* copy() const=0;

    // Get an instance copy of this in its initial state
    virtual InputFilter* initcopy() const=0;

    // Declare the next input chunk to the filter
    virtual void putInput(
        const void* ptr,
        std::size_t size, bool end)=0;

    // Get the rest of the input chunk left
    // unparsed yet (defaults to nothing left)
    virtual void getRest(
        const void*& ptr,
        std::size_t& size, bool& end);

    // Get a named member value of the filter
    virtual bool getValue(
        const char* id, std::string& val) const;

    // Get next element
    virtual bool getNext(
        ElementType& type,
        const void*& element, std::size_t& elementsize)=0;

    // Get type of the document
    virtual bool getDocType( types::DocType& doctype);

    // Evaluate if the document metadata are avalable
    // and set state for fetching them if needed
    virtual bool getMetadata();

    // Get the current state
    State state() const;

    // Set input filter state with error message
    void setState( State s, const char* msg=0);
};

// Shared input filter reference
typedef types::CountedReference<InputFilter> InputFilterR;


}}//namespace
#endif


#include "appdevel/doctypeModuleMacros.hpp"
#include "appdevel/moduleFrameMacros.hpp"

class DoctypeDetectorMyFormat
	:public _Wolframe::cmdbind::DoctypeDetector
{
public:
	DoctypeDetectorMyFormat(){}

	virtual void putInput( const char* chunk, std::size_t chunksize)
	{
		// ... feed your detector with a new input chunk here
	}

	virtual bool run()
	{
		// ... try to detect your document format in the input
		// ... return false and set the lastError in case of error
		// ... return false without setting lastError if you need more input chunks
		// ... return true, if you can decide wheter the document format is yours or not
	}

	virtual const char* lastError() const
	{
		// ... return the last error occurred here
	}

	virtual const _Wolframe::types::DoctypeInfoR& info() const
	{
		// ... return the document type infor structure here, if the document format is yours
	}
};

WF_MODULE_BEGIN( "myDocformat", "document type/format detection for MyFormat")
 WF_DOCUMENT_FORMAT( "MYFM", DoctypeDetectorMyFormat)
WF_MODULE_END

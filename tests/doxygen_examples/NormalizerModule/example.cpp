#include "appdevel/normalizeModuleMacros.hpp"
#include "appdevel/moduleFrameMacros.hpp"

class MyNormalizeFunction
	:public _Wolframe::types::NormalizeFunction
{
public:
	MyNormalizeFunction( const std::vector<_Wolframe::types::Variant>& arg)
	{
		// ... the WF_NORMALIZER* template needs a constructor with this signature
	}

	virtual const char* name() const
	{
		// ... return the identifier of the function here
	}
	virtual _Wolframe::types::Variant execute( const _Wolframe::types::Variant& i) const
	{
		// ... execure this function on the input variant here
	}
	virtual _Wolframe::types::NormalizeFunction* copy() const
	{
		// ... return a copy of this here
	}
};

WF_MODULE_BEGIN( "My normalizer", "my normalizer module")
 WF_NORMALIZER( "mynormalize",  MyNormalizeFunction)
WF_MODULE_END

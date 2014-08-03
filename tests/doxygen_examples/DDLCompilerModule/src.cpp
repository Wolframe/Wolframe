#include "appdevel/ddlCompilerModuleMacros.hpp"
#include "appdevel/moduleFrameMacros.hpp"
#include "langbind/ddlCompilerInterface.hpp"

class MyDDLCompiler
	:public _Wolframe::langbind::DDLCompiler
{
public:
	MyDDLCompiler()
		:_Wolframe::langbind::DDLCompiler( "myddl", "mdl") {}

	virtual std::vector<_Wolframe::types::FormDescriptionR> compile( const std::string& /*filename*/, const _Wolframe::types::NormalizeFunctionMap* /*typemap*/) const
	{
		// ... compile the source file filename and return the resulting form definitions here
		return std::vector<_Wolframe::types::FormDescriptionR>();
	}
};

WF_MODULE_BEGIN( "MyDDLCompiler", "compiler for my language describing data forms")
 WF_DDLCOMPILER( "MyDDL", MyDDLCompiler)
WF_MODULE_END

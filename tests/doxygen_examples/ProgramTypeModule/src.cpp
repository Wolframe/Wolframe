#include "appdevel/programTypeModuleMacros.hpp"
#include "appdevel/moduleFrameMacros.hpp"

class MyProgram
	:public _Wolframe::prgbind::Program
{
public:
	MyProgram()
		:_Wolframe::prgbind::Program( SuperFunction){}

	virtual bool is_mine( const std::string& /*filename*/) const
	{
		// ... return true, if the file filename is of my type here
		return false;
	}

	virtual void loadProgram( _Wolframe::prgbind::ProgramLibrary& /*library*/, _Wolframe::db::Database* /*transactionDB*/, const std::string& /*filename*/)
	{
		// ... load the program in the file filename and store its declared items in the program library or in the database
	}
};

WF_MODULE_BEGIN( "MyProgramTypeModule", "my program type module")
 WF_PROGRAM_TYPE( "MyProgram", MyProgram)
WF_MODULE_END


#include "pythonInterpreterInstance.hpp"
#include "logger-v1.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;
using namespace _Wolframe::langbind::python;

python::InterpreterInstance::InterpreterInstance( )
{
}

StructureR python::InterpreterInstance::call( const proc::ProcessorProviderInterface*, const StructureR& arg)
{
	return arg;
}

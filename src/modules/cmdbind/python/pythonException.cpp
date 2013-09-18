#include "pythonException.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::langbind;
using namespace _Wolframe::langbind::python;

void python::THROW_ON_ERROR( const char* usrmsg)
{
	PyObject* err = PyErr_Occurred();
	if (err)
	{
		if (PyErr_GivenExceptionMatches( err, PyExc_MemoryError))
		{
			throw std::bad_alloc();
		}
		else
		{
			throw std::runtime_error( usrmsg);
		}
	}
}

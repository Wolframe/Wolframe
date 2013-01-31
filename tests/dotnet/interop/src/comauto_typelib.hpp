#ifndef _Wolframe_COM_AUTOMATION_TYPELIB_HPP_INCLUDED
#define _Wolframe_COM_AUTOMATION_TYPELIB_HPP_INCLUDED
#include "comauto_utils.hpp"
#include "comauto_function.hpp"
#include <oaidl.h>
#include <comdef.h>
#include <atlbase.h>
#include <atlcom.h>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace comauto {

class TypeLib
{
public:
	explicit TypeLib( const std::string& file);
	virtual ~TypeLib();
	void print( std::ostream& out) const;

	std::vector<comauto::FunctionR> comauto::TypeLib::loadFunctions( comauto::CommonLanguageRuntime* clr, const std::string& assemblyname);

private:
	ITypeLib* m_typelib;
};

}} //namespace
#endif


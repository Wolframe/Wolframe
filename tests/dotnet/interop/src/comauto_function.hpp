#ifndef _Wolframe_COM_AUTOMATION_FUNCTION_HPP_INCLUDED
#define _Wolframe_COM_AUTOMATION_FUNCTION_HPP_INCLUDED
#include "comauto_utils.hpp"
#include <oaidl.h>
#include <comdef.h>
#include <atlbase.h>
#include <atlcom.h>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>

namespace comauto {

class Function
{
public:
	Function( ITypeInfo* typeinfo_, const std::string& classname_, unsigned short fidx);
	~Function();

	const std::string& classname() const	{return m_classname;}
	const std::string& methodname() const	{return m_methodname;}

	void print( std::ostream& out);

private:
	ITypeInfo* m_typeinfo;
	FUNCDESC* m_funcdesc;
	std::string m_classname;
	std::string m_methodname;
	typedef std::pair< std::string, TYPEDESC*> Parameter;
	std::vector<Parameter> m_parameterlist;
	TYPEDESC* m_returntype;
};

typedef boost::shared_ptr<Function> FunctionR;

} //namespace
#endif


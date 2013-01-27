#ifndef _Wolframe_COM_AUTOMATION_FUNCTION_HPP_INCLUDED
#define _Wolframe_COM_AUTOMATION_FUNCTION_HPP_INCLUDED
#include "comauto_utils.hpp"
#include "filter/typedfilter.hpp"
#include "langbind/formFunction.hpp"
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

class Function;
class CommonLanguageRuntime;

class FunctionResult :public langbind::TypedInputFilter
{
public:
	FunctionResult()
	{
		m_data.vt = VT_EMPTY;
	}
	explicit FunctionResult( const VARIANT& v)
		:m_data(v){}

	virtual ~FunctionResult(){}
	virtual bool getNext( ElementType& type, Element& element);

	virtual TypedInputFilter* copy() const
	{
		return new FunctionResult( m_data);
	}

	VARIANT* data() const	{return &m_data;}

private:
	VARIANT m_data;
};

class FunctionClosure :public langbind::FormFunctionClosure
{
public:
	FunctionClosure( const Function* func_)
		:m_func(func_)
		,m_flags(serialize::Context::None)
		,m_provider(0)
		,m_parameter(0)
		,m_resultdata(0){}

	virtual ~FunctionClosure();
	virtual bool call();

	virtual void init( const proc::ProcessorProvider* p, const langbind::TypedInputFilterR& i, serialize::Context::Flags f=serialize::Context::None);

	virtual langbind::TypedInputFilterR result() const	{return m_result;}

private:
	const Function* m_func;
	langbind::TypedInputFilterR m_input;
	serialize::Context::Flags m_flags;
	const proc::ProcessorProvider* m_provider;
	VARIANT* m_parameter;
	VARIANT* m_resultdata;
	langbind::TypedInputFilterR m_result;
};


class Function
	:public langbind::FormFunction
{
public:
	Function( comauto::CommonLanguageRuntime* clr_, ITypeInfo* typeinfo_, const std::string& assemblyname_, const std::string& classname_, unsigned short fidx);
	virtual ~Function();

	const std::string& assemblyname() const								{return m_assemblyname;}
	const std::string& classname() const								{return m_classname;}
	const std::string& methodname() const								{return m_methodname;}
	std::size_t nofParameter() const									{return m_parameterlist.size();}
	std::size_t getParameterIndex( const std::string& name) const;
	const std::string& getParameterName( std::size_t idx) const;
	TYPEDESC* getParameter( std::size_t idx) const;
	comauto::CommonLanguageRuntime* clr() const							{return m_clr;}

	void print( std::ostream& out);
	virtual langbind::FormFunctionClosure* createClosure() const	{return new FunctionClosure(this);}

private:
	comauto::CommonLanguageRuntime* m_clr;
	ITypeInfo* m_typeinfo;
	FUNCDESC* m_funcdesc;
	std::string m_assemblyname;
	std::string m_classname;
	std::string m_methodname;
	typedef std::pair< std::string, TYPEDESC*> Parameter;
	std::vector<Parameter> m_parameterlist;
	TYPEDESC* m_returntype;
};

typedef boost::shared_ptr<Function> FunctionR;

}} //namespace
#endif


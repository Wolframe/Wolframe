#ifndef _Wolframe_COM_AUTOMATION_FUNCTION_HPP_INCLUDED
#define _Wolframe_COM_AUTOMATION_FUNCTION_HPP_INCLUDED
#include "comauto_utils.hpp"
#include "comauto_recordinfo.hpp"
#include "comauto_typelib.hpp"
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


class FunctionClosure
	:public langbind::FormFunctionClosure
{
public:
	FunctionClosure( const Function* func_);

	virtual ~FunctionClosure();
	virtual bool call();

	virtual void init( const proc::ProcessorProvider* p, const langbind::TypedInputFilterR& i, serialize::Context::Flags f=serialize::Context::None);

	virtual langbind::TypedInputFilterR result() const;

private:
	const proc::ProcessorProvider* m_provider;						//< processor provider reference for function called
	const Function* m_func;											//< function to call
	langbind::TypedInputFilterR m_input;							//< input parameters
	serialize::Context::Flags m_flags;								//< flag passed by called to stear validation strictness
	VARIANT* m_param;												//< array of function parameters to initialize
	enum {null_paramidx=0xFFFF};
	std::size_t m_paramidx;											//< currently selected parameter of the function [0,1,.. n-1]
	TypeLib::AssignmentClosureR m_paramclosure;						//< closure for current parameter assignment
	std::map<std::size_t,std::vector<VARIANT> > m_arrayparam;		//< temporary buffer for parameters passed as array (with their name)
	langbind::TypedInputFilterR m_result;							//< reference to result of the function call
};


class Function
	:public langbind::FormFunction
{
public:
	struct Parameter
	{
		enum AddrMode {Value,Safearray};

		Parameter( const Parameter& o);
		Parameter( const std::string& name_, const TYPEDESC* typedesc_, const ITypeInfo* typeinfo_, AddrMode addrMode_);
		~Parameter();

		std::string name;
		AddrMode addrMode;
		const TYPEDESC* typedesc;
		const ITypeInfo* typeinfo;
	};

	struct ReturnType
	{
		ReturnType( const ReturnType& o);
		ReturnType( const TYPEDESC* typedesc_, const ITypeInfo* typeinfo_);
		explicit ReturnType( const TYPEDESC* typedesc_=0);
		~ReturnType();

		const TYPEDESC* typedesc;
		const ITypeInfo* typeinfo;
	};

public:
	Function( comauto::CommonLanguageRuntime* clr_, const comauto::TypeLib* typelib_, const ITypeInfo* typeinfo_, const std::string& assemblyname_, const std::string& classname_, unsigned short fidx);
	virtual ~Function();

	const std::string& assemblyname() const								{return m_assemblyname;}
	const std::string& classname() const								{return m_classname;}
	const std::string& methodname() const								{return m_methodname;}
	std::size_t nofParameter() const									{return m_parameterlist.size();}
	std::size_t getParameterIndex( const std::string& name) const;
	const Parameter* getParameter( std::size_t idx) const;
	comauto::CommonLanguageRuntime* clr() const							{return m_clr;}
	const comauto::TypeLib* typelib() const								{return m_typelib;}
	const ITypeInfo* typeinfo() const									{return m_typeinfo;}
	const ReturnType* getReturnType() const								{return &m_returntype;}

	void print( std::ostream& out) const;
	virtual langbind::FormFunctionClosure* createClosure() const		{return new FunctionClosure(this);}

private:
	comauto::CommonLanguageRuntime* m_clr;
	const comauto::TypeLib* m_typelib;
	const ITypeInfo* m_typeinfo;
	FUNCDESC* m_funcdesc;
	std::string m_assemblyname;
	std::string m_classname;
	std::string m_methodname;
	std::vector<Parameter> m_parameterlist;
	ReturnType m_returntype;
};

typedef boost::shared_ptr<Function> FunctionR;

std::vector<comauto::FunctionR> loadFunctions( const comauto::TypeLib* typelib, comauto::CommonLanguageRuntime* clr, const std::string& assemblyname);

}} //namespace
#endif


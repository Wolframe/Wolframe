#ifndef _Wolframe_COM_AUTOMATION_FUNCTION_HPP_INCLUDED
#define _Wolframe_COM_AUTOMATION_FUNCTION_HPP_INCLUDED
#include "comauto_utils.hpp"
#include "comauto_recordinfo.hpp"
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
	VARIANT* initParameter( std::size_t eidx, VARIANT* val, const IRecordInfo* recinfo) const;
	std::string errorMessage( const std::string& msg) const;
	void assignValue( VARIANT* dstrec, std::size_t ofs, VARTYPE dsttype, const langbind::TypedFilterBase::Element& elem);

private:
	enum State
	{
		CheckParam,
		MapParam,
		MapAttrParam
	};

	///\class StateStackElem
	///\brief Element of stack for parameter initialization statemachine
	struct StateStackElem
	{
		State m_state;
		const RecordInfo* m_recinfo;
		int m_variableidx;
		std::size_t m_ofs;

		StateStackElem( State state_, const RecordInfo* recinfo_, std::size_t ofs_=0, int variableidx_=-1)
			:m_state(state_)
			,m_recinfo(recinfo_)
			,m_variableidx(variableidx_)
			,m_ofs(ofs_){}

		StateStackElem( const StateStackElem& o)
			:m_state(o.m_state)
			,m_recinfo(o.m_recinfo)
			,m_variableidx(o.m_variableidx)
			,m_ofs(o.m_ofs){}

		~StateStackElem(){}
	};

	///\class StateStack
	///\brief Stack for parameter initialization statemachine
	class StateStack
	{
	public:
		StateStack(){}
		~StateStack()						{clear();}
		void push( const StateStackElem& e)	{m_impl.push_back( e);}
		void pop()							{m_impl.pop_back();}
		StateStackElem& top()				{return m_impl.back();}
		const StateStackElem& top()	const	{return m_impl.back();}
		bool empty() const					{return m_impl.empty();}
		std::size_t size() const			{return m_impl.size();}
		void clear()						{while (!empty()) pop();}
		std::string variablepath() const;

	private:
		std::vector<StateStackElem> m_impl;
	};

private:
	const proc::ProcessorProvider* m_provider;		//< processor provider reference for function called
	const Function* m_func;							//< function to call
	langbind::TypedInputFilterR m_input;			//< input parameters
	serialize::Context::Flags m_flags;				//< flag passed by called to stear validation strictness
	VARIANT* m_parameter;							//< array of function parameters to initialize
	langbind::TypedInputFilterR m_result;			//< reference to result of the function call
	StateStack m_statestack;						//< stack of parameter initialization state machine
	VARIANT* m_paramvalue;							//< currently selected parameter value to initialize
	std::size_t m_paramidx;							//< currently selected parameter of the function [0,1,.. n-1]
	VARTYPE m_selectedtype;							//< currently selected variable type, determines intialization of an atomic type in the structure by value
	unsigned int m_recdepht;						//< depht in tree (used for RecordInfoMap key - RecordInfoKey)
};


class Function
	:public langbind::FormFunction
{
public:
	struct Parameter
	{
		std::string name;
		const TYPEDESC* typedesc;
		RecordInfoMap recinfomap;

		Parameter( const Parameter& o)
			:name(o.name),typedesc(o.typedesc),recinfomap(o.recinfomap){}
		Parameter( const std::string& name_, const TYPEDESC* typedesc_, const RecordInfoMap& recinfomap_)
			:name(name_),typedesc(typedesc_),recinfomap(recinfomap_){}
		Parameter( const std::string& name_, const TYPEDESC* typedesc_)
			:name(name_),typedesc(typedesc_){}
	};

	struct ReturnType
	{
		ReturnType( const ReturnType& o)
			:typedesc(o.typedesc),typeinfo(o.typeinfo){}
		ReturnType( const TYPEDESC* typedesc_, const CComPtr<ITypeInfo>& typeinfo_)
			:typedesc(typedesc_),typeinfo(typeinfo_){}
		explicit ReturnType( const TYPEDESC* typedesc_=0)
			:typedesc(typedesc_){}

		const TYPEDESC* typedesc;
		CComPtr<ITypeInfo> typeinfo;
	};

public:
	Function( comauto::CommonLanguageRuntime* clr_, ITypeInfo* typeinfo_, const std::string& assemblyname_, const std::string& classname_, unsigned short fidx);
	virtual ~Function();

	const std::string& assemblyname() const								{return m_assemblyname;}
	const std::string& classname() const								{return m_classname;}
	const std::string& methodname() const								{return m_methodname;}
	std::size_t nofParameter() const									{return m_parameterlist.size();}
	std::size_t getParameterIndex( const std::string& name) const;
	const Parameter* getParameter( std::size_t idx) const;
	comauto::CommonLanguageRuntime* clr() const							{return m_clr;}
	ITypeInfo* typeinfo() const											{return m_typeinfo;}
	const ReturnType* getReturnType() const								{return &m_returntype;}

	void print( std::ostream& out) const;
	virtual langbind::FormFunctionClosure* createClosure() const		{return new FunctionClosure(this);}

private:
	comauto::CommonLanguageRuntime* m_clr;
	ITypeInfo* m_typeinfo;
	FUNCDESC* m_funcdesc;
	std::string m_assemblyname;
	std::string m_classname;
	std::string m_methodname;
	std::vector<Parameter> m_parameterlist;
	ReturnType m_returntype;
};

typedef boost::shared_ptr<Function> FunctionR;

}} //namespace
#endif


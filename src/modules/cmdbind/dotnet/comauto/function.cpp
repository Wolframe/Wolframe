/************************************************************************
Copyright (C) 2011 - 2013 Project Wolframe.
All rights reserved.

This file is part of Project Wolframe.

Commercial Usage
Licensees holding valid Project Wolframe Commercial licenses may
use this file in accordance with the Project Wolframe
Commercial License Agreement provided with the Software or,
alternatively, in accordance with the terms contained
in a written agreement between the licensee and Project Wolframe.

GNU General Public License Usage
Alternatively, you can redistribute this file and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Wolframe is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
#include "comauto/function.hpp"
#include "comauto/utils.hpp"
#include "comauto/clr.hpp"
#include "comauto/typelib.hpp"
#include <oaidl.h>
#include <comdef.h>
#include <atlbase.h>
#include <atlcom.h>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>

using namespace _Wolframe;
using namespace _Wolframe::comauto;

class DotnetFunctionClosure::Impl
{
public:
	Impl( const DotnetFunction* func_);

	~Impl();
	bool call();

	void init( const proc::ProcessorProvider* p, const langbind::TypedInputFilterR& i, serialize::Context::Flags f=serialize::Context::None);

	langbind::TypedInputFilterR result() const;

private:
	const proc::ProcessorProvider* m_provider;			//< processor provider reference for function called
	const DotnetFunction* m_func;					//< function to call
	langbind::TypedInputFilterR m_input;				//< input parameters
	serialize::Context::Flags m_flags;				//< flag passed by called to stear validation strictness
	VARIANT* m_param;						//< array of function parameters to initialize
	enum {null_paramidx=0xFFFF};
	std::size_t m_paramidx;						//< currently selected parameter of the function [0,1,.. n-1]
	AssignmentClosureR m_paramclosure;			//< closure for current parameter assignment
	std::map<std::size_t,std::vector<VARIANT> > m_arrayparam;	//< temporary buffer for parameters passed as array (with their name)
	langbind::TypedInputFilterR m_result;				//< reference to result of the function call
};

class DotnetFunction::Impl
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
	Impl( comauto::CommonLanguageRuntime* clr_, const comauto::TypeLib* typelib_, const ITypeInfo* typeinfo_, const std::string& assemblyname_, const std::string& classname_, unsigned short fidx);
	~Impl();

	const std::string& assemblyname() const				{return m_assemblyname;}
	const std::string& classname() const				{return m_classname;}
	const std::string& methodname() const				{return m_methodname;}
	std::size_t nofParameter() const				{return m_parameterlist.size();}
	std::size_t getParameterIndex( const std::string& name) const;
	const Parameter* getParameter( std::size_t idx) const;
	comauto::CommonLanguageRuntime* clr() const			{return m_clr;}
	const comauto::TypeLib* typelib() const				{return m_typelib;}
	const ITypeInfo* typeinfo() const				{return m_typeinfo;}
	const ReturnType* getReturnType() const				{return &m_returntype;}

	void print( std::ostream& out) const;

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

namespace
{
class FunctionResult
	:public langbind::TypedInputFilter
{
public:
	FunctionResult( const comauto::TypeLib* typelib_, const ITypeInfo* typeinfo_, VARIANT data_, serialize::Context::Flags flags_)
		:types::TypeSignature( "comauto::FunctionResult", __LINE__)
		,m_typelib(typelib_)
		,m_flags(flags_)
	{
		m_stk.push_back( StackElem( "", m_typelib->getRecordInfo(typeinfo_), const_cast<ITypeInfo*>(typeinfo_), data_));
	}

	FunctionResult( const FunctionResult& o)
		:types::TypeSignature( "comauto::FunctionResult", __LINE__)
		,m_stk(o.m_stk)
		,m_elembuf(o.m_elembuf)
		,m_typelib(o.m_typelib)
	{}

	virtual ~FunctionResult()
	{}

	virtual TypedInputFilter* copy() const
	{
		return new FunctionResult( *this);
	}

	virtual bool getNext( ElementType& type, types::VariantConst& element);

private:
	enum State
	{
		VarOpen,
		VarClose
	};
	struct StackElem
	{
		State state;
		ITypeInfo* typeinfo;
		const IRecordInfo* recinfo;
		TYPEATTR* typeattr;
		VARIANT data;
		std::string name;
		std::size_t idx;

		StackElem( const StackElem& o);
		StackElem( const std::string& name_, const IRecordInfo* recinfo_, ITypeInfo* typeinfo_, VARIANT data_);
		~StackElem();
	};
	std::vector<StackElem> m_stk;
	std::string m_elembuf;
	const comauto::TypeLib* m_typelib;
	serialize::Context::Flags m_flags;
};
}//anonymous namespace


static void findFunctions( const comauto::TypeLib* typelib, const ITypeInfo* typeinfo, std::vector<comauto::DotnetFunctionR>& funcs, comauto::CommonLanguageRuntime* clr, const std::string& assemblyname, const std::string& classname)
{
	TYPEATTR* typeattr = 0;
	ITypeInfo* classtypeinfo = 0;
	try
	{
		unsigned short ii;
		WRAP( const_cast<ITypeInfo*>(typeinfo)->GetTypeAttr( &typeattr))
		if ((typeattr->typekind == TKIND_DISPATCH || typeattr->typekind == TKIND_RECORD) && classname.empty())
		{
			return; //no follow on toplevel interface or POD data structure declaration
		}
		if (typeattr->cFuncs)
		{
			if (classname.empty())
			{
				throw std::runtime_error( "functions found outside class context");
			}
			for (ii = 0; ii < typeattr->cFuncs; ++ii)
			{
				comauto::DotnetFunctionR func( new comauto::DotnetFunction( clr, typelib, typeinfo, assemblyname, classname, ii));
				if (!comauto::isCOMInterfaceMethod( func->methodname()))
				{
					funcs.push_back( func);
				}
			}
		}
		std::string subclassname( classname);
		if (typeattr->typekind == TKIND_COCLASS)
		{
			if (!classname.empty()) throw std::runtime_error( "nested class definitions not supported");
			if (subclassname != "IUnknown")
			{
				subclassname.clear();
				subclassname.append( comauto::typestr( typeinfo));
			}
		}
		for (ii = 0; ii < typeattr->cImplTypes; ++ii)
		{
			HREFTYPE hreftype;
			WRAP( const_cast<ITypeInfo*>(typeinfo)->GetRefTypeOfImplType( ii, &hreftype))
			WRAP( const_cast<ITypeInfo*>(typeinfo)->GetRefTypeInfo( hreftype, &classtypeinfo))
			findFunctions( typelib, classtypeinfo, funcs, clr, assemblyname, subclassname);
			classtypeinfo->Release();
			classtypeinfo = 0;
		}
		const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
		typeattr = 0;
	}
	catch (const std::runtime_error& e)
	{
		if (typeattr) const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
		if (classtypeinfo) classtypeinfo->Release();
		throw e;
	}
}

std::vector<comauto::DotnetFunctionR> comauto::loadFunctions( const comauto::TypeLib* typelib, comauto::CommonLanguageRuntime* clr, const std::string& assemblyname)
{
	std::vector<comauto::DotnetFunctionR> rt;
	ITypeInfo* typeinfo = 0;
	try
	{
		ITypeLib* tl = const_cast<ITypeLib*>( typelib->typelib());
		UINT ii = 0, nn = tl->GetTypeInfoCount();

		for (; ii < nn; ++ii)
		{
			WRAP( tl->GetTypeInfo( ii, &typeinfo))
			typelib->getRecordInfo( typeinfo);
			findFunctions( typelib, typeinfo, rt, clr, assemblyname, "");
			typeinfo->Release();
			typeinfo = 0;
		}
	}
	catch (const std::runtime_error& e)
	{
		if (typeinfo) typeinfo->Release();
		throw e;
	}
	return rt;
}


comauto::DotnetFunction::Impl::Parameter::Parameter( const Parameter& o)
	:name(o.name),addrMode(o.addrMode),typedesc(o.typedesc),typeinfo(o.typeinfo)
{
	if (typeinfo) const_cast<ITypeInfo*>(typeinfo)->AddRef();
}

comauto::DotnetFunction::Impl::Parameter::Parameter( const std::string& name_, const TYPEDESC* typedesc_, const ITypeInfo* typeinfo_, AddrMode addrMode_)
	:name(name_),addrMode(addrMode_),typedesc(typedesc_),typeinfo(typeinfo_)
{
	if (typeinfo) const_cast<ITypeInfo*>(typeinfo)->AddRef();
}

comauto::DotnetFunction::Impl::Parameter::~Parameter()
{
	if (typeinfo) const_cast<ITypeInfo*>(typeinfo)->Release();
}

comauto::DotnetFunction::Impl::ReturnType::ReturnType( const ReturnType& o)
	:typedesc(o.typedesc),typeinfo(o.typeinfo)
{
	if (typeinfo) const_cast<ITypeInfo*>(typeinfo)->AddRef();
}

comauto::DotnetFunction::Impl::ReturnType::ReturnType( const TYPEDESC* typedesc_, const ITypeInfo* typeinfo_)
	:typedesc(typedesc_),typeinfo(typeinfo_)
{
	if (typeinfo) const_cast<ITypeInfo*>(typeinfo)->AddRef();
}

comauto::DotnetFunction::Impl::ReturnType::ReturnType( const TYPEDESC* typedesc_)
	:typedesc(typedesc_),typeinfo(0){}

comauto::DotnetFunction::Impl::ReturnType::~ReturnType()
{
	if (typeinfo) const_cast<ITypeInfo*>(typeinfo)->Release();
}

comauto::DotnetFunction::Impl::Impl( comauto::CommonLanguageRuntime* clr_, const comauto::TypeLib* typelib_, const ITypeInfo* typeinfo_, const std::string& assemblyname_, const std::string& classname_, unsigned short fidx)
	:m_clr(clr_)
	,m_typelib(typelib_)
	,m_typeinfo(typeinfo_)
	,m_funcdesc(0)
	,m_assemblyname(assemblyname_)
	,m_classname(classname_)
{
	if (m_typeinfo) const_cast<ITypeInfo*>(m_typeinfo)->AddRef();
	WRAP( const_cast<ITypeInfo*>(m_typeinfo)->GetFuncDesc( fidx, &m_funcdesc))
	struct Local	//exception safe memory allocation of local variables
	{
		BSTR* pnames;
		UINT size;
		ITypeInfo* rectypeinfo;

		Local()	:pnames(0),rectypeinfo(0){}
		~Local()
		{
			if (pnames)
			{
				for (UINT ii=0; ii<size; ++ii) if (pnames[ii]) ::SysFreeString( pnames[ii]);
				delete [] pnames;
			}
			if (rectypeinfo) rectypeinfo->Release();
		}
	};
	Local local;
	local.pnames = new BSTR[ local.size = m_funcdesc->cParams+1];
	UINT ii,nn;
	WRAP( const_cast<ITypeInfo*>(m_typeinfo)->GetNames( m_funcdesc->memid, local.pnames, m_funcdesc->cParams+1, &nn))

	m_methodname = comauto::utf8string( local.pnames[0]);
	for (ii=1; ii<nn; ++ii)
	{
		const TYPEDESC* td = &m_funcdesc->lprgelemdescParam[ii-1].tdesc;
		if (td->vt == VT_USERDEFINED)
		{
			if (local.rectypeinfo) local.rectypeinfo->Release();
			local.rectypeinfo = 0;
			WRAP( const_cast<ITypeInfo*>(m_typeinfo)->GetRefTypeInfo( td->hreftype, &local.rectypeinfo))
			Parameter param( comauto::utf8string( local.pnames[ii]), td, local.rectypeinfo, Parameter::Value);
			m_parameterlist.push_back( param);
		}
		else if (td->vt == VT_SAFEARRAY)
		{
			if (local.rectypeinfo) local.rectypeinfo->Release();
			local.rectypeinfo = 0;
			if (!comauto::isAtomicType(td->lptdesc->vt) && !comauto::isStringType(td->lptdesc->vt))
			{
				WRAP( const_cast<ITypeInfo*>(m_typeinfo)->GetRefTypeInfo( td->lptdesc->hreftype, &local.rectypeinfo))
			}
			Parameter param( comauto::utf8string( local.pnames[ii]), td->lptdesc, local.rectypeinfo, Parameter::Safearray);
			m_parameterlist.push_back( param);
		}
		else
		{
			m_parameterlist.push_back( Parameter( comauto::utf8string( local.pnames[ii]), td, 0, Parameter::Value));
		}
	}
	if (m_funcdesc->elemdescFunc.tdesc.vt == VT_USERDEFINED)
	{
		if (local.rectypeinfo) local.rectypeinfo->Release();
		local.rectypeinfo = 0;
		WRAP( const_cast<ITypeInfo*>(m_typeinfo)->GetRefTypeInfo( m_funcdesc->elemdescFunc.tdesc.hreftype, &local.rectypeinfo))
		m_returntype = ReturnType( &m_funcdesc->elemdescFunc.tdesc, local.rectypeinfo);
	}
	else
	{
		m_returntype = ReturnType( &m_funcdesc->elemdescFunc.tdesc);
	}
}

comauto::DotnetFunction::Impl::~Impl()
{
	if (m_typeinfo)
	{
		const_cast<ITypeInfo*>(m_typeinfo)->ReleaseFuncDesc( m_funcdesc);
		const_cast<ITypeInfo*>(m_typeinfo)->Release();
	}
}

void comauto::DotnetFunction::Impl::print( std::ostream& out) const
{
	if (m_classname.empty())
	{
		out << "FUNCTION " << m_methodname << "( ";
	}
	else
	{
		out << "METHOD " << m_classname << "." << m_methodname << "( ";
	}
	std::vector<Parameter>::const_iterator pi = m_parameterlist.begin(), pe = m_parameterlist.end();
	for (; pi != pe; ++pi)
	{
		if (pi != m_parameterlist.begin()) out << ", ";
		out << pi->name << " :" << ((pi->addrMode==Parameter::Safearray)?"[] ":"") << comauto::typestr( m_typeinfo, const_cast<TYPEDESC*>(pi->typedesc));
	}
	out << " ) :" << comauto::typestr( m_typeinfo, m_returntype.typedesc) << std::endl;
}

std::size_t comauto::DotnetFunction::Impl::getParameterIndex( const std::string& name) const
{
	std::vector<Parameter>::const_iterator pi = m_parameterlist.begin(), pe = m_parameterlist.end();
	for (; pi != pe; ++pi)
	{
		if (pi->name == name) return pi-m_parameterlist.begin();
	}
	throw std::runtime_error( std::string("unknown parameter name '") + name + "'");
}

const comauto::DotnetFunction::Impl::Parameter* comauto::DotnetFunction::Impl::getParameter( std::size_t idx) const
{
	if (idx >= m_parameterlist.size()) throw std::runtime_error( "parameter index out of range");
	return &m_parameterlist.at( idx);
}

comauto::DotnetFunctionClosure::Impl::Impl( const DotnetFunction* func_)
		:m_provider(0)
		,m_func(func_)
		,m_flags(serialize::Context::None)
		,m_param(0)
		,m_paramidx(null_paramidx){}

static void clearArrayParam( std::map<std::size_t,std::vector<VARIANT> >& ap)
{
	std::map<std::size_t,std::vector<VARIANT> >::iterator ai = ap.begin(), ae = ap.end();
	for (; ai != ae; ++ai)
	{
		std::vector<VARIANT>::iterator vi = ai->second.begin(), ve = ai->second.end();
		for (; vi != ve; ++vi) comauto::wrapVariantClear( &*vi);
	}
	ap.clear();
}

comauto::DotnetFunctionClosure::Impl::~Impl()
{
	if (m_param)
	{
		std::size_t pi = 0, pe = m_func->m_impl->nofParameter();
		for (; pi != pe; ++pi)
		{
			comauto::wrapVariantClear( &m_param[pi]);
		}
		delete [] m_param;
	}
	clearArrayParam( m_arrayparam);
}

void comauto::DotnetFunctionClosure::Impl::init( const proc::ProcessorProvider* p, const langbind::TypedInputFilterR& i, serialize::Context::Flags f)
{
	m_provider = p;
	m_input = i;
	m_flags = f;
	std::size_t ii = 0,nn = m_func->m_impl->nofParameter();
	if (m_param) delete [] m_param;
	m_param = 0;	//... because of exception safety (new VARIANT[] might fail)
	m_param = new VARIANT[ m_func->m_impl->nofParameter()];
	for (; ii < nn; ++ii)
	{
		m_param[ii].vt = VT_EMPTY;
	}
	m_paramidx = 0;
	clearArrayParam( m_arrayparam);
}


bool comauto::DotnetFunctionClosure::Impl::call()
{
AGAIN:
	try
	{
		langbind::FilterBase::ElementType elemtype;
		types::VariantConst elemvalue;

		if (m_paramclosure.get())
		{
			//... we are in a parameter initilization
			VARIANT paramvalue;
			paramvalue.vt = VT_EMPTY;
			try
			{
				if (!m_paramclosure->call( paramvalue))
				{
					if (m_input->state() == langbind::InputFilter::Open) throw std::runtime_error( "unexpected end of input");
					return false;
				}
				const DotnetFunction::Impl::Parameter* paramdescr = m_func->m_impl->getParameter( m_paramidx);
				if (m_param[ m_paramidx].vt != VT_EMPTY) throw std::runtime_error( std::string("duplicate definition of parameter '") + paramdescr->name + "'");

				switch (paramdescr->addrMode)
				{
					case DotnetFunction::Impl::Parameter::Value:
					{
						m_param[ m_paramidx] = paramvalue;
						break;
					}
					case DotnetFunction::Impl::Parameter::Safearray:
					{
						m_arrayparam[ m_paramidx].push_back( paramvalue);
						paramvalue.vt = VT_EMPTY;
						break;
					}
				}
				m_paramidx = null_paramidx;
				m_paramclosure.reset();
			}
			catch (const std::runtime_error& e)
			{
				comauto::wrapVariantClear( &paramvalue);
				throw e;
			}
		}
		while (m_input->getNext( elemtype, elemvalue))
		{
			switch (elemtype)
			{
				case langbind::InputFilter::Attribute:
				case langbind::InputFilter::OpenTag:
				{
					if (elemvalue.type() == types::Variant::Int)
					{
						__int64 val = elemvalue.data().value.Int;
						if (val < 0 || (std::size_t)val >= m_func->m_impl->nofParameter()) throw std::runtime_error( "function parameter index out of range");
						m_paramidx = (std::size_t)val;
					}
					else if (elemvalue.type() == types::Variant::UInt)
					{
						unsigned __int64 val = elemvalue.data().value.UInt;
						if ((std::size_t)val >= m_func->m_impl->nofParameter()) throw std::runtime_error( "function parameter index out of range");
						m_paramidx = (std::size_t)val;
					}
					else if (elemvalue.type() == types::Variant::String)
					{
						std::string paramname( elemvalue.charptr(), elemvalue.charsize());
						m_paramidx = m_func->m_impl->getParameterIndex( paramname);
					}
					else
					{
						throw std::runtime_error( "unexpected node type (function parameter name or index expected)");
					}
					const DotnetFunction::Impl::Parameter* param = m_func->m_impl->getParameter( m_paramidx);

					if (elemtype==langbind::InputFilter::Attribute)
					{
						if (param->typeinfo)
						{
							throw std::runtime_error( "atomic parameter expected if passed as attribute");
						}
						else
						{
							m_paramclosure.reset( new AssignmentClosure( m_func->m_impl->typelib(), m_input, param->typedesc->vt, true));
						}
					}
					else
					{
						if (param->typeinfo)
						{
							m_paramclosure.reset( new AssignmentClosure( m_func->m_impl->typelib(), m_input, param->typeinfo));
						}
						else
						{
							m_paramclosure.reset( new AssignmentClosure( m_func->m_impl->typelib(), m_input, param->typedesc->vt, false));
						}
					}
					goto AGAIN;
				}
				case langbind::InputFilter::CloseTag:
					throw std::runtime_error( "unexpected close tag (tags not balanced)");
					break;

				case langbind::InputFilter::Value:
					throw std::runtime_error( "unexpected content token (expected a parameter name or index)");
			}
			break;
		}
		switch (m_input->state())
		{
			case langbind::InputFilter::Error: return false;
			case langbind::InputFilter::EndOfMessage: return false;
			case langbind::InputFilter::Open: break;
		}
		// create all array parameters:
		std::map<std::size_t,std::vector<VARIANT> >::iterator pi = m_arrayparam.begin(), pe = m_arrayparam.end();
		for (; pi != pe; ++pi)
		{
			const DotnetFunction::Impl::Parameter* param = m_func->m_impl->getParameter( pi->first);
			const IRecordInfo* recinfo = m_func->m_impl->typelib()->getRecordInfo( param->typeinfo);
			m_param[ pi->first] = comauto::createVariantArray( pi->second[0].vt, recinfo, pi->second);
		}

		// function signature validation:
		if ((m_flags & serialize::Context::ValidateAttributes) != 0)
		{
			// ... don't know yet what to do here, because attributes are not defined for .NET functions ...
		}
		std::size_t ii = 0,nn = m_func->m_impl->nofParameter();
		for (; ii < nn; ++ii)
		{
			if (m_param[ii].vt == VT_EMPTY)
			{
				const DotnetFunction::Impl::Parameter* param = m_func->m_impl->getParameter( ii);

				if ((m_flags & serialize::Context::ValidateInitialization) != 0)
				{
					throw std::runtime_error( std::string( "missing parameter '") + param->name + "'");
				}
				switch (param->addrMode)
				{
					case DotnetFunction::Impl::Parameter::Safearray:
						if (comauto::isAtomicType( param->typedesc->vt))
						{
							const IRecordInfo* recinfo = m_func->m_impl->typelib()->getRecordInfo( param->typeinfo);
							m_param[ ii] = comauto::createVariantArray( param->typedesc->vt, recinfo);
						}
						else if (param->typeinfo)
						{
							const IRecordInfo* recinfo = m_func->m_impl->typelib()->getRecordInfo( param->typeinfo);
							if (!recinfo) throw std::runtime_error( std::string( "default initialization of parameter '") + param->name + "' failed (record info unknown)");
							m_param[ ii] = comauto::createVariantArray( param->typedesc->vt, recinfo);
						}
						else
						{
							throw std::runtime_error( std::string( "default initialization of parameter '") + param->name + "' failed");
						}
						break;
					case DotnetFunction::Impl::Parameter::Value:
						if (comauto::isAtomicType( param->typedesc->vt))
						{
							m_param[ ii] = comauto::createVariantType( types::Variant(), param->typedesc->vt);
						}
						else if (param->typeinfo)
						{
							const IRecordInfo* recinfo = m_func->m_impl->typelib()->getRecordInfo( param->typeinfo);
							if (!recinfo) throw std::runtime_error( std::string( "default initialization of parameter '") + param->name + "' failed (record info unknown)");
							m_param[ ii].pvRecord = const_cast<IRecordInfo*>(recinfo)->RecordCreate();
							m_param[ ii].pRecInfo = const_cast<IRecordInfo*>(recinfo);
							m_param[ ii].vt = param->typedesc->vt;
						}
						else
						{
							throw std::runtime_error( std::string( "default initialization of parameter '") + param->name + "' failed");
						}
						break;
				}
			}
		}
		VARIANT resdata;
		m_func->m_impl->clr()->call( &resdata, m_func->m_impl->assemblyname(), m_func->m_impl->classname(), m_func->m_impl->methodname(), m_func->m_impl->nofParameter(), m_param);
		m_result.reset( new FunctionResult( m_func->m_impl->typelib(), m_func->m_impl->getReturnType()->typeinfo, resdata, m_flags));
		return true;
	}
	catch (const std::runtime_error& e)
	{
		std::string funcname = m_func->m_impl->classname() + "." +  m_func->m_impl->methodname();
		if (m_paramclosure.get())
		{
			const DotnetFunction::Impl::Parameter* paramdescr = m_func->m_impl->getParameter( m_paramidx);
			throw std::runtime_error( std::string("error calling function '") + funcname + " in parameter '" + paramdescr->name + "' (" + m_paramclosure->variablepath() + "): " + e.what());
		}
		else
		{
			throw std::runtime_error( std::string("error calling function '") + funcname + "': " + e.what());
		}
	}
}

langbind::TypedInputFilterR comauto::DotnetFunctionClosure::Impl::result() const
{
	return m_result;
}

FunctionResult::StackElem::StackElem( const StackElem& o)
	:name(o.name),state(o.state),recinfo(o.recinfo),typeinfo(o.typeinfo),typeattr(o.typeattr),idx(o.idx)
{
	try
	{
		data.vt = VT_EMPTY;
		WRAP( comauto::wrapVariantCopy( &data, &o.data))
		if (typeinfo)
		{
			typeinfo->AddRef();
			WRAP( typeinfo->GetTypeAttr( &typeattr))
		}
	}
	catch (const std::runtime_error& e)
	{
		comauto::wrapVariantClear( &data);
		if (typeinfo)
		{
			if (typeattr) typeinfo->ReleaseTypeAttr( typeattr);
			typeinfo->Release();
		}
		throw e;
	}
}

FunctionResult::StackElem::StackElem( const std::string& name_, const IRecordInfo* recinfo_, ITypeInfo* typeinfo_, VARIANT data_)
	:name(name_),state(VarOpen),recinfo(recinfo_),typeinfo(typeinfo_),typeattr(0),data(data_),idx(0)
{
	try
	{
		if (typeinfo)
		{
			typeinfo->AddRef();
			WRAP( typeinfo->GetTypeAttr( &typeattr))
		}
	}
	catch (const std::runtime_error& e)
	{
		if (typeinfo)
		{
			if (typeattr) typeinfo->ReleaseTypeAttr( typeattr);
			typeinfo->Release();
		}
		throw e;
	}
}

FunctionResult::StackElem::~StackElem()
{
	comauto::wrapVariantClear( &data);
	if (typeinfo)
	{
		if (typeattr) typeinfo->ReleaseTypeAttr( typeattr);
		typeinfo->Release();
	}
}

bool FunctionResult::getNext( ElementType& type, types::VariantConst& element)
{
AGAIN:
	VARIANT data;
	data.vt = VT_EMPTY;
	VARDESC* vardesc = 0;
	ITypeInfo* reftypeinfo = 0;
	try
	{
		if (m_stk.empty()) return false;
		StackElem& cur = m_stk.back();	//< REMARK: 'cur' only valid till next m_stk.push_back()/pop_back(). Check that push/pop return or goto AGAIN !
		switch (cur.state)
		{
			case VarOpen:
			{
				if ((cur.data.vt & VT_ARRAY) == VT_ARRAY)
				{
					if (1 != cur.data.parray->cDims)
					{
						throw std::runtime_error( "cannont handle multi dimensional arrays");
					}
					if (cur.idx >= cur.data.parray->rgsabound->cElements)
					{
						m_stk.pop_back();
						goto AGAIN;
					}
					cur.state = VarClose;
					if (((int)m_flags & serialize::Context::SerializeWithIndices) != 0 || cur.name.empty())
					{
						element = types::VariantConst( cur.idx+1);
					}
					else
					{
						element = types::VariantConst( m_elembuf = cur.name);
					}
					type = OpenTag;
					LONG idx = cur.idx++;
					VARTYPE elemvt = cur.data.vt - VT_ARRAY;

					if (comauto::isAtomicType( elemvt))
					{
						data.vt = elemvt;
						WRAP( ::SafeArrayGetElement( cur.data.parray, &idx, const_cast<void*>(comauto::arithmeticTypeAddress( &data))))
						cur.state = VarClose;
						m_stk.push_back( StackElem( "", 0, 0, data));
						std::memset( &data, 0, sizeof(data));
						data.vt = VT_EMPTY;
					}
					else if (comauto::isStringType( elemvt))
					{
						std::memset( &data, 0, sizeof(data));
						data.vt = elemvt;
						switch (elemvt)
						{
							case VT_LPSTR:
								WRAP( ::SafeArrayGetElement( cur.data.parray, &idx, &V_LPSTR( const_cast<VARIANT*>(&data))))
								break;
							case VT_LPWSTR:
								WRAP( ::SafeArrayGetElement( cur.data.parray, &idx, &V_LPWSTR( const_cast<VARIANT*>(&data))))
								break;
							case VT_BSTR:
								WRAP( ::SafeArrayGetElement( cur.data.parray, &idx, V_BSTR( const_cast<VARIANT*>(&data))))
								break;
							default:
								throw std::logic_error("internal: unknown string type");
						}
						cur.state = VarClose;
						m_stk.push_back( StackElem( "", 0, 0, data));
						std::memset( &data, 0, sizeof(data));
						data.vt = VT_EMPTY;
					}
					else if (elemvt == VT_RECORD)
					{
						std::memset( &data, 0, sizeof(data));
						data.vt = elemvt;
						WRAP( ::SafeArrayGetRecordInfo( cur.data.parray, &data.pRecInfo))
						if (!data.pRecInfo) throw std::runtime_error( "cannot iterate on result structure without record info");
						data.pvRecord = data.pRecInfo->RecordCreate();
						WRAP( ::SafeArrayGetElement( cur.data.parray, &idx, const_cast<void*>(data.pvRecord)))
						data.pRecInfo->GetTypeInfo( &reftypeinfo);
						cur.state = VarClose;
						m_stk.push_back( StackElem( "", data.pRecInfo, reftypeinfo, data));
						std::memset( &data, 0, sizeof(data));
						data.vt = VT_EMPTY;
						reftypeinfo->Release();
						reftypeinfo = 0;
					}
					else
					{
						throw std::runtime_error(std::string("cannot handle this array element type in result: '") + comauto::typestr(elemvt));
					}
					return true;
				}
				else if (comauto::isAtomicType( cur.data.vt) || comauto::isStringType( cur.data.vt))
				{
					element = comauto::getAtomicElement( cur.data, m_elembuf);
					type = Value;
					m_stk.pop_back();
					return true;
				}
				else if (cur.data.vt == VT_RECORD)
				{
					LONG idx = cur.idx++;
					cur.state = VarClose;

					if (idx >= cur.typeattr->cVars)
					{
						m_stk.pop_back();
						goto AGAIN;
					}
					WRAP( cur.typeinfo->GetVarDesc( idx, &vardesc))
					std::wstring varname( comauto::variablename_utf16( cur.typeinfo, vardesc));
					cur.typeinfo->ReleaseVarDesc( vardesc);
					vardesc = 0;

					const_cast<IRecordInfo*>(cur.recinfo)->GetField( cur.data.pvRecord, varname.c_str(), &data);
					if ((data.vt & VT_ARRAY) == VT_ARRAY)
					{
						bool rt = false;
						std::string elemname;
						if (((int)m_flags & serialize::Context::SerializeWithIndices) != 0)
						{
							type = OpenTag;
							element = types::VariantConst( m_elembuf = comauto::utf8string( varname));
							cur.state = VarClose;
							rt = true;
						}
						else
						{
							elemname = comauto::utf8string(varname);
							cur.state = VarOpen;
						}
						m_stk.push_back( StackElem( elemname, 0, 0, data));
						std::memset( &data, 0, sizeof(data));
						data.vt = VT_EMPTY;
						if (rt) return true;
						goto AGAIN;
					}
					else if (data.vt == VT_RECORD)
					{
						if (!data.pRecInfo || !data.pvRecord) 
						{
							throw std::runtime_error("cannot iterate through structure without record info");
						}
						WRAP( data.pRecInfo->GetTypeInfo( &reftypeinfo));
						m_stk.push_back( StackElem( "", data.pRecInfo, reftypeinfo, data));
						reftypeinfo->Release();
						reftypeinfo = 0;
						type = OpenTag;
						element = types::VariantConst( m_elembuf = comauto::utf8string( varname));
					}
					else if (comauto::isAtomicType( data.vt) || comauto::isStringType( data.vt))
					{
						m_stk.push_back( StackElem( "", 0, 0, data));
						type = OpenTag;
						element = types::VariantConst( m_elembuf = comauto::utf8string( varname));
					}
					else
					{
						throw std::runtime_error( std::string( "cannot handle this type in result structure '") + comauto::typestr(data.vt) + "'");
					}
					std::memset( &data, 0, sizeof(data));
					data.vt = VT_EMPTY;
					return true;
				}
				else
				{
					throw std::runtime_error( std::string("cannot handle this type of result '") + comauto::typestr(cur.data.vt) + "'");
				}
			}
			case VarClose:
			{
				cur.state = VarOpen;
				element = types::VariantConst();
				type = CloseTag;
				return true;
			}
		}
	}
	catch (const std::runtime_error& e)
	{
		comauto::wrapVariantClear( &data);
		if (vardesc) m_stk.back().typeinfo->ReleaseVarDesc( vardesc);
		if (reftypeinfo) reftypeinfo->Release();
		throw e;
	}
	return false;
}

DotnetFunctionClosure::DotnetFunctionClosure( const DotnetFunction* func_)
	:m_impl(new Impl( func_))
{}

DotnetFunctionClosure::~DotnetFunctionClosure()
{
	delete m_impl;
}

bool DotnetFunctionClosure::call()
{
	return m_impl->call();
}

void DotnetFunctionClosure::init( const proc::ProcessorProvider* p, const langbind::TypedInputFilterR& i, serialize::Context::Flags f)
{
	m_impl->init( p, i, f);
}

langbind::TypedInputFilterR DotnetFunctionClosure::result() const
{
	return m_impl->result();
}

DotnetFunction::DotnetFunction( comauto::CommonLanguageRuntime* clr_, const comauto::TypeLib* typelib_, const ITypeInfo* typeinfo_, const std::string& assemblyname_, const std::string& classname_, unsigned short fidx)
	:m_impl(new Impl(clr_,typelib_,typeinfo_,assemblyname_,classname_,fidx))
{}

DotnetFunction::~DotnetFunction()
{
	delete m_impl;
}

void DotnetFunction::print( std::ostream& out) const
{
	m_impl->print( out);
}

langbind::FormFunctionClosure* DotnetFunction::createClosure() const
{
	return new DotnetFunctionClosure(this);
}

const std::string& DotnetFunction::assemblyname() const	{return m_impl->assemblyname();}
const std::string& DotnetFunction::classname() const		{return m_impl->classname();}
const std::string& DotnetFunction::methodname() const		{return m_impl->methodname();}


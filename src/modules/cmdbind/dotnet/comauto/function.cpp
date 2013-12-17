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
#include "comauto/procProviderDispatch.hpp"
#include "comauto/function.hpp"
#include "comauto/utils.hpp"
#include "comauto/clr.hpp"
#include "comauto/typelib.hpp"
#include "comauto/variantInputFilter.hpp"
#include "comauto/variantAssignment.hpp"
#include "comauto/variantToString.hpp"
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
	const proc::ProcessorProvider* m_provider;		//< processor provider reference for function called
	const DotnetFunction* m_func;					//< function to call
	langbind::TypedInputFilterR m_input;				//< input parameters
	serialize::Context::Flags m_flags;				//< flag passed by called to stear validation strictness
	VARIANT* m_param;						//< array of function parameters to initialize
	enum {null_paramidx=0xFFFF};
	std::size_t m_paramidx;						//< currently selected parameter of the function [0,1,.. n-1]
	AssignmentClosureR m_paramclosure;			//< closure for current parameter assignment
	std::map<std::size_t,std::vector<VARIANT> > m_arrayparam;	//< temporary buffer for parameters passed as array (with their name)
	IDispatch* m_providerdispatch;					//< IDispatch Interface of processor provider
	langbind::TypedInputFilterR m_result;				//< reference to result of the function call
};

class DotnetFunction::Impl
{
public:
	struct Parameter
	{
		enum AddrMode {Value,SafeArray,ProcProvider};

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

static void findFunctions( const comauto::TypeLib* typelib, const ITypeInfo* typeinfo, std::vector<comauto::DotnetFunctionR>& funcs, comauto::CommonLanguageRuntime* clr, const std::string& assemblyname, const std::string& classname)
{
	TYPEATTR* typeattr = 0;
	ITypeInfo* classtypeinfo = 0;
	try
	{
		unsigned short ii;
		WRAP( const_cast<ITypeInfo*>(typeinfo)->GetTypeAttr( &typeattr));
		if ((typeattr->typekind == TKIND_INTERFACE || typeattr->typekind == TKIND_DISPATCH || typeattr->typekind == TKIND_RECORD) && classname.empty())
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
			if (!classname.empty())
			{
				throw std::runtime_error( "nested class definitions not supported");
			}
			if (subclassname != "IUnknown")
			{
				subclassname.clear();
				subclassname.append( comauto::typestr( typeinfo));
			}
		}
		else if (typeattr->typekind == TKIND_INTERFACE)
		{
			if (subclassname != "IUnknown")
			{
				subclassname.clear();
				subclassname.append( comauto::typestr( typeinfo));
			}
		}
		for (ii = 0; ii < typeattr->cImplTypes; ++ii)
		{
			HREFTYPE hreftype;
			WRAP( const_cast<ITypeInfo*>(typeinfo)->GetRefTypeOfImplType( ii, &hreftype));
			WRAP( const_cast<ITypeInfo*>(typeinfo)->GetRefTypeInfo( hreftype, &classtypeinfo));
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
			WRAP( tl->GetTypeInfo( ii, &typeinfo));
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
	WRAP( const_cast<ITypeInfo*>(m_typeinfo)->GetFuncDesc( fidx, &m_funcdesc));
	if (!m_funcdesc)
	{
		throw std::runtime_error("FUNCDESC not defined for .NET function")
	}
	struct Local	//exception safe memory allocation of local variables
	{
		BSTR* pnames;
		UINT size;
		ITypeInfo* rectypeinfo;
		TYPEATTR* recattr;

		Local()	:pnames(0),rectypeinfo(0),recattr(0){}
		~Local()
		{
			if (pnames)
			{
				for (UINT ii=0; ii<size; ++ii) if (pnames[ii]) ::SysFreeString( pnames[ii]);
				delete [] pnames;
			}
			clear();
		}
		void clear()
		{
			if (rectypeinfo && recattr) rectypeinfo->ReleaseTypeAttr( recattr);
			recattr = 0;
			if (rectypeinfo) rectypeinfo->Release();
			rectypeinfo = 0;
		}
	};
	Local local;
	local.pnames = new BSTR[ local.size = m_funcdesc->cParams+1];
	UINT ii,nn;
	WRAP( const_cast<ITypeInfo*>(m_typeinfo)->GetNames( m_funcdesc->memid, local.pnames, m_funcdesc->cParams+1, &nn));

	m_methodname = comauto::utf8string( local.pnames[0]);
	for (ii=nn-1; ii>0; --ii)
	{
		const TYPEDESC* td = &m_funcdesc->lprgelemdescParam[ii-1].tdesc;
		if (td->vt == VT_USERDEFINED)
		{
			local.clear();

			WRAP( const_cast<ITypeInfo*>(m_typeinfo)->GetRefTypeInfo( td->hreftype, &local.rectypeinfo));
			WRAP( local.rectypeinfo->GetTypeAttr( &local.recattr));
			if (local.recattr->typekind == TKIND_RECORD)
			{
				Parameter param( comauto::utf8string( local.pnames[ii]), td, local.rectypeinfo, Parameter::Value);
				m_parameterlist.push_back( param);
			}
			else if (local.recattr->guid == ProcessorProviderDispatch::uuid())
			{
				Parameter param( comauto::utf8string( local.pnames[ii]), td, local.rectypeinfo, Parameter::ProcProvider);
				m_parameterlist.push_back( param);
			}
			else
			{
				throw std::runtime_error( "can only handle VT_USERDEFINED type of kind VT_RECORD (a structure with no methods) or TKIND_DISPATCH as IID_ProcProvider");
			}
		}
		else if (td->vt == VT_PTR && td->lptdesc->vt == VT_USERDEFINED)
		{
			local.clear();

			WRAP( const_cast<ITypeInfo*>(m_typeinfo)->GetRefTypeInfo( td->lptdesc->hreftype, &local.rectypeinfo));
			WRAP( local.rectypeinfo->GetTypeAttr( &local.recattr));
			if (local.recattr->guid == ProcessorProviderDispatch::uuid())
			{
				Parameter param( comauto::utf8string( local.pnames[ii]), td, local.rectypeinfo, Parameter::ProcProvider);
				m_parameterlist.push_back( param);
			}
			else if (local.recattr->typekind == TKIND_RECORD)
			{
				m_parameterlist.push_back( Parameter( comauto::utf8string( local.pnames[ii]), td, 0, Parameter::Value));
			}
			else
			{
				throw std::runtime_error( "can only handle VT_PTR of VT_USERDEFINED type of kind TKIND_RECORD or processor provider context (TKIND_DISPATCH IID_ProcProvider)");
			}
		}
		else if (td->vt == VT_SAFEARRAY)
		{
			local.clear();

			if (!comauto::isAtomicType(td->lptdesc->vt) && !comauto::isStringType(td->lptdesc->vt))
			{
				WRAP( const_cast<ITypeInfo*>(m_typeinfo)->GetRefTypeInfo( td->lptdesc->hreftype, &local.rectypeinfo));
			}
			Parameter param( comauto::utf8string( local.pnames[ii]), td->lptdesc, local.rectypeinfo, Parameter::SafeArray);
			m_parameterlist.push_back( param);
		}
		else
		{
			m_parameterlist.push_back( Parameter( comauto::utf8string( local.pnames[ii]), td, 0, Parameter::Value));
		}
	}
	if (m_funcdesc->elemdescFunc.tdesc.vt == VT_USERDEFINED)
	{
		local.clear();

		WRAP( const_cast<ITypeInfo*>(m_typeinfo)->GetRefTypeInfo( m_funcdesc->elemdescFunc.tdesc.hreftype, &local.rectypeinfo));
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
		out << pi->name << " :" << ((pi->addrMode==Parameter::SafeArray)?"[] ":"") << comauto::typestr( m_typeinfo, const_cast<TYPEDESC*>(pi->typedesc));
	}
	out << " ) :" << comauto::typestr( m_typeinfo, m_returntype.typedesc) << std::endl;
}

std::size_t comauto::DotnetFunction::Impl::getParameterIndex( const std::string& name) const
{
	std::vector<Parameter>::const_iterator pi = m_parameterlist.begin(), pe = m_parameterlist.end();
	for (; pi != pe; ++pi)
	{
		if (pi->name == name)
		{
			return (pi-m_parameterlist.begin());
		}
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
		,m_paramidx(null_paramidx)
		,m_providerdispatch(0){}

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
	if (m_providerdispatch)
	{
		m_providerdispatch->Release();
	}
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
	try
	{
AGAIN:
		langbind::FilterBase::ElementType elemtype;
		types::VariantConst elemvalue;

		if (m_paramclosure.get())
		{
			//... we are in a parameter initilization
			VARIANT paramvalue;
			paramvalue.vt = VT_EMPTY;

			if (!m_paramclosure->call( paramvalue))
			{
				if (m_input->state() == langbind::InputFilter::Open) throw std::runtime_error( "unexpected end of input");
				return false;
			}
			const DotnetFunction::Impl::Parameter* paramdescr = m_func->m_impl->getParameter( m_paramidx);
			if (m_param[ m_paramidx].vt != VT_EMPTY) throw std::runtime_error( std::string("duplicate definition of parameter '") + paramdescr->name + "'");

			switch (paramdescr->addrMode)
			{
				case DotnetFunction::Impl::Parameter::ProcProvider:
				{
					throw std::runtime_error( std::string( "Processor provider parameter is reserved for callback and cannot be initialized: '") + paramdescr->name + "'");
				}
				case DotnetFunction::Impl::Parameter::Value:
				{
					m_param[ m_paramidx] = paramvalue;
					break;
				}
				case DotnetFunction::Impl::Parameter::SafeArray:
				{
					m_arrayparam[ m_paramidx].push_back( paramvalue);
					paramvalue.vt = VT_EMPTY;
					break;
				}
			}
			m_paramidx = null_paramidx;
			m_paramclosure.reset();
		}
		bool parameters_consumed = false;
		while (!parameters_consumed && m_input->getNext( elemtype, elemvalue))
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
					if (m_input->getNext( elemtype, elemvalue))
					{
						throw std::runtime_error( "unexpected close tag (tags not balanced)");
					}
					parameters_consumed = true;
					// ... got final close
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
			// ... ignored because XML attributes are unknown in .NET structures ...
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
					case DotnetFunction::Impl::Parameter::SafeArray:
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
					case DotnetFunction::Impl::Parameter::ProcProvider:
						if (!m_providerdispatch)
						{
							m_providerdispatch = ProcessorProviderDispatch::create( m_provider, m_func->m_impl->typelib(), m_func->m_impl->typelib()->getProviderInterface());
						}
						m_param[ ii].vt = VT_DISPATCH | VT_BYREF;
						m_providerdispatch->AddRef();
						m_param[ ii].ppdispVal = &m_providerdispatch;
						break;
				}
			}
		}
		VARIANT resdata;
		m_func->m_impl->clr()->call( &resdata, m_func->m_impl->assemblyname(), m_func->m_impl->classname(), m_func->m_impl->methodname(), m_func->m_impl->nofParameter(), m_param);
		m_result.reset( new VariantInputFilter( m_func->m_impl->typelib(), m_func->m_impl->getReturnType()->typeinfo, resdata, m_flags));
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

const std::string& DotnetFunction::assemblyname() const		{return m_impl->assemblyname();}
const std::string& DotnetFunction::classname() const		{return m_impl->classname();}
const std::string& DotnetFunction::methodname() const		{return m_impl->methodname();}


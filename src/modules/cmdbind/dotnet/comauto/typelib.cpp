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
#include "comauto/typelib.hpp"
#include "comauto/utils.hpp"
#include "comauto/function.hpp"
#include "comauto/procProviderDispatch.hpp"
#include "comauto/variantAssignment.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <cstring>
#include <oaidl.h>
#include <comdef.h>
#include <atlcom.h>
#include <atlbase.h>

using namespace _Wolframe;

class comauto::TypeLib::Impl
{
public:
	explicit Impl( const std::string& file, comauto::TypeLib* this_);
	virtual ~Impl();

	void print( std::ostream& out) const;
	void printvalue( std::ostream& out, const std::string& name, const VARIANT& val, const ITypeInfo* typeinfo, std::size_t indentcnt=0) const;

	const IRecordInfo* getRecordInfo( const ITypeInfo* typeinfo) const;

	const ITypeLib* typelib() const			{return m_typelib;}
	const TLIBATTR* libattr() const			{return m_libattr;}

	void initProviderInterface();
	ITypeInfo* getProviderInterface() const;

private:
	comauto::TypeLib* m_this;
	ITypeLib* m_typelib;
	TLIBATTR* m_libattr;
	ITypeInfo* m_providerTypeInfo;
};

void comauto::TypeLib::Impl::initProviderInterface()
{
	ITypeInfo* typeinfo = 0;
	TYPEATTR* typeattr = 0;
	try
	{
		UINT ii = 0, nn = const_cast<ITypeLib*>(m_typelib)->GetTypeInfoCount();

		for (ii = 0; ii < nn; ++ii)
		{
			WRAP( const_cast<ITypeLib*>(m_typelib)->GetTypeInfo( ii, &typeinfo))
			WRAP( const_cast<ITypeInfo*>(typeinfo)->GetTypeAttr( &typeattr))

			if (typeattr->typekind == TKIND_DISPATCH && typeattr->guid == comauto::ProcessorProviderDispatch::uuid())
			{
				enum {method_CALL=0};
				if (typeattr->cFuncs < 1) throw std::runtime_error("loaded illegal processor provider interface");
				comauto::DotnetFunction func( 0, m_this, typeinfo, "", "", method_CALL);
				if (func.methodname() != "call") throw std::runtime_error("loaded illegal processor provider interface");

				m_providerTypeInfo = typeinfo;
				if (typeinfo && typeattr) const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
				typeattr = 0;
				typeinfo = 0;
				break;
			}
			if (typeinfo && typeattr) const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
			typeattr = 0;
			if (typeinfo) typeinfo->Release();
			typeinfo = 0;
		}
	}
	catch (const std::runtime_error& e)
	{
		if (typeinfo && typeattr) const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
		if (typeinfo) typeinfo->Release();
		throw e;
	}
}

comauto::TypeLib::Impl::Impl( const std::string& file, comauto::TypeLib* this_)
	:m_this(this_),m_typelib(0),m_libattr(0),m_providerTypeInfo(0)
{
	try
	{
		std::wstring ws_type_lib_file = utf16string( file);
		WRAP( ::LoadTypeLibEx( ws_type_lib_file.c_str(), REGKIND_NONE, &m_typelib))
		WRAP( m_typelib->GetLibAttr( &m_libattr))

		initProviderInterface();
	}
	catch (const std::runtime_error& e)
	{
		if (m_typelib) m_typelib->Release();
		throw e;
	}
}

comauto::TypeLib::Impl::~Impl()
{
	if (m_providerTypeInfo)
	{
		m_providerTypeInfo->Release();
	}
	if (m_typelib)
	{
		if (m_libattr) m_typelib->ReleaseTLibAttr( m_libattr);
		m_typelib->Release();
	}
}

// forward declaration for printAttributes(const char*,ITypeInfo*,TYPEATTR*,int)
static void printItem( std::ostream& out, const comauto::TypeLib* typelib, const ITypeInfo* typeinfo, int indentcnt=0);

static void printVar( std::ostream& out, const ITypeInfo* typeinfo, VARDESC* var, int indentcnt=0)
{
	std::string name = comauto::variablename( typeinfo, var);
	std::string type = comauto::variabletype( typeinfo, var);
	out << std::string(indentcnt,'\t') << "VAR " << name << " :" << type << std::endl;
}

static void printAttributes( std::ostream& out, const char* title, const comauto::TypeLib* typelib, const ITypeInfo* typeinfo, TYPEATTR* typeattr, int indentcnt=0)
{
	ITypeInfo* classtypeinfo = 0;
	VARDESC* var = 0;
	try
	{
		std::string namestr( comauto::typestr( typeinfo));
		if (namestr != "IUnknown")
		{
			out << std::string( indentcnt, '\t') << title << " " << namestr << std::endl;
			unsigned short ii;

			for (ii = 0; ii < typeattr->cImplTypes; ++ii)
			{
				HREFTYPE hreftype;
				WRAP( const_cast<ITypeInfo*>(typeinfo)->GetRefTypeOfImplType( ii, &hreftype))
				WRAP( const_cast<ITypeInfo*>(typeinfo)->GetRefTypeInfo( hreftype, &classtypeinfo))
				printItem( out, typelib, classtypeinfo, indentcnt+1);
				classtypeinfo->Release();
				classtypeinfo = 0;
			}
			for (ii = 0; ii < typeattr->cFuncs; ++ii)
			{
				comauto::DotnetFunction func( 0, typelib, typeinfo, "", "", ii);
				if (!comauto::isCOMInterfaceMethod( func.methodname()))
				{
					out << std::string( indentcnt+1, '\t');
					func.print( out);
				}
			}
			for (ii = 0; ii < typeattr->cVars; ++ii)
			{
				WRAP( const_cast<ITypeInfo*>(typeinfo)->GetVarDesc( ii, &var))
				printVar( out, typeinfo, var, indentcnt+1);
				const_cast<ITypeInfo*>(typeinfo)->ReleaseVarDesc( var);
				var = 0;
			}
		}
	}
	catch (const std::runtime_error& e)
	{
		if (classtypeinfo) classtypeinfo->Release();
		if (var) const_cast<ITypeInfo*>(typeinfo)->ReleaseVarDesc( var);
		throw e;
	}
}

static void printItem( std::ostream& out, const comauto::TypeLib* typelib, const ITypeInfo* typeinfo, int indentcnt)
{
	TYPEATTR* typeattr = 0;
	ITypeInfo* reftypeinfo = 0;
	try
	{
		std::string indent( indentcnt, '\t');
		WRAP( const_cast<ITypeInfo*>(typeinfo)->GetTypeAttr( &typeattr))
		
		switch (typeattr->typekind)
		{
			case TKIND_ENUM:
			{
				out << indent << "ENUM " << std::endl;
				break;
			}
			case TKIND_RECORD:
			{
				printAttributes( out, "RECORD", typelib, typeinfo, typeattr, indentcnt);
				break;
			}
			case TKIND_MODULE:
			{
				out << indent << "MODULE " << std::endl;
				break;
			}
			case TKIND_INTERFACE:
			{
				printAttributes( out, "INTERFACE", typelib, typeinfo, typeattr, indentcnt);
				break;
			}
			case TKIND_DISPATCH:
			{
				printAttributes( out, "DISPATCH", typelib, typeinfo, typeattr, indentcnt);
				break;
			}
			case TKIND_ALIAS:
			{
				out << indent << "ALIAS" << std::endl;
				WRAP( const_cast<ITypeInfo*>(typeinfo)->GetRefTypeInfo( typeattr->tdescAlias.hreftype, &reftypeinfo))
				printItem( out, typelib, reftypeinfo, indentcnt+1);
				reftypeinfo->Release();
				reftypeinfo = 0;
				break;
			}
			case TKIND_UNION:
			{
				out << indent << "UNION " << std::endl;
				break;
			}
			case TKIND_COCLASS:
			{
				printAttributes( out, "COCLASS", typelib, typeinfo, typeattr, indentcnt);
				break;
			}
		}
		const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
	}
	catch (const std::runtime_error& e)
	{
		if (typeattr) const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
		if (reftypeinfo) const_cast<ITypeInfo*>(reftypeinfo)->Release();
		throw e;
	}
}

static void printItem( std::ostream& out, const comauto::TypeLib* typelib, int indentcnt=0)
{
	ITypeInfo* typeinfo = 0;
	try
	{
		UINT ii = 0, nn = const_cast<ITypeLib*>(typelib->typelib())->GetTypeInfoCount();

		for (ii = 0; ii < nn; ++ii)
		{
			WRAP( const_cast<ITypeLib*>(typelib->typelib())->GetTypeInfo( ii, &typeinfo))
			printItem( out, typelib, typeinfo, indentcnt);
			typeinfo->Release();
			typeinfo = 0;
		}
	}
	catch (const std::runtime_error& e)
	{
		if (typeinfo) typeinfo->Release();
		throw e;
	}
}

void comauto::TypeLib::Impl::print( std::ostream& out) const
{
	printItem( out, m_this);
}

void comauto::TypeLib::Impl::printvalue( std::ostream& out, const std::string& name, const VARIANT& val, const ITypeInfo* typeinfo, std::size_t indentcnt) const
{
	VARIANT elem;
	elem.vt = VT_EMPTY;
	VARDESC* vardesc = 0;
	TYPEATTR* typeattr = 0;
	ITypeInfo* rectypeinfo = 0;
	try
	{
		if (typeinfo && val.pRecInfo && val.pvRecord)
		{
			out << std::string(indentcnt,'\t') << name << " {" << std::endl;

			WRAP( const_cast<ITypeInfo*>(typeinfo)->GetTypeAttr( &typeattr))
			UINT ii=0, nn=typeattr->cVars;
			for (; ii < nn; ++ii)
			{
				WRAP( const_cast<ITypeInfo*>(typeinfo)->GetVarDesc( ii, &vardesc))
				std::wstring recname( comauto::variablename_utf16( typeinfo, vardesc));
				val.pRecInfo->GetField( val.pvRecord, recname.c_str(), &elem);

				if (vardesc->elemdescVar.tdesc.vt == VT_USERDEFINED)
				{
					WRAP( elem.pRecInfo->GetTypeInfo( &rectypeinfo))
					printvalue( out, comauto::utf8string(recname), elem, rectypeinfo, indentcnt+1);
					rectypeinfo->Release();
					rectypeinfo = 0;
				}
				else
				{
					printvalue( out, comauto::utf8string(recname), elem, 0, indentcnt+1);
				}
				if (elem.vt != VT_EMPTY)
				{
					comauto::wrapVariantClear( &elem);
					elem.vt = VT_EMPTY;
				}
				const_cast<ITypeInfo*>(typeinfo)->ReleaseVarDesc( vardesc);
				vardesc = 0;
			}
			out << std::string(indentcnt,'\t') << "}" << std::endl;
		}
		else if (comauto::isAtomicType(val.vt) || comauto::isStringType(val.vt))
		{
			std::string elembuf;
			out << std::string(indentcnt,'\t') << name << " : " << comauto::typestr(val.vt) << " '" << comauto::getAtomicElement( val, elembuf).tostring() << "'" << std::endl;
		}
		else
		{
			throw std::runtime_error( "print not implemented for this type value");
		}
	}
	catch (const std::runtime_error& e)
	{
		if (vardesc) const_cast<ITypeInfo*>(typeinfo)->ReleaseVarDesc( vardesc);
		if (rectypeinfo) rectypeinfo->Release();
		if (typeattr) const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
		throw e;
	}
}

const IRecordInfo* comauto::TypeLib::Impl::getRecordInfo( const ITypeInfo* typeinfo) const
{
	LCID lcid_US = 0x0409;
	IRecordInfo* rt = NULL;
	TYPEATTR* typeattr = NULL;
	if (!typeinfo) return 0;
	try
	{
		WRAP( const_cast<ITypeInfo*>(typeinfo)->GetTypeAttr( &typeattr))
		ULONG uVerMajor = m_libattr->wMajorVerNum;
		ULONG uVerMinor = m_libattr->wMinorVerNum;
		if (typeattr->typekind == TKIND_RECORD)
		{
			WRAP( ::GetRecordInfoFromGuids( m_libattr->guid, uVerMajor, uVerMinor, lcid_US, typeattr->guid, &rt))
			const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
			if (!rt) throw std::runtime_error( std::string("record info not defined for type '") + comauto::typestr( typeinfo) + "'");
		}
		else
		{
			const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
		}
	}
	catch (...)
	{
		if (typeattr) const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
		return 0;
	}
	return rt;
}

ITypeInfo* comauto::TypeLib::Impl::getProviderInterface() const
{
	return m_providerTypeInfo;
}


comauto::TypeLib::TypeLib( const std::string& file)
	:m_impl( new Impl(file,this))
{}

comauto::TypeLib::~TypeLib()
{
	delete m_impl;
}

void comauto::TypeLib::print( std::ostream& out) const
{
	m_impl->print( out);
}

void comauto::TypeLib::printvalue( std::ostream& out, const std::string& name, const tagVARIANT& val, const ITypeInfo* typeinfo, std::size_t indentcnt) const
{
	m_impl->printvalue( out, name, val, typeinfo, indentcnt);
}

const IRecordInfo* comauto::TypeLib::getRecordInfo( const ITypeInfo* typeinfo) const
{
	return m_impl->getRecordInfo( typeinfo);
}

const ITypeLib* comauto::TypeLib::typelib() const
{
	return m_impl->typelib();
}

const tagTLIBATTR* comauto::TypeLib::libattr() const
{
	return m_impl->libattr();
}

ITypeInfo* comauto::TypeLib::getProviderInterface() const
{
	return m_impl->getProviderInterface();
}

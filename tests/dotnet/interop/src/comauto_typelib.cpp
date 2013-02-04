#include "comauto_typelib.hpp"
#include "comauto_utils.hpp"
#include "comauto_function.hpp"
#include "ddl_form.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <cstring>
#include <oaidl.h>
#include <comdef.h>
#include <atlbase.h>
#include <atlcom.h>

using namespace _Wolframe;

comauto::TypeLib::TypeLib( const std::string& file)
	:m_typelib(0),m_libattr(0)
{
	try
	{
		std::wstring ws_type_lib_file = utf16string( file);
		WRAP( ::LoadTypeLibEx( ws_type_lib_file.c_str(), REGKIND_NONE, &m_typelib))
		WRAP( m_typelib->GetLibAttr( &m_libattr))
	}
	catch (const std::runtime_error& e)
	{
		if (m_typelib) m_typelib->Release();
		throw e;
	}

}

comauto::TypeLib::~TypeLib()
{
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
				comauto::Function func( 0, typelib, typeinfo, "", "", ii);
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

void comauto::TypeLib::print( std::ostream& out) const
{
	printItem( out, this);
}

const IRecordInfo* comauto::TypeLib::getRecordInfo( const ITypeInfo* typeinfo) const
{
	LCID lcid_US = 0x0409;
	IRecordInfo* rt = NULL;
	TYPEATTR* typeattr = NULL;
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

comauto::TypeLib::AssignmentClosure::StackElem::StackElem( ITypeInfo* typeinfo_, const IRecordInfo* recinfo_)
	:typeinfo(typeinfo_),typeattr(0),recinfo(recinfo_)
{
	value.vt = VT_EMPTY;
	if (typeinfo)
	{
		if (typeattr->typekind == TKIND_RECORD && !recinfo) throw std::logic_error( "using typeinfo of structure without recordinfo");
		WRAP( typeinfo->GetTypeAttr( &typeattr))
		for (UINT ii=0; ii<typeattr->cVars; ++ii)
		{
			VARDESC* vardesc = NULL;
			try
			{
				WRAP( typeinfo->GetVarDesc( ii, &vardesc))
				keymap[ comauto::variablename( typeinfo, vardesc)] = ii; 
			}
			catch (const std::runtime_error& e)
			{

				if (vardesc) typeinfo->ReleaseVarDesc( vardesc);
				throw e;
			}
		}
		typeinfo->AddRef();
	}
}

comauto::TypeLib::AssignmentClosure::StackElem::StackElem( const StackElem& o)
	:typeinfo(o.typeinfo),typeattr(0),recinfo(o.recinfo),key(o.key),keymap(o.keymap)
{
	value.vt = VT_EMPTY;
	comauto::wrapVariantCopy( &value, &o.value);
	if (typeinfo)
	{
		WRAP( typeinfo->GetTypeAttr( &typeattr));
		typeinfo->AddRef();
	}
}

comauto::TypeLib::AssignmentClosure::StackElem::~StackElem()
{
	if (typeinfo)
	{
		if (typeattr) typeinfo->ReleaseTypeAttr( typeattr);
		typeinfo->Release();
		::VariantClear( &value);
	}
}

std::string comauto::TypeLib::AssignmentClosure::variablepath() const
{
	std::string rt;
	std::vector<StackElem>::const_iterator si = m_stk.begin(), se = m_stk.end();
	for (; si != se; ++si)
	{
		if (!si->key.empty())
		{
			if (!rt.empty()) rt.push_back('.');
			rt.append( si->key);
		}
	}
	return rt;
}

bool comauto::TypeLib::AssignmentClosure::call( VARIANT& output)
{
	VARIANT value;
	value.vt = VT_EMPTY;
	VARDESC* vardesc = 0;
	ITypeInfo* rectypeinfo = 0;
	if (m_stk.empty()) return true;
	langbind::TypedFilterBase::ElementType elemtype;
	langbind::TypedFilterBase::Element elemvalue;

	try
	{
		while (m_input->getNext( elemtype, elemvalue))
		{
AGAIN:
			switch (elemtype)
			{
				case langbind::TypedFilterBase::OpenTag:
				{
					StackElem& cur = m_stk.back();
					if (!cur.key.empty()) throw std::runtime_error("illegal filter input sequence (value instead of open tag expected after attribute)");
					if (elemvalue.type != langbind::TypedFilterBase::Element::string_) throw std::runtime_error( "string expected for tag name");
					if (cur.value.vt != VT_RECORD || cur.value.pvRecord == 0 || cur.value.pRecInfo == 0) throw std::runtime_error( "structure assigned to atomic value"); 
					cur.key = elemvalue.tostring();
					std::map<std::string,int>::const_iterator ki = cur.keymap.find( cur.key);
					if (ki == cur.keymap.end())
					{
						cur.key.clear();
						throw std::runtime_error( std::string( "undefined element '") + elemvalue.tostring() + "'");
					}
					WRAP( cur.typeinfo->GetVarDesc( ki->second, &vardesc))
					if (vardesc->elemdescVar.tdesc.vt == VT_USERDEFINED)
					{
						WRAP( cur.typeinfo->GetRefTypeInfo( vardesc->elemdescVar.tdesc.hreftype, &rectypeinfo))
						const IRecordInfo* recinfo = m_typelib->getRecordInfo( rectypeinfo);
						m_stk.push_back( StackElem( rectypeinfo, recinfo));
						rectypeinfo->Release();
						rectypeinfo = 0;
					}
					else
					{
						m_stk.push_back( StackElem());
					}
					m_stk.back().typeinfo->ReleaseVarDesc( vardesc);
					vardesc = 0;
					break;
				}
				case langbind::TypedFilterBase::CloseTag:
				{
					if (!m_stk.back().key.empty()) throw std::runtime_error("illegal filter input sequence (value instead of close tag expected after attribute)");
					value = m_stk.back().value;
					m_stk.back().value.vt = VT_EMPTY;
					m_stk.pop_back();
					if (m_stk.empty())
					{
						output = value;
						if (m_outtype != VT_USERDEFINED && m_outtype != output.vt)
						{
							comauto::wrapVariantChangeType( &output, &output, 0, m_outtype);
						}
						return true;
					}
					StackElem& cur = m_stk.back();
					std::wstring kk( comauto::utf16string( cur.key));
					WRAP( cur.value.pRecInfo->PutField( INVOKE_PROPERTYPUTREF, cur.value.pvRecord, kk.c_str(), &value));
					comauto::wrapVariantClear( &value);
					cur.key.clear();
					break;
				}
				case langbind::TypedFilterBase::Attribute:
				{
					StackElem& cur = m_stk.back();
					if (!cur.key.empty()) throw std::runtime_error("illegal filter input sequence (value instead of attribute expected after attribute)");
					if (elemvalue.type != langbind::TypedFilterBase::Element::string_) throw std::runtime_error( "string expected for attribute name");
					cur.key = elemvalue.tostring();
					break;
				}
				case langbind::TypedFilterBase::Value:
				{
					StackElem& cur = m_stk.back();
					if (!cur.key.empty())
					{
						value = comauto::createVariantType( elemvalue);
						if (cur.value.vt != VT_RECORD || cur.value.pvRecord == 0 || cur.value.pRecInfo == 0) throw std::runtime_error( "illegal state (structure element context expected)"); 
						std::wstring kk( comauto::utf16string( cur.key));
						WRAP( cur.value.pRecInfo->PutField( INVOKE_PROPERTYPUTREF, cur.value.pvRecord, kk.c_str(), &value));
						comauto::wrapVariantClear( &value);
						cur.key.clear();
					}
					else if (cur.value.vt == VT_RECORD)
					{
						cur.key = "_";
						goto AGAIN;
					}
					else
					{
						if (cur.value.vt != VT_EMPTY) throw std::runtime_error( "duplicate value assignment");
						cur.value = comauto::createVariantType( elemvalue);
					}
					break;
				}
			}
			if (m_single)
			{
				if (elemtype != langbind::TypedFilterBase::Value) throw std::runtime_error( "atomic value expected");
				break;
			}
		}
		if (m_input->state() == langbind::InputFilter::Open)
		{
			if (m_stk.size() != 1) throw std::runtime_error( "tags not balanced in input");
			output = m_stk.back().value;
			m_stk.back().value.vt = VT_EMPTY;
			m_stk.pop_back();
			if (m_outtype != VT_USERDEFINED && m_outtype != output.vt)
			{
				comauto::wrapVariantChangeType( &output, &output, 0, m_outtype);
			}
			return true;
		}
		return false;
	}
	catch (const std::runtime_error& e)
	{
		comauto::wrapVariantClear( &value);
		if (rectypeinfo) rectypeinfo->Release();
		if (vardesc) m_stk.back().typeinfo->ReleaseVarDesc( vardesc);
		throw e;
	}
}

comauto::TypeLib::AssignmentClosure::AssignmentClosure()
	:m_typelib(0),m_typeinfo(0),m_outtype(VT_EMPTY),m_single(false)
{}

comauto::TypeLib::AssignmentClosure::AssignmentClosure( const TypeLib* typelib_, const langbind::TypedInputFilterR& input_, VARTYPE outtype_, bool single_)
	:m_typelib(const_cast<TypeLib*>(typelib_)),m_typeinfo(0),m_input(input_),m_outtype(outtype_),m_single(single_)
{
	m_stk.push_back( StackElem());
}

comauto::TypeLib::AssignmentClosure::AssignmentClosure( const TypeLib* typelib_, const langbind::TypedInputFilterR& input_, const ITypeInfo* typeinfo_)
	:m_typelib(const_cast<TypeLib*>(typelib_)),m_typeinfo( const_cast<ITypeInfo*>(typeinfo_)),m_input(input_),m_outtype(VT_USERDEFINED),m_single(false)
{
	const IRecordInfo* recinfo = m_typelib->getRecordInfo( m_typeinfo);
	m_stk.push_back( StackElem( m_typeinfo, recinfo));
}


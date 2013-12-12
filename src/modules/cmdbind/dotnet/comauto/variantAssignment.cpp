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
#include "comauto/variantAssignment.hpp"
#include "comauto/typelib.hpp"
#include "comauto/utils.hpp"
#include <oaidl.h>
#include <comdef.h>
#include <atlcom.h>
#include <atlbase.h>

using namespace _Wolframe;

class comauto::AssignmentClosure::Impl
{
public:
	Impl();
	Impl( const TypeLib* typelib_, const langbind::TypedInputFilterR& input_, VARTYPE outtype, bool single_);
	Impl( const TypeLib* typelib_, const langbind::TypedInputFilterR& input_, const ITypeInfo* typeinfo_);
	virtual ~Impl(){}

	bool call( VARIANT& value);
	std::string variablepath() const;
	const IRecordInfo* recinfo() const		{return m_recinfo;}

private:
	struct StackElem
	{
		ITypeInfo* typeinfo;
		TYPEATTR* typeattr;
		const IRecordInfo* recinfo;
		VARTYPE vt;
		VARIANT value;
		std::string key;
		std::map<std::string,int> keymap;
		std::map<std::size_t,std::vector<VARIANT> > elemar;

		StackElem( ITypeInfo* typeinfo_, const IRecordInfo* recinfo_, VARTYPE vt_);
		StackElem( VARTYPE vt_);
		StackElem( const StackElem& o);
		~StackElem();
	};

private:
	const TypeLib* m_typelib;
	const ITypeInfo* m_typeinfo;
	const IRecordInfo* m_recinfo;
	std::vector<StackElem> m_stk;
	langbind::TypedInputFilterR m_input;
	VARTYPE m_outtype;
	bool m_single;
};

comauto::AssignmentClosure::Impl::StackElem::StackElem( ITypeInfo* typeinfo_, const IRecordInfo* recinfo_, VARTYPE vt_)
	:typeinfo(typeinfo_),typeattr(0),recinfo(recinfo_),vt(vt_)
{
	if (!typeinfo) throw std::logic_error( "illegal state in AssignmentClosure::StackElem");
	VARDESC* vardesc = NULL;
	value.vt = VT_EMPTY;
	try
	{
		WRAP( typeinfo->GetTypeAttr( &typeattr))
		if (typeattr->typekind == TKIND_RECORD && !recinfo)
		{
			throw std::logic_error( "using typeinfo of structure without recordinfo");
		}
		for (UINT ii=0; ii<typeattr->cVars; ++ii)
		{
			WRAP( typeinfo->GetVarDesc( ii, &vardesc))
			keymap[ comauto::variablename( typeinfo, vardesc)] = ii; 
			typeinfo->ReleaseVarDesc( vardesc);
			vardesc = 0;
		}
		value.pRecInfo = const_cast<IRecordInfo*>(recinfo);
		value.pvRecord = value.pRecInfo->RecordCreate();
		value.vt = vt;
		typeinfo->AddRef();
	}
	catch (const std::runtime_error& e)
	{
		if (typeattr) typeinfo->ReleaseTypeAttr( typeattr);
		if (vardesc) typeinfo->ReleaseVarDesc( vardesc);
		throw e;
	}
}

comauto::AssignmentClosure::Impl::StackElem::StackElem( VARTYPE vt_)
	:typeinfo(0),typeattr(0),recinfo(0),vt(vt_)
{
	value.vt = VT_EMPTY;
}

comauto::AssignmentClosure::Impl::StackElem::StackElem( const StackElem& o)
	:typeinfo(o.typeinfo),typeattr(0),recinfo(o.recinfo),vt(o.vt),key(o.key),keymap(o.keymap),elemar(o.elemar)
{
	value.vt = VT_EMPTY;
	comauto::wrapVariantCopy( &value, &o.value);
	if (typeinfo)
	{
		WRAP( typeinfo->GetTypeAttr( &typeattr));
		typeinfo->AddRef();
	}
}

comauto::AssignmentClosure::Impl::StackElem::~StackElem()
{
	if (typeinfo)
	{
		if (typeattr) typeinfo->ReleaseTypeAttr( typeattr);
		typeinfo->Release();
		comauto::wrapVariantClear( &value);
	}
}

std::string comauto::AssignmentClosure::Impl::variablepath() const
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

bool comauto::AssignmentClosure::Impl::call( VARIANT& output)
{
	VARIANT value;
	value.vt = VT_EMPTY;
	ITypeInfo* rectypeinfo = 0;
	VARDESC* vardesc = 0;
	if (m_stk.empty()) return true;
	langbind::FilterBase::ElementType elemtype;
	types::VariantConst elemvalue;
	StackElem* cur = 0;

	try
	{
		while (m_input->getNext( elemtype, elemvalue))
		{
AGAIN:
			cur = &m_stk.back();
			switch (elemtype)
			{
				case langbind::FilterBase::OpenTag:
				{
					if (!cur->key.empty()) throw std::runtime_error("illegal filter input sequence (value instead of open tag expected after attribute)");
					if (elemvalue.type() != types::Variant::String) throw std::runtime_error( "string expected for tag name");
					if (cur->value.vt != VT_RECORD || cur->value.pvRecord == 0 || cur->value.pRecInfo == 0) throw std::runtime_error( "structure assigned to atomic value or array"); 
					cur->key = elemvalue.tostring();
					std::map<std::string,int>::const_iterator ki = cur->keymap.find( cur->key);
					if (ki == cur->keymap.end())
					{
						cur->key.clear();
						throw std::runtime_error( std::string( "undefined element '") + elemvalue.tostring() + "'");
					}
					WRAP( cur->typeinfo->GetVarDesc( ki->second, &vardesc))
					VARTYPE elemvartype = vardesc->elemdescVar.tdesc.vt;
					HREFTYPE elemhreftype = vardesc->elemdescVar.tdesc.hreftype;

					if (elemvartype == VT_SAFEARRAY)
					{
						VARTYPE arelemvartype = vardesc->elemdescVar.tdesc.lptdesc->vt;
						HREFTYPE arelemhreftype = vardesc->elemdescVar.tdesc.lptdesc->hreftype;
						if (arelemvartype == VT_USERDEFINED)
						{
							WRAP( cur->typeinfo->GetRefTypeInfo( arelemhreftype, &rectypeinfo))
							const IRecordInfo* recinfo = m_typelib->getRecordInfo( rectypeinfo);
							m_stk.push_back( StackElem( rectypeinfo, recinfo, VT_RECORD));
							rectypeinfo->Release();
							rectypeinfo = 0;
						}
						else
						{
							m_stk.push_back( StackElem( arelemvartype));
						}
						cur->typeinfo->ReleaseVarDesc( vardesc);
						vardesc = 0;
					}
					else
					{
						cur->typeinfo->ReleaseVarDesc( vardesc);
						vardesc = 0;

						if (elemvartype == VT_USERDEFINED)
						{
							WRAP( cur->typeinfo->GetRefTypeInfo( elemhreftype, &rectypeinfo))
							const IRecordInfo* recinfo = m_typelib->getRecordInfo( rectypeinfo);
							m_stk.push_back( StackElem( rectypeinfo, recinfo, VT_RECORD));
							rectypeinfo->Release();
							rectypeinfo = 0;
						}
						else if (comauto::isAtomicType(elemvartype) || comauto::isStringType(elemvartype))
						{
							m_stk.push_back( StackElem( elemvartype));
						}
						else
						{
							throw std::runtime_error( std::string("cannot pass this type as parameter: '") + comauto::typestr(elemvartype) + "'");
						}
					}
					break;
				}
				case langbind::FilterBase::CloseTag:
				{
					std::map<std::size_t,std::vector<VARIANT> >::iterator ei = cur->elemar.begin(), ee = cur->elemar.end();
					for (; ei != ee; ++ei)
					{
						WRAP( cur->typeinfo->GetVarDesc( ei->first, &vardesc))
						std::wstring key( comauto::variablename_utf16( m_stk.back().typeinfo, vardesc));

						if (vardesc->elemdescVar.tdesc.vt == VT_SAFEARRAY)
						{
							VARTYPE avtype = vardesc->elemdescVar.tdesc.lptdesc->vt;
							HREFTYPE avhreftype = vardesc->elemdescVar.tdesc.lptdesc->hreftype;
							if (avtype == VT_USERDEFINED)
							{
								WRAP( cur->typeinfo->GetRefTypeInfo( avhreftype, &rectypeinfo))
								const IRecordInfo* avrecinfo = m_typelib->getRecordInfo( rectypeinfo);
								value = createVariantArray( VT_RECORD, avrecinfo, ei->second);
								rectypeinfo->Release();
								rectypeinfo = 0;
								WRAP( cur->value.pRecInfo->PutField( INVOKE_PROPERTYPUT, cur->value.pvRecord, key.c_str(), &value))
							}
							else
							{
								value = createVariantArray( avtype, 0, ei->second);
								WRAP( cur->value.pRecInfo->PutField( INVOKE_PROPERTYPUT, cur->value.pvRecord, key.c_str(), &value))
							}
						}
						else if (ei->second.size() > 1)
						{
							throw std::runtime_error( "duplicate assignment to non array variable");
						}
						else
						{
							WRAP( cur->value.pRecInfo->PutField( INVOKE_PROPERTYPUT, cur->value.pvRecord, key.c_str(), &ei->second[0]))
						}
						cur->typeinfo->ReleaseVarDesc( vardesc);
						vardesc = 0;
						comauto::wrapVariantClear( &value);
						value.vt = VT_EMPTY;
					}
					if (!m_stk.back().key.empty()) throw std::runtime_error( "illegal filter input sequence (value instead of close tag expected after attribute)");
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
					cur = &m_stk.back();
					cur->elemar[ cur->keymap[ cur->key]].push_back( value);
					value.vt = VT_EMPTY;
					cur->key.clear();
					break;
				}
				case langbind::FilterBase::Attribute:
				{
					if (!cur->key.empty()) throw std::runtime_error("illegal filter input sequence (value instead of attribute expected after attribute)");
					if (elemvalue.type() != types::Variant::String) throw std::runtime_error( "string expected for attribute name");
					cur->key = elemvalue.tostring();
					break;
				}
				case langbind::FilterBase::Value:
				{
					if (!cur->key.empty())
					{
						value = comauto::createVariantType( elemvalue);
						if (cur->value.vt != VT_RECORD || cur->value.pvRecord == 0 || cur->value.pRecInfo == 0) throw std::runtime_error( "illegal state (structure element context expected)"); 
						cur->elemar[ cur->keymap[ cur->key]].push_back( value);
						value.vt = VT_EMPTY;
						cur->key.clear();
					}
					else if (cur->value.vt == VT_RECORD)
					{
						cur->key = "_";
						goto AGAIN;
					}
					else
					{
						if (cur->value.vt != VT_EMPTY) throw std::runtime_error( "duplicate value assignment");
						cur->value = comauto::createVariantType( elemvalue, cur->vt);
					}
					break;
				}
			}
			if (m_single)
			{
				if (elemtype != langbind::FilterBase::Value) throw std::runtime_error( "atomic value expected");
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
		if (vardesc) cur->typeinfo->ReleaseVarDesc( vardesc);
		comauto::wrapVariantClear( &value);
		if (rectypeinfo) rectypeinfo->Release();
		throw e;
	}
}

comauto::AssignmentClosure::Impl::Impl()
	:m_typelib(0),m_typeinfo(0),m_recinfo(0),m_outtype(VT_EMPTY),m_single(false)
{}

comauto::AssignmentClosure::Impl::Impl( const TypeLib* typelib_, const langbind::TypedInputFilterR& input_, VARTYPE outtype_, bool single_)
	:m_typelib(const_cast<TypeLib*>(typelib_)),m_typeinfo(0),m_recinfo(0),m_input(input_),m_outtype(outtype_),m_single(single_)
{
	m_stk.push_back( StackElem( outtype_));
}

comauto::AssignmentClosure::Impl::Impl( const TypeLib* typelib_, const langbind::TypedInputFilterR& input_, const ITypeInfo* typeinfo_)
	:m_typelib(const_cast<TypeLib*>(typelib_)),m_typeinfo( const_cast<ITypeInfo*>(typeinfo_)),m_recinfo(0),m_input(input_),m_outtype(VT_USERDEFINED),m_single(false)
{
	m_recinfo = m_typelib->getRecordInfo( m_typeinfo);
	m_stk.push_back( StackElem( const_cast<ITypeInfo*>(m_typeinfo), m_recinfo, VT_RECORD));
}

comauto::AssignmentClosure::AssignmentClosure()
	:m_impl(new Impl())
{}

comauto::AssignmentClosure::AssignmentClosure( const TypeLib* typelib_, const langbind::TypedInputFilterR& input_, unsigned short/*VARTYPE*/ outtype, bool single_)
	:m_impl( new Impl( typelib_, input_, outtype, single_))
{}

comauto::AssignmentClosure::AssignmentClosure( const TypeLib* typelib_, const langbind::TypedInputFilterR& input_, const ITypeInfo* typeinfo_)
	:m_impl( new Impl( typelib_, input_, typeinfo_))
{}

comauto::AssignmentClosure::~AssignmentClosure()
{
	delete m_impl;
}

bool comauto::AssignmentClosure::call( VARIANT& value)
{
	return m_impl->call( value);
}

std::string comauto::AssignmentClosure::variablepath() const
{
	return m_impl->variablepath();
}

const IRecordInfo* comauto::AssignmentClosure::recinfo() const
{
	return m_impl->recinfo();
}


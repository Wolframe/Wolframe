/************************************************************************
 Copyright (C) 2011 - 2014 Project Wolframe.
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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
//\file comauto/variantInputFilter.hpp
//\brief InputFilter implementation for MSDN variant type
#include "comauto/variantInputFilter.hpp"
#include "comauto/typelib.hpp"
#include "comauto/utils.hpp"
#include <comdef.h>

using namespace _Wolframe;
using namespace _Wolframe::comauto;

VariantInputFilter::VariantInputFilter( const comauto::TypeLib* typelib_, const ITypeInfo* typeinfo_, VARIANT data_, serialize::Context::Flags flags_)
	:utils::TypeSignature( "comauto::VariantInputFilter", __LINE__)
	,m_typelib(typelib_)
	,m_flags(flags_)
	,m_done(false)
{
	m_stk.push_back( StackElem( "", m_typelib->getRecordInfo(typeinfo_), const_cast<ITypeInfo*>(typeinfo_), data_));
}

VariantInputFilter::VariantInputFilter( const VariantInputFilter& o)
	:utils::TypeSignature( "comauto::VariantInputFilter", __LINE__)
	,m_stk(o.m_stk)
	,m_elembuf(o.m_elembuf)
	,m_typelib(o.m_typelib)
	,m_done(o.m_done)
{}

VariantInputFilter::~VariantInputFilter()
{}

langbind::TypedInputFilter* VariantInputFilter::copy() const
{
	return new VariantInputFilter( *this);
}

VariantInputFilter::StackElem::StackElem( const StackElem& o)
	:name(o.name),state(o.state),recinfo(o.recinfo),typeinfo(o.typeinfo),typeattr(o.typeattr),idx(o.idx)
{
	try
	{
		data.vt = VT_EMPTY;
		WRAP( comauto::wrapVariantCopy( &data, &o.data));
		if (typeinfo)
		{
			typeinfo->AddRef();
			WRAP( typeinfo->GetTypeAttr( &typeattr));
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

VariantInputFilter::StackElem::StackElem( const std::string& name_, const IRecordInfo* recinfo_, ITypeInfo* typeinfo_, VARIANT data_)
	:name(name_),state(VarOpen),recinfo(recinfo_),typeinfo(typeinfo_),typeattr(0),data(data_),idx(0)
{
	try
	{
		if (typeinfo)
		{
			typeinfo->AddRef();
			WRAP( typeinfo->GetTypeAttr( &typeattr));
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

VariantInputFilter::StackElem::~StackElem()
{
	if (typeinfo)
	{
		if (typeattr) typeinfo->ReleaseTypeAttr( typeattr);
		typeinfo->Release();
	}
}

bool VariantInputFilter::getNext( ElementType& type, types::VariantConst& element)
{
AGAIN:
	VARIANT data;
	data.vt = VT_EMPTY;
	VARDESC* vardesc = 0;
	ITypeInfo* reftypeinfo = 0;
	try
	{
		if (m_stk.empty())
		{
			if (!m_done)
			{
				element = types::VariantConst();
				type = CloseTag;
				m_done = true;
				return true;
			}
			return false;
		}
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
						WRAP( ::SafeArrayGetElement( cur.data.parray, &idx, const_cast<void*>(comauto::arithmeticTypeAddress( &data))));
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
								WRAP( ::SafeArrayGetElement( cur.data.parray, &idx, &V_LPSTR( const_cast<VARIANT*>(&data))));
								break;
							case VT_LPWSTR:
								WRAP( ::SafeArrayGetElement( cur.data.parray, &idx, &V_LPWSTR( const_cast<VARIANT*>(&data))));
								break;
							case VT_BSTR:
								WRAP( ::SafeArrayGetElement( cur.data.parray, &idx, V_BSTR( const_cast<VARIANT*>(&data))));
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
						WRAP( ::SafeArrayGetRecordInfo( cur.data.parray, &data.pRecInfo));
						if (!data.pRecInfo) throw std::runtime_error( "cannot iterate on result structure without record info");
						data.pvRecord = data.pRecInfo->RecordCreate();
						WRAP( ::SafeArrayGetElement( cur.data.parray, &idx, const_cast<void*>(data.pvRecord)));
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
					WRAP( cur.typeinfo->GetVarDesc( idx, &vardesc));
					std::wstring varname( comauto::variablename_utf16( cur.typeinfo, vardesc));
					cur.typeinfo->ReleaseVarDesc( vardesc);
					vardesc = 0;

					WRAP( const_cast<IRecordInfo*>(cur.recinfo)->GetField( cur.data.pvRecord, varname.c_str(), &data));
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
		if (vardesc) m_stk.back().typeinfo->ReleaseVarDesc( vardesc);
		if (reftypeinfo) reftypeinfo->Release();
		throw e;
	}
	return false;
}



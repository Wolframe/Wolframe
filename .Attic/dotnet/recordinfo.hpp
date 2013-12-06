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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
#ifndef _Wolframe_COM_AUTOMATION_RECORDINFO_HPP_INCLUDED
#define _Wolframe_COM_AUTOMATION_RECORDINFO_HPP_INCLUDED
#error DEPRECATED
#include <oaidl.h>
#include <comdef.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <windef.h>
#include <map>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace comauto {

class RecordInfo
	:public CComObjectRoot
	,public IRecordInfo
{
public:
	RecordInfo( ITypeInfo* typeinfo_);
	virtual ~RecordInfo();

	// Implementation of the IUnknown interface:
	virtual ULONG STDMETHODCALLTYPE AddRef()		{return InternalAddRef();}
	virtual ULONG STDMETHODCALLTYPE Release()		{return InternalRelease();}
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID* ppvObj);

	// Implementation of the IRecordInfo interface:
	virtual HRESULT STDMETHODCALLTYPE RecordInit( PVOID pvNew);
	virtual HRESULT STDMETHODCALLTYPE RecordClear( PVOID pvExisting);
	virtual HRESULT STDMETHODCALLTYPE RecordCopy( PVOID pvExisting, PVOID pvNew);
	virtual HRESULT STDMETHODCALLTYPE GetGuid( GUID* pguid);
	virtual HRESULT STDMETHODCALLTYPE GetName( BSTR* pbstrName);
	virtual HRESULT STDMETHODCALLTYPE GetSize( ULONG* pcbSize);
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( ITypeInfo** ppTypeInfo);
	virtual HRESULT STDMETHODCALLTYPE GetField( PVOID pvData, LPCOLESTR szFieldName, VARIANT* pvarField);
	virtual HRESULT STDMETHODCALLTYPE GetFieldNoCopy( PVOID pvData, LPCOLESTR szFieldName, VARIANT* pvarField, PVOID* ppvDataCArray);
	virtual HRESULT STDMETHODCALLTYPE PutField( ULONG wFlags, PVOID pvData, LPCOLESTR szFieldName, VARIANT* pvarField);
	virtual HRESULT STDMETHODCALLTYPE PutFieldNoCopy( ULONG wFlags, PVOID pvData, LPCOLESTR szFieldName, VARIANT* pvarField);
	virtual HRESULT STDMETHODCALLTYPE GetFieldNames( ULONG* pcNames, BSTR* rgBstrNames);
	virtual BOOL STDMETHODCALLTYPE IsMatchingType( IRecordInfo *pRecordInfo);
	virtual PVOID STDMETHODCALLTYPE RecordCreate();
	virtual HRESULT STDMETHODCALLTYPE RecordCreateCopy( PVOID pvSource, PVOID* ppvDest);
	virtual HRESULT STDMETHODCALLTYPE RecordDestroy( PVOID pvRecord);

	const ITypeInfo* typeinfo() const	{return m_typeinfo;}
	const TYPEATTR* typeattr() const	{return m_typeattr;}
	PVOID createRecord();

	struct VariableDescriptor
	{
		VARTYPE type;
		std::size_t ofs;
		int varnum;
	};
	bool getVariableDescriptor( const std::string& name, VariableDescriptor& descr) const;

private:
	enum InitType {ClearInit,CopyInit,DefaultConstructor,DefaultConstructorZero,CopyConstructor,Destructor};
	static const char* initTypeName( InitType it)
	{
		static const char* ar[] = {"ClearInit","CopyInit","DefaultConstructor","DefaultConstructorZero","CopyConstructor","Destructor"}; 
		return ar[(int)it];
	}
	HRESULT RecordFill( PVOID pvNew, InitType initType, bool doThrow, PVOID pvOld=0);
	void initDescr();

private:
	ITypeInfo* m_typeinfo;
	TYPEATTR* m_typeattr;
	typedef std::map<std::string,VariableDescriptor> VariableDescriptorMap;
	VariableDescriptorMap m_descrmap;
};

typedef boost::shared_ptr<RecordInfo> RecordInfoR;

}}//namespace
#endif



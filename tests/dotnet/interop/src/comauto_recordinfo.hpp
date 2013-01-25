#ifndef _Wolframe_COM_AUTOMATION_RECORDINFO_HPP_INCLUDED
#define _Wolframe_COM_AUTOMATION_RECORDINFO_HPP_INCLUDED
#include <oaidl.h>
#include <comdef.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <windef.h>

namespace comauto
{

class RecordInfo
	:public CComObjectRoot
	,public IRecordInfo
{
public:
	RecordInfo( ITypeInfo* typeinfo_);
	virtual ~RecordInfo();

	// Implementation of the IUnknown interface:
	virtual ULONG STDMETHODCALLTYPE AddRef()  {return InternalAddRef();}
	virtual ULONG STDMETHODCALLTYPE Release()  {return InternalRelease();}
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

private:
	enum InitType {ClearInit,CopyInit,DefaultConstructor,DefaultConstructorZero,CopyConstructor,Destructor};
	HRESULT RecordFill( PVOID pvNew, InitType initType, PVOID pvOld=0);

private:
	ITypeInfo* m_typeinfo;
	TYPEATTR* m_typeattr;
};

}//namespace
#endif



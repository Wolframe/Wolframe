#ifndef _Wolframe_COM_AUTOMATION_TYPELIB_HPP_INCLUDED
#define _Wolframe_COM_AUTOMATION_TYPELIB_HPP_INCLUDED
#include "comauto_utils.hpp"
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

class TypeLib
{
public:
	explicit TypeLib( const std::string& file);
	virtual ~TypeLib();
	void print( std::ostream& out) const;
	void printvalue( std::ostream& out, const std::string& name, const VARIANT& val, const ITypeInfo* typeinfo, std::size_t indentcnt=0) const;

	const IRecordInfo* getRecordInfo( const ITypeInfo* typeinfo) const;

	const ITypeLib* typelib() const			{return m_typelib;}
	const TLIBATTR* libattr() const			{return m_libattr;}

	class AssignmentClosure
	{
	public:
		AssignmentClosure();
		AssignmentClosure( const TypeLib* typelib_, const langbind::TypedInputFilterR& input_, VARTYPE outtype, bool single_);
		AssignmentClosure( const TypeLib* typelib_, const langbind::TypedInputFilterR& input_, const ITypeInfo* typeinfo_);
		virtual ~AssignmentClosure(){}

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

	typedef boost::shared_ptr<AssignmentClosure> AssignmentClosureR;

private:
	ITypeLib* m_typelib;
	TLIBATTR* m_libattr;
};

}} //namespace
#endif


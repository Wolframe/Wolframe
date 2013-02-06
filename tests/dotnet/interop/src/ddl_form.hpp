#ifndef _Wolframe_DDL_FORM_HPP_INCLUDED
#define _Wolframe_DDL_FORM_HPP_INCLUDED
#include <string>
#include <vector>
#include <map>
#include <wchar.h>
#include "filter/typedfilter.hpp"

namespace _Wolframe {
namespace langbind {

class Form
{
public:
	enum Type
	{
		Atomic,
		Struct,
		Array
	};
	typedef std::map<std::string,Form> Map;
	typedef std::vector<Form> Vector;

public:
	explicit Form( const std::string& v)
		:m_type(Atomic),m_value(v){}
	Form( const Form& o)
		:m_type(o.m_type),m_map(o.m_map),m_ar(o.m_ar),m_value(o.m_value){}
	explicit Form( Type type_=Struct)
		:m_type(type_){}

	Form& operator()( const Form& value);
	Form& operator()( const std::string& name, const Form& value);

	const std::string& value() const;
	const wchar_t* wcvalue() const;
	langbind::TypedInputFilterR get() const;

	const Form& operator[]( const std::string& name) const;
	Form& operator[]( const std::string& name);
	const Form& operator[]( std::size_t idx) const;
	Form& operator[]( std::size_t idx);

	Map::const_iterator structbegin() const		{return m_map.begin();}
	Map::const_iterator structend() const		{return m_map.end();}
	Map::iterator structbegin()					{return m_map.begin();}
	Map::iterator structend()					{return m_map.end();}

	Vector::const_iterator arraybegin() const	{return m_ar.begin();}
	Vector::const_iterator arrayend() const		{return m_ar.end();}
	Vector::iterator arraybegin()				{return m_ar.begin();}
	Vector::iterator arrayend()					{return m_ar.end();}

	bool isAtomic() const						{return m_type == Atomic;}
	bool isArray() const						{return m_type == Array;}
	bool isStruct() const						{return m_type == Struct;}
	std::size_t size() const					{return (m_type == Atomic)?m_value.size():((m_type == Array)?m_ar.size():m_map.size());}
	bool empty() const							{return m_type == Struct && m_map.empty();}

private:
	Type m_type;
	Map m_map;
	Vector m_ar;
	std::string m_value;
};


class FormInputFilter
	:public langbind::TypedInputFilter
{
public:
	FormInputFilter( const Form& form);
	FormInputFilter( const FormInputFilter& o);
	virtual ~FormInputFilter(){}

	virtual FormInputFilter* copy() const	{return new FormInputFilter(*this);}

	virtual bool getNext( ElementType& type, Element& element);

private:
	enum TagState
	{
		OpenTagState,
		CloseTagState,
		ValueState
	};

	struct StackElem
	{
		Form::Type type;
		std::string atag;
		Form::Vector::const_iterator aitr;
		Form::Vector::const_iterator aend;
		Form::Map::const_iterator sitr;
		Form::Map::const_iterator send;
		const std::string* ptr;
		TagState state;

		StackElem( const StackElem& o)
			:type(o.type),atag(o.atag),aitr(o.aitr),aend(o.aend),sitr(o.sitr),send(o.send),ptr(o.ptr),state(o.state){}
		StackElem( const Form::Map::const_iterator& itr_, const Form::Map::const_iterator& end_)
			:type(Form::Struct),sitr(itr_),send(end_),ptr(0),state(OpenTagState){}
		StackElem( const std::string& atag_, const Form::Vector::const_iterator& itr_, const Form::Vector::const_iterator& end_)
			:type(Form::Array),atag(atag_),aitr(itr_),aend(end_),ptr(0),state(ValueState){}
		StackElem( const std::string* ptr_)
			:type(Form::Atomic),ptr(ptr_),state(ValueState){}

		bool end() const;
	};

	std::vector<StackElem> m_stk;
};

}}//namespace
#endif

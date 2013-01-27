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
	typedef std::map<std::string,Form> Map;

	Form( const std::string& v)
		:m_value(v){}
	Form( const Form& o)
		:m_map(o.m_map),m_value(o.m_value){}
	Form(){}

	Form& operator()( const std::string& name, const Form& value);
	const std::string& operator*() const;
	const std::string& value() const;
	const wchar_t* wcvalue() const;
	langbind::TypedInputFilterR get() const;

	const Form& operator[]( const std::string& name) const;
	Form& operator[]( const std::string& name);

	Map::const_iterator begin() const	{return m_map.begin();}
	Map::const_iterator end() const		{return m_map.end();}
	Map::iterator begin()				{return m_map.begin();}
	Map::iterator end()					{return m_map.end();}

	bool atomic() const;
	bool empty() const;

private:
	Map m_map;
	std::string m_value;
};


class FormInputFilter
	:public langbind::TypedInputFilter
{
public:
	FormInputFilter( const Form& form)
		:types::TypeSignature("langbind::FormInputFilter", __LINE__)
		,m_tagstate(0)
	{
		m_stk.push_back( StackElem( form.begin(), form.begin()));
	}

	FormInputFilter( const FormInputFilter& o)
		:types::TypeSignature("langbind::FormInputFilter", __LINE__)
		,langbind::TypedInputFilter(o)
		,m_stk(o.m_stk)
		,m_tagstate(o.m_tagstate){}

	virtual ~FormInputFilter(){}

	virtual FormInputFilter* copy() const	{return new FormInputFilter(*this);}

	virtual bool getNext( ElementType& type, Element& element);

private:
	typedef std::pair<Form::Map::const_iterator,Form::Map::const_iterator> StackElem;
	std::vector<StackElem> m_stk;
	int m_tagstate;
};

}}//namespace
#endif

#ifndef _Wolframe_DDL_FORM_HPP_INCLUDED
#define _Wolframe_DDL_FORM_HPP_INCLUDED
#include <string>
#include <map>
#include <wchar.h>

namespace ddl {

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
	const std::string& operator*();
	const std::string& value();
	const wchar_t* wcvalue() const;

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

}//namespace
#endif

#include "ddl_form.hpp"
#include <stdexcept>

using namespace ddl;

Form& Form::operator()( const std::string& name, const Form& value)
{
	m_map[ name] = value;
	return *this;
}
const std::string& Form::operator*()
{
	return m_value;
}
const std::string& Form::value()
{
	return m_value;
}
const wchar_t* Form::wcvalue() const
{
	return std::wstring( m_value.begin(), m_value.end()).c_str();
}

const Form& Form::operator[]( const std::string& name) const
{
	Map::const_iterator fi = m_map.find( name);
	if (fi == m_map.end()) throw std::runtime_error( std::string("element '") + name + "' not found");
	return fi->second;
}
Form& Form::operator[]( const std::string& name)
{
	Map::iterator fi = m_map.find( name);
	if (fi == m_map.end()) throw std::runtime_error( std::string("element '") + name + "' not found");
	return fi->second;
}

bool Form::atomic() const
{
	return m_map.empty();
}

bool Form::empty() const
{
	return m_map.empty() && m_value.empty();
}


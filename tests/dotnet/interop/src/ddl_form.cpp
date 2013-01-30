#include "ddl_form.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

Form& Form::operator()( const std::string& name, const Form& value)
{
	m_map[ name] = value;
	return *this;
}
const std::string& Form::operator*() const
{
	return m_value;
}
const std::string& Form::value() const
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

langbind::TypedInputFilterR Form::get() const
{
	return langbind::TypedInputFilterR( new FormInputFilter( *this));
}

static TypedFilterBase::Element::Type getType( const std::string& val)
{
	std::string::const_iterator vi = val.begin(), ve = val.end();
	TypedFilterBase::Element::Type rt = TypedFilterBase::Element::string_;
	if (vi != ve && *vi == '-')
	{
		++vi;
		if (vi != ve && *vi >= '0' && *vi <= '9') rt = TypedFilterBase::Element::int_;
	}
	else if (vi != ve && *vi >= '0' && *vi <= '9')
	{
		rt = TypedFilterBase::Element::uint_;
	}
	else if (vi != ve && (*vi == 'f' || *vi == 't'))
	{
		if (val == "true") return TypedFilterBase::Element::bool_;
		if (val == "false") return TypedFilterBase::Element::bool_;
	}
	for (++vi; vi != ve; ++vi)
	{
		switch (rt)
		{
			case TypedFilterBase::Element::string_:
			case TypedFilterBase::Element::bool_: return rt;

			case TypedFilterBase::Element::int_:
			case TypedFilterBase::Element::uint_:
				if (*vi >= '0' && *vi <= '9') continue; 
				if (*vi == '.') rt = TypedFilterBase::Element::double_;
				rt = TypedFilterBase::Element::string_;
				break;
			case TypedFilterBase::Element::double_:
				if (*vi >= '0' && *vi <= '9') continue; 
				rt = TypedFilterBase::Element::string_;
				break;
		}
	}
	return rt;
}

static TypedFilterBase::Element getElement( const std::string& val)
{
	switch (getType( val))
	{
		case TypedFilterBase::Element::string_: return TypedFilterBase::Element( val);
		case TypedFilterBase::Element::blob_: return TypedFilterBase::Element( val);
		case TypedFilterBase::Element::bool_: return TypedFilterBase::Element( (bool)(val == "true"));
		case TypedFilterBase::Element::int_: return TypedFilterBase::Element( boost::lexical_cast<int>( val));
		case TypedFilterBase::Element::uint_: return TypedFilterBase::Element( boost::lexical_cast<unsigned int>( val));
		case TypedFilterBase::Element::double_: return TypedFilterBase::Element( boost::lexical_cast<double>( val));
	}
	return TypedFilterBase::Element( val);
}

bool FormInputFilter::getNext( ElementType& type, Element& element)
{
AGAIN:
	if (m_stk.empty()) return false;
	if (m_stk.back().itr == m_stk.back().end)
	{
		m_stk.pop_back();
		goto AGAIN;
	}
	switch (m_stk.back().state)
	{
		case OpenTagState:
			type = FilterBase::OpenTag;
			element = getElement( m_stk.back().itr->first);
			m_stk.back().state = ValueState;
			return true;

		case CloseTagState:
			type = FilterBase::CloseTag;
			element = Element();
			++m_stk.back().itr;
			m_stk.back().state = OpenTagState;
			return true;

		case ValueState:
			type = FilterBase::Value;
			if (m_stk.back().itr->second.atomic())
			{
				element = getElement( m_stk.back().itr->second.value());
				m_stk.back().state = CloseTagState;
				return true;
			}
			else
			{
				m_stk.back().state = CloseTagState;
				m_stk.push_back( StackElem( m_stk.back().itr->second.begin(), m_stk.back().itr->second.end()));
				goto AGAIN;
			}
	}
	throw std::runtime_error( "illegal state in 'FormInputFilter::getNext'");
}


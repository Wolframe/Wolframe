#include "mylangStructure.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;
using namespace _Wolframe::langbind::mylang;

Structure* Structure::addStructElement( const std::string& elemid_)
{
	std::vector<KeyValuePair>::const_iterator si = m_struct.begin(), se = m_struct.end();
	for (; si != se; ++si)
	{
		if (si->first.type() != types::Variant::String) continue;
		if (si->first.tostring() == elemid_)
		{
			throw std::runtime_error( std::string("duplicate definition of element '") + elemid_ + "'");
		}
	}
	m_struct.push_back( KeyValuePair( elemid_, 0));
	return m_struct.back().second = new Structure( m_instance);
}

Structure* Structure::addArrayElement()
{
	std::vector<KeyValuePair>::const_iterator si = m_struct.begin(), se = m_struct.end();
	if (!m_array) for (; si != se; ++si)
	{
		if (si->first.type() == types::Variant::String) throw std::runtime_error( "mixing array with structure");
	}
	m_array = true;
	// add element at end of array:
	unsigned int aridx = m_struct.size()+1;
	m_struct.push_back( KeyValuePair( aridx, 0));
	return m_struct.back().second = new Structure( m_instance);
}

void Structure::setValue( const types::Variant& value_)
{
	if (!value_.defined()) throw std::runtime_error("set undefined value");
	if (m_value.defined()) throw std::runtime_error("value already defined");
	if (!m_struct.empty()) throw std::runtime_error("setValue for atomic value called for a structure");
	m_value = value_;
}

const types::Variant& Structure::getValue() const
{
	if (!m_struct.empty()) throw std::runtime_error("setValue for atomic value called for a structure");
	return m_value;
}

bool Structure::atomic() const
{
	return (m_struct.empty() && m_value.defined());
}

bool Structure::array() const
{
	return m_array;
}

Structure::const_iterator Structure::begin() const
{
	return m_struct.begin();
}

Structure::const_iterator Structure::end() const
{
	return m_struct.end();
}



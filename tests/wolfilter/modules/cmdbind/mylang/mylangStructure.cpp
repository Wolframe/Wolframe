#include "mylangStructure.hpp"
#include <sstream>

using namespace _Wolframe;
using namespace _Wolframe::langbind;
using namespace _Wolframe::langbind::mylang;

int Structure::const_iterator::compare( const const_iterator& o) const
{
	if (!m_st || !o.m_st) throw std::logic_error( "comparing null iterator");
	return m_itr - o.m_itr;
}

Structure::const_iterator::const_iterator()
	:m_st(0)
{
	m_elem.key = types::VariantConst();
	m_elem.val = 0;
}

Structure::const_iterator::const_iterator( const Structure* st_, std::size_t pos_)
	:m_st(st_)
{
	m_itr = m_st->m_struct.begin() + pos_;
	m_end = m_st->m_struct.end();
	m_elem.key = types::VariantConst();
	m_elem.val = 0;
}

Structure::const_iterator::const_iterator( const Structure* st_)
	:m_st(st_)
{
	m_itr = m_st->m_struct.begin();
	m_end = m_st->m_struct.end();
	if (m_itr == m_end)
	{
		m_elem.key = types::VariantConst();
		m_elem.val = 0;
	}
	else
	{
		m_elem.key = m_itr->first;
		m_elem.val = m_itr->second;
	}
}

Structure::const_iterator::const_iterator( const const_iterator& o)
	:m_st(o.m_st),m_itr(o.m_itr),m_end(o.m_end)
{
	m_elem.key = o.m_elem.key;
	m_elem.val = o.m_elem.val;
}

void Structure::const_iterator::fetch_next()
{
	++m_itr;
	if (m_itr >= m_end)
	{
		m_elem.key = types::VariantConst();
		m_elem.val = 0;
	}
	else
	{
		m_elem.key = m_itr->first;
		m_elem.val = m_itr->second;
	}
}


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
	return m_struct.back().second = new Structure();
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
	return m_struct.back().second = new Structure();
}

void Structure::setValue( const types::Variant& value_)
{
	if (!value_.defined()) throw std::runtime_error("set undefined value");
	if (m_value.defined()) throw std::runtime_error("value already defined");
	if (!m_struct.empty()) throw std::runtime_error("setValue for atomic value called for a structure");
	m_value = value_;
}

types::Variant Structure::getValue() const
{
	if (!m_struct.empty()) throw std::runtime_error("setValue for atomic value called for a structure");
	return m_value;
}

bool Structure::atomic() const
{
	return (m_struct.empty() || m_value.defined());
}

bool Structure::array() const
{
	return m_array;
}

unsigned int Structure::lastArrayIndex() const
{
	if (!m_array) return 0;
	return m_struct.size();
}

static void print_newitem( std::ostream& out, const utils::PrintFormat* pformat, std::size_t level)
{
	out << pformat->newitem;
	for (std::size_t ll=0; ll<level; ++ll) out << pformat->indent;
}

void Structure::print( std::ostream& out, const utils::PrintFormat* pformat, std::size_t level) const
{
	if (array())
	{
		std::vector<KeyValuePair>::const_iterator si = m_struct.begin(), se = m_struct.end();
		out << pformat->openstruct;
		int idx = 0;
		for (; si != se; ++si,++idx)
		{
			if (idx) out << pformat->decldelimiter;
			print_newitem( out, pformat, level);
			si->second->print( out, pformat, level+1);
		}
		if (idx>0)
		{
			print_newitem( out, pformat, level);
		}
		out << pformat->closestruct;
	}
	else if (atomic())
	{
		out << pformat->startvalue;
		out << m_value.tostring();
		out << pformat->endvalue;
	}
	else
	{
		std::vector<KeyValuePair>::const_iterator si = m_struct.begin(), se = m_struct.end();
		out << pformat->openstruct;

		int idx = 0;
		for (; si != se; ++si,++idx)
		{
			if (idx) out << pformat->decldelimiter;
			print_newitem( out, pformat, level);

			out << si->first.tostring() << pformat->assign;
			si->second->print( out, pformat, level+1);
		}
		out << pformat->closestruct;
	}
}

std::string Structure::tostring( const utils::PrintFormat* pformat) const
{
	std::ostringstream buf;
	print( buf, pformat, 0);
	return buf.str();
}


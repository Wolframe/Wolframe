#include "mylangStructure.hpp"
#include "mylangStructureBuilder.hpp"
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
		m_elem.val = m_itr->second.get();
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
		m_elem.val = m_itr->second.get();
	}
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
		if (pformat->maxitemsize)
		{
			out << utils::getLogString( m_value, pformat->maxitemsize);
		}
		else
		{
			out << m_value.tostring();
		}
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


#include "types/variantStructDescription.hpp"
#include "types/variantStruct.hpp"
#include <limits>
#include <stdexcept>
#include <cstdlib>
#include <sstream>
#include <stdint.h>	//... for uintptr_t

using namespace _Wolframe;
using namespace _Wolframe::types;

void VariantStructDescription::Element::makeArray()
{
	*initvalue = initvalue->array();
}

Variant::Type VariantStructDescription::Element::type()
{
	return (Variant::Type)initvalue->type();
}

VariantStructDescription::VariantStructDescription()
		:m_size(0),m_ar(0){}

VariantStructDescription::VariantStructDescription( const VariantStructDescription& o)
	:m_size(o.m_size),m_ar(0)
{
	std::size_t ii=0;
	m_ar = (Element*)std::calloc( m_size, sizeof(Element));
	if (!m_ar) goto BAD_ALLOC;
	for (; ii<m_size; ++ii)
	{
		m_ar[ ii].name = (char*)std::malloc( std::strlen( o.m_ar[ ii].name)+1);
		if (!m_ar[ ii].name) goto BAD_ALLOC;
		std::strcpy( m_ar[ ii].name, o.m_ar[ ii].name);
		try
		{
			m_ar[ ii].initvalue = new VariantStruct( *o.m_ar[ ii].initvalue);
		}
		catch (const std::bad_alloc&)
		{
			goto BAD_ALLOC;
		}
		m_ar[ ii].normalizer = o.m_ar[ ii].normalizer;
	}
	BAD_ALLOC:
		if (m_ar)
		{
			for (; ii>0; --ii)
			{
				if (m_ar[ ii-1].name) std::free( m_ar[ ii-1].name);
				if (m_ar[ ii-1].initvalue) delete m_ar[ ii-1].initvalue;
			}
			std::free( m_ar);
		}
		throw std::bad_alloc();
}

int VariantStructDescription::add( const std::string& name_, const VariantStruct& initvalue, const NormalizeFunction* normalizer_)
{
	if ((std::size_t)std::numeric_limits<int>::max() <= m_size+1) throw std::bad_alloc();
	{
		Element* ar_ = (Element*)std::realloc( m_ar, sizeof(Element) * (m_size+1));
		if (!ar_) throw std::bad_alloc();
		m_ar = ar_;
	}
	m_ar[m_size].name = (char*)std::malloc( name_.size() +1);
	if (!m_ar[m_size].name) throw std::bad_alloc();
	std::memcpy( m_ar[ m_size].name, name_.c_str(), name_.size());
	m_ar[ m_size].name[ name_.size()] = 0;
	try
	{
		m_ar[ m_size].initvalue = new VariantStruct( initvalue);
	}
	catch (const std::bad_alloc& e)
	{
		free( m_ar[ m_size].name);
		m_ar[ m_size].name = 0;
		throw e;
	}
	m_ar[ m_size].normalizer = normalizer_;
	m_ar[ m_size].substruct = 0;
	return m_size++;
}

int VariantStructDescription::add( const std::string& name_, const VariantStructDescription& substruct_)
{
	if ((std::size_t)std::numeric_limits<int>::max() <= m_size+1) throw std::bad_alloc();
	{
		Element* ar_ = (Element*)std::realloc( m_ar, sizeof(Element) * (m_size+1));
		if (!ar_) throw std::bad_alloc();
		m_ar = ar_;
	}
	m_ar[m_size].name = (char*)std::malloc( name_.size() +1);
	if (!m_ar[m_size].name) throw std::bad_alloc();
	std::memcpy( m_ar[ m_size].name, name_.c_str(), name_.size());
	m_ar[ m_size].name[ name_.size()] = 0;
	try
	{
		m_ar[m_size].substruct = new VariantStructDescription( substruct_);
	}
	catch (const std::bad_alloc& e)
	{
		free( m_ar[ m_size].name);
		m_ar[ m_size].name = 0;
		throw e;
	}
	try
	{
		m_ar[ m_size].initvalue = new VariantStruct( m_ar[m_size].substruct);
	}
	catch (const std::bad_alloc& e)
	{
		delete m_ar[m_size].substruct;
		free( m_ar[ m_size].name);
		m_ar[ m_size].name = 0;
		throw e;
	}
	m_ar[ m_size].normalizer = 0;
	return m_size++;
}

int VariantStructDescription::addAttribute( const std::string& name_, const VariantStruct& initvalue_, const NormalizeFunction* normalizer_)
{
	int rt = add( name_, initvalue_, normalizer_);
	m_ar[ rt].setAttribute();
	++m_nofattributes;
	if (m_nofattributes < m_size)
	{
		//... insert attribute at its position
		Element attr;
		std::memcpy( &attr, &m_ar[ m_size-1], sizeof( attr));			//... store attribute in 'attr'
		std::size_t mm = (m_size-m_nofattributes) * sizeof(attr);		//... calculate memory to move in the shift operation
		std::memmove( &m_ar[ m_nofattributes-1], &m_ar[ m_nofattributes], mm);	//... shift elements up one position to get the slot to swap 'attr' with
		std::memcpy( &m_ar[ m_nofattributes-1], &attr, sizeof( attr));		//... move 'attr' to the free slot
	}
	return rt;
}

int VariantStructDescription::findidx( const std::string& name_) const
{
	std::size_t ii = 0;
	for (; ii<m_size; ++ii) if (std::strcmp( name_.c_str(), m_ar[ii].name) == 0) return (int)ii;
	return -1;
}

VariantStructDescription::const_iterator VariantStructDescription::find( const std::string& name_) const
{
	int findidx_ = findidx( name_);
	if (findidx_ < 0) return VariantStructDescription::const_iterator();
	return VariantStructDescription::const_iterator( m_ar+findidx_);
}

VariantStructDescription::iterator VariantStructDescription::find( const std::string& name_)
{
	int findidx_ = findidx( name_);
	if (findidx_ < 0) return VariantStructDescription::iterator();
	return VariantStructDescription::iterator( m_ar+findidx_);
}

int VariantStructDescription::const_iterator::compare( const const_iterator& o) const
{
	if (!o.m_itr) return m_itr?-1:0;
	if (!m_itr) return o.m_itr?+1:0;
	if (m_itr > o.m_itr) return +1;
	if (m_itr < o.m_itr) return -1;
	return 0;
}

int VariantStructDescription::iterator::compare( const iterator& o) const
{
	if (!o.m_itr) return m_itr?-1:0;
	if (!m_itr) return o.m_itr?+1:0;
	if (m_itr > o.m_itr) return +1;
	if (m_itr < o.m_itr) return -1;
	return 0;
}

int VariantStructDescription::compare( const VariantStructDescription& o) const
{
	if (this == &o) return 0;
	const_iterator ai = begin(), ae = end();
	const_iterator bi = o.begin(), be = o.end();
	for (; ai != ae && bi != be; ++ai,++bi)
	{
		int cmp = std::strcmp( ai->name, bi->name);
		if (cmp) return cmp;
		const VariantStruct* ap = ai->initvalue;
		const VariantStruct* bp = bi->initvalue;
		if (ap != bp)
		{
			if (ap == 0) return -1;
			if (bp == 0) return +1;
			cmp = ap->compare(*bp);
			if (cmp) return cmp;
		}
		const NormalizeFunction* af = ai->normalizer;
		const NormalizeFunction* bf = bi->normalizer;
		if (ap != bp)
		{
			return (uintptr_t)(af) > (uintptr_t)(bf)?1:-1;
		}
	}
	if (ai != ae) return -1;
	if (bi != be) return +1;
	return 0;
}

std::string VariantStructDescription::names( const std::string& sep) const
{
	std::string rt;
	const_iterator ni = begin(), ne = end();
	for (; ni != ne; ++ni)
	{
		if (rt.size()) rt.append( sep);
		rt.append( ni->name);
	}
}





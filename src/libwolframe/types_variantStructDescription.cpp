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
	if (array()) throw std::logic_error( "illegal operation (make array called on array)");
	flags |= (unsigned char)Array;
	*initvalue = initvalue->array();
}

void VariantStructDescription::Element::copy( const Element& o)
{
	Element elem;
	std::memset( &elem, 0, sizeof( elem));
	std::size_t nn = std::strlen( o.name);
	if (o.name) elem.name = (char*)std::malloc( nn + 1);
	if (!elem.name) throw std::bad_alloc();
	std::memcpy( elem.name, o.name, nn);
	elem.name[ nn] = 0;
	if (o.initvalue) try
	{
		elem.initvalue = new VariantStruct( *o.initvalue);
	}
	catch (const std::bad_alloc& e)
	{
		if (elem.name) free( elem.name);
		throw e;
	}
	if (o.substruct) try
	{
		elem.substruct = new VariantStructDescription( *o.substruct);
	}
	catch (const std::bad_alloc& e)
	{
		if (elem.initvalue) delete elem.initvalue;
		if (elem.name) free( elem.name);
		throw e;
	}
	elem.normalizer = o.normalizer;
	elem.flags = o.flags;

	if (substruct) delete substruct;
	if (initvalue) delete initvalue;
	if (name) delete name;
	std::memcpy( this, &elem, sizeof( elem));
}

Variant::Type VariantStructDescription::Element::type()
{
	return (Variant::Type)initvalue->type();
}

VariantStructDescription::VariantStructDescription()
		:m_size(0),m_nofattributes(0),m_ar(0){}

VariantStructDescription::VariantStructDescription( const VariantStructDescription& o)
	:m_size(o.m_size),m_nofattributes(o.m_nofattributes),m_ar(0)
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

VariantStructDescription::~VariantStructDescription()
{
	std::size_t ii;
	for (ii=0; ii<m_size; ++ii)
	{
		Element* ee = m_ar+ii;
		if (ee->substruct) delete ee->substruct;
		if (ee->initvalue) delete ee->initvalue;
		if (ee->name) delete ee->name;
	}
	if (m_ar) std::free( m_ar);
}

int VariantStructDescription::addAtom( const std::string& name_, const Variant& initvalue, const NormalizeFunction* normalizer_)
{
	if (findidx( name_) < 0) throw std::runtime_error( std::string("tried to add duplicate element '") + name_ + "' to structure description");

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

int VariantStructDescription::addStructure( const std::string& name_, const VariantStructDescription& substruct_)
{
	if (findidx( name_) < 0) throw std::runtime_error( std::string("tried to add duplicate element '") + name_ + "' to structure description");

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

int VariantStructDescription::addIndirection( const std::string& name_, const VariantStructDescription* descr)
{
	return addAtom( name_, VariantIndirection( descr), 0);
}

int VariantStructDescription::addElement( const Element& elem)
{
	if (!elem.name) throw std::runtime_error( "try to add element without name in structure description");
	if (findidx( elem.name) < 0) throw std::runtime_error( std::string("tried to add duplicate element '") + elem.name + "' to structure description");

	if (elem.attribute())
	{
		if (!elem.initvalue) throw std::runtime_error( "try to add incomplete element (null value)");
		return addAttribute( elem.name, *elem.initvalue, elem.normalizer);
	}
	else
	{
		if ((std::size_t)std::numeric_limits<int>::max() <= m_size+1) throw std::bad_alloc();
		Element* ar_ = (Element*)std::realloc( m_ar, sizeof(Element) * (m_size+1));
		if (!ar_) throw std::bad_alloc();
		m_ar = ar_;
		std::memset( m_ar + m_size, 0, sizeof( *m_ar));
		m_ar[ m_size].copy( elem);
		return m_size++;
	}
}

int VariantStructDescription::addAttribute( const std::string& name_, const Variant& initvalue_, const NormalizeFunction* normalizer_)
{
	int rt = addAtom( name_, initvalue_, normalizer_);
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

void VariantStructDescription::inherit( const VariantStructDescription& parent)
{
	const_iterator pi = parent.begin(), pe = parent.end();
	for (; pi != pe; ++pi) addElement( *pi);
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
	return rt;
}





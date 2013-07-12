#include "types/variantStruct.hpp"
#include <limits>
#include <stdexcept>
#include <cstdlib>

using namespace _Wolframe;
using namespace _Wolframe::types;

StructDescription::StructDescription()
		:m_size(0),m_namear(0),m_initvaluear(0){}

StructDescription::StructDescription( const StructDescription& o)
	:m_size(o.m_size),m_namear(0),m_initvaluear(0)
{
	std::size_t ii=0;
	m_namear = (char**)std::calloc( m_size, sizeof(char*));
	m_initvaluear = (VariantStruct**)std::calloc( m_size, sizeof(VariantStruct*));
	if (!m_namear || !m_initvaluear) goto BAD_ALLOC;
	for (; ii<m_size; ++ii)
	{
		m_namear[ ii] = (char*)std::malloc( std::strlen( o.m_namear[ ii])+1);
		if (!m_namear[ ii]) goto BAD_ALLOC;
		std::strcpy( m_namear[ ii], o.m_namear[ ii]);
		try
		{
			m_initvaluear[ ii] = new VariantStruct( *o.m_initvaluear[ ii]);
		}
		catch (const std::bad_alloc&)
		{
			goto BAD_ALLOC;
		}
	}
	BAD_ALLOC:
		if (m_namear)
		{
			for (; ii>0; --ii) std::free( m_namear[ ii-1]);
			std::free( m_namear);
		}
		if (m_initvaluear)
		{
			for (; ii>0; --ii) delete m_initvaluear[ ii-1];
			std::free( m_initvaluear);
		}
		throw std::bad_alloc();
}

void StructDescription::add( const char* name_, const VariantStruct& initvalue)
{
	if ((std::size_t)std::numeric_limits<int>::max() <= m_size+1) throw std::bad_alloc();
	char** ptr = (char**)std::realloc( m_namear, m_size+1);
	VariantStruct** valar = (VariantStruct**)std::realloc( m_initvaluear, m_size+1);
	if (!ptr || !valar) throw std::bad_alloc();
	m_namear = ptr;
	m_namear[ m_size] = (char*)std::malloc( std::strlen( name_)+1);
	if (!m_namear[ m_size]) throw std::bad_alloc();
	std::strcpy( m_namear[ m_size], name_);
	try
	{
		m_initvaluear[ m_size] = new VariantStruct( initvalue);
	}
	catch (const std::bad_alloc& e)
	{
		free( m_namear[ m_size]);
		throw e;
	}
	m_size++;
}

int StructDescription::findidx( const char* name_)
{
	std::size_t ii = 0;
	for (; ii<m_size; ++ii) if (std::strcmp( name_, m_namear[ii]) == 0) return (int)ii;
	return -1;
}

StructDescription::const_iterator StructDescription::find( const char* name_)
{
	int findidx_ = findidx( name_);
	if (findidx_ < 0) return end();
	return StructDescription::const_iterator( *this, findidx_);
}

int StructDescription::const_iterator::compare( const const_iterator& o) const
{
	int oidx = (o.m_visited && o.m_idx < o.m_visited->size())?o.m_idx:std::numeric_limits<int>::max();
	int tidx = (m_visited && m_idx < m_visited->size())?m_idx:std::numeric_limits<int>::max();
	return tidx - oidx;
}

void StructDescription::const_iterator::increment()
{
	if (m_idx >= (std::size_t)std::numeric_limits<int>::max()) throw std::logic_error("array bound read");
	++m_idx;
}

int StructDescription::compare( const StructDescription& o) const
{
	if (this == &o) return 0;
	const_iterator ai = begin(), ae = end();
	const_iterator bi = o.begin(), be = o.end();
	for (; ai != ae && bi != be; ++ai,++bi)
	{
		int cmp = std::strcmp( ai->name(), bi->name());
		if (cmp) return cmp;
		const VariantStruct* ap = ai->prototype();
		const VariantStruct* bp = bi->prototype();
		if (ap != bp)
		{
			if (ap == 0) return -1;
			if (bp == 0) return +1;
			cmp = ap->compare(*bp);
			if (cmp) return cmp;
		}
	}
	if (ai != ae) return -1;
	if (bi != be) return +1;
	return 0;
}

VariantStruct VariantStruct::array() const
{
	VariantStruct* prototype = (VariantStruct*)std::malloc( sizeof( VariantStruct));
	if (!prototype) throw std::bad_alloc();
	try
	{
		VariantStruct::initcopy( *prototype, *this);
	}
	catch (const std::bad_alloc& e)
	{
		std::free( prototype);
		throw e;
	}
	VariantStruct rt;
	rt.setType( array_);
	rt.m_data.dim.size = 0;
	rt.m_data.value.ref_ = prototype;
	return rt;
}

void VariantStruct::initindirection( const StructDescription* descr)
{
	setType( indirection_);
	m_data.dim.metadata = (const void*)descr;
	m_data.value.ref_ = 0;
}

void VariantStruct::expand()
{
	if (type() != indirection_) return;
	initstruct( description());
}

void VariantStruct::initstruct( const StructDescription* descr)
{
	setType( struct_);
	m_data.dim.metadata = (const void*)descr;
	m_data.value.ref_ = (VariantStruct*)std::malloc( descr->size() * sizeof( *this));
	if (!m_data.value.ref_) throw std::bad_alloc();
	StructDescription::const_iterator si = descr->begin(), se = descr->end();
	std::size_t idx=0;
	try
	{
		for (; si!=se; ++si,++idx)
		{
			VariantStruct::initcopy( ((VariantStruct*)m_data.value.ref_)[idx], *si->prototype());
		}
	}
	catch (const std::bad_alloc& e)
	{
		for (; idx>0; --idx) ((VariantStruct*)m_data.value.ref_)[idx-1].release();
		std::free( m_data.value.ref_);
		throw e;
	}
}

void VariantStruct::initcopy( VariantStruct& dest, const VariantStruct& orig)
{
	std::size_t ii, nn;
	switch (orig.type())
	{
		case VariantStruct::bool_:
		case VariantStruct::double_:
		case VariantStruct::int_:
		case VariantStruct::uint_:
		case VariantStruct::string_:
			Variant::initcopy( dest, orig);
			break;

		case VariantStruct::struct_:
			dest.init();
			dest.setType( struct_);
			dest.m_data.dim.metadata = orig.m_data.dim.metadata;
			ii=0; nn=orig.description()->size();
			dest.m_data.value.ref_ = std::malloc( nn * sizeof( dest));
			if (!dest.m_data.value.ref_) throw std::bad_alloc();
			try
			{
				for (; ii<nn; ++ii)
				{
					VariantStruct::initcopy( ((VariantStruct*)dest.m_data.value.ref_)[ ii], ((VariantStruct*)orig.m_data.value.ref_)[ ii]);
				}
			}
			catch (const std::bad_alloc& e)
			{
				for (; ii>0; --ii) ((VariantStruct*)dest.m_data.value.ref_)[ ii-1].release();
				std::free( dest.m_data.value.ref_);
				throw e;
			}
			break;

		case VariantStruct::array_:
			dest.init();
			dest.setType( array_);
			dest.m_data.dim.size = orig.m_data.dim.size;
			dest.m_data.value.ref_ = std::malloc( (orig.m_data.dim.size+1) + sizeof( VariantStruct));
			if (!dest.m_data.value.ref_) throw std::bad_alloc();
			ii=0; nn=orig.data().dim.size+1;
			try
			{
				for (; ii<nn; ++ii)
				{
					VariantStruct::initcopy( ((VariantStruct*)dest.m_data.value.ref_)[ ii], ((VariantStruct*)orig.m_data.value.ref_)[ ii]);
				}
			}
			catch (const std::bad_alloc& e)
			{
				for (; ii>0; --ii) ((VariantStruct*)dest.m_data.value.ref_)[ ii-1].release();
				std::free( dest.m_data.value.ref_);
				throw e;
			}
			break;

		case VariantStruct::indirection_:
			dest.init();
			dest.setType( indirection_);
			dest.m_data.dim.metadata = orig.m_data.dim.metadata;
			break;
	}
}

void VariantStruct::release()
{
	std::size_t ii, nn;
	switch ((Type)m_type)
	{
		case VariantStruct::bool_:
		case VariantStruct::double_:
		case VariantStruct::int_:
		case VariantStruct::uint_:
		case VariantStruct::string_:
			Variant::release();
			break;

		case VariantStruct::struct_:
			ii=0; nn=description()->size();
			for (; ii<nn; ++ii)
			{
				((VariantStruct*)m_data.value.ref_)[ ii].release();
			}
			std::free( m_data.value.ref_);
			break;

		case VariantStruct::array_:
			ii=0; nn=m_data.dim.size+1;
			for (; ii<nn; ++ii)
			{
				((VariantStruct*)m_data.value.ref_)[ ii].release();
			}
			std::free( m_data.value.ref_);
			break;

		case VariantStruct::indirection_:
			break;
	}
}

int VariantStruct::compare_array( std::size_t size, const VariantStruct* a1, const VariantStruct* a2)
{
	std::size_t ii=0;
	for (; ii<size; ++ii)
	{
		int cmp = a1[ii].compare( a2[ii]);
		if (!cmp) return cmp;
	}
	return 0;
}

int VariantStruct::compare( const VariantStruct& o) const
{
	if ((Type)m_type == array_)
	{
		if ((Type)o.m_type != array_) return -2;
		if (m_data.dim.size != o.m_data.dim.size)
		{
			return (int)(m_data.dim.size >= o.m_data.dim.size) + (int)(m_data.dim.size > o.m_data.dim.size) -1;
		}
		return compare_array( m_data.dim.size, (const VariantStruct*)m_data.value.ref_ + 1, (const VariantStruct*)o.m_data.value.ref_ + 1);
		//... ref +1, because prototype is not compared
	}
	else if ((Type)m_type == struct_)
	{
		if ((Type)o.m_type != struct_) return -2;
		int size1 = (int)(description()->size());
		int size2 = (int)(o.description()->size());
		if (size1 != size2) return size1 - size2;
		return compare_array( size1, (const VariantStruct*)m_data.value.ref_, (const VariantStruct*)o.m_data.value.ref_);
	}
	else if ((Type)m_type == indirection_)
	{
		if ((Type)o.m_type != indirection_) return -2;
		const StructDescription* dd = description();
		const StructDescription* od = o.description();
		if (!dd) return 1;
		if (!od) return -2;
		return dd->compare( *od);
	}
	else if ((Type)o.m_type == array_ || (Type)o.m_type == struct_ || (Type)o.m_type == indirection_)
	{
		return +2;
	}
	return Variant::compare( o);
}

const VariantStruct& VariantStruct::operator[]( std::size_t idx) const
{
	if ((Type)m_type == array_)
	{
		if (idx >= m_data.dim.size) throw std::logic_error( "array bound read");
		return *((const VariantStruct*)m_data.value.ref_ + 1 + idx);
	}
	else if ((Type)m_type == struct_)
	{
		if (idx >= description()->size()) throw std::logic_error( "array bound read");
		return *((const VariantStruct*)m_data.value.ref_ + idx);
	}
	else if ((Type)m_type == indirection_)
	{
		throw std::logic_error( "random access not supported for indirection");
	}
	else
	{
		throw std::logic_error( "random access not supported for atomic types");
	}
}

VariantStruct& VariantStruct::operator[]( std::size_t idx)
{
	if ((Type)m_type == array_)
	{
		if (idx >= m_data.dim.size) throw std::logic_error( "array bound read");
		return *((VariantStruct*)m_data.value.ref_ + 1 + idx);
	}
	else if ((Type)m_type == struct_)
	{
		if (idx >= description()->size()) throw std::logic_error( "array bound read");
		return *((VariantStruct*)m_data.value.ref_ + idx);
	}
	else if ((Type)m_type == indirection_)
	{
		throw std::logic_error( "random access not supported for indirection");
	}
	else
	{
		throw std::logic_error( "random access not supported for atomic types");
	}
}

const char* VariantStruct::const_iterator::name() const
{
	if (m_visited->type() != struct_) return 0;
	const StructDescription* descr = m_visited->description();
	if (!descr) return 0;
	return descr->name( m_idx);
}

const VariantStruct* VariantStruct::const_iterator::value() const
{
	if (m_visited->type() != struct_) return 0;
	const StructDescription* descr = m_visited->description();
	if (!descr) return 0;
	return descr->value( m_visited, m_idx);
}


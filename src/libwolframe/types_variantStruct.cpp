#include "types/variantStruct.hpp"
#include "types/variantStructDescription.hpp"
#include <limits>
#include <stdexcept>
#include <cstdlib>
#include <sstream>
#include <stdint.h>	//... for uintptr_t

using namespace _Wolframe;
using namespace _Wolframe::types;

VariantStruct VariantStruct::array() const
{
	VariantStruct* prototype_ = (VariantStruct*)std::malloc( sizeof( VariantStruct));
	if (!prototype_) throw std::bad_alloc();
	try
	{
		VariantStruct::initCopy( *prototype_, *this);
	}
	catch (const std::bad_alloc& e)
	{
		std::free( prototype_);
		throw e;
	}
	VariantStruct rt;
	rt.setType( array_);
	rt.m_data.dim.size = 0;
	rt.m_data.value.ref_ = prototype_;
	return rt;
}

VariantIndirection::VariantIndirection( const VariantStructDescription* descr)
{
	setType( (Variant::Type)VariantStruct::indirection_);
	m_data.dim.metadata = (const void*)descr;
	m_data.value.ref_ = 0;
}

void VariantStruct::expandIndirection()
{
	if (type() != indirection_) return;
	initStruct( description());
}

void VariantStruct::initStruct( const VariantStructDescription* descr)
{
	setType( struct_);
	m_data.dim.metadata = (const void*)descr;
	m_data.value.ref_ = (VariantStruct*)std::malloc( descr->size() * sizeof( *this));
	if (!m_data.value.ref_) throw std::bad_alloc();
	VariantStructDescription::const_iterator si = descr->begin(), se = descr->end();
	std::size_t idx=0;
	try
	{
		for (; si!=se; ++si,++idx)
		{
			VariantStruct::initCopy( ((VariantStruct*)m_data.value.ref_)[idx], *si->initvalue);
		}
	}
	catch (const std::bad_alloc& e)
	{
		for (; idx>0; --idx) ((VariantStruct*)m_data.value.ref_)[idx-1].release();
		std::free( m_data.value.ref_);
		throw e;
	}
}

void VariantStruct::initCopy( VariantStruct& dest, const VariantStruct& orig)
{
	if (orig.constant())
	{
		std::memcpy( &dest, &orig, sizeof(dest));
	}
	else
	{
		std::size_t ii, nn;
		switch (orig.type())
		{
			case VariantStruct::bool_:
			case VariantStruct::double_:
			case VariantStruct::int_:
			case VariantStruct::uint_:
			case VariantStruct::string_:
				Variant::initCopy( dest, orig);
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
						VariantStruct::initCopy( ((VariantStruct*)dest.m_data.value.ref_)[ ii], ((VariantStruct*)orig.m_data.value.ref_)[ ii]);
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
				dest.m_data.value.ref_ = std::malloc( (orig.m_data.dim.size+1) * sizeof( VariantStruct));
				if (!dest.m_data.value.ref_) throw std::bad_alloc();
				ii=0; nn=orig.data().dim.size+1;
				try
				{
					for (; ii<nn; ++ii)
					{
						VariantStruct::initCopy( ((VariantStruct*)dest.m_data.value.ref_)[ ii], ((VariantStruct*)orig.m_data.value.ref_)[ ii]);
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
}

void VariantStruct::push()
{
	if ((Type)type() != VariantStruct::array_) throw std::logic_error("illegal operation push on non array");
	void* ref_ = std::realloc( m_data.value.ref_, (m_data.dim.size+2) * sizeof( VariantStruct));
	if (!ref_) throw std::bad_alloc();
	m_data.value.ref_ = ref_;
	std::size_t idx = m_data.dim.size;

	VariantStruct::initCopy( ((VariantStruct*)m_data.value.ref_)[ idx], ((VariantStruct*)m_data.value.ref_)[ 0]);
	//... copy prototype as value of the new element pushed
	++m_data.dim.size;
}

const VariantStruct& VariantStruct::back() const
{
	if ((Type)type() != VariantStruct::array_) throw std::logic_error("illegal operation push on non array");
	if (!m_data.dim.size) throw std::logic_error( "array bound read");
	return ((VariantStruct*)m_data.value.ref_)[ m_data.dim.size];
}

VariantStruct& VariantStruct::back()
{
	if ((Type)type() != VariantStruct::array_) throw std::logic_error("illegal operation push on non array");
	if (!m_data.dim.size) throw std::logic_error( "array bound write");
	return ((VariantStruct*)m_data.value.ref_)[ m_data.dim.size];
}

void VariantStruct::release()
{
	if (constant()) return;
	std::size_t ii, nn;
	switch ((Type)type())
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

int VariantStruct::compareArray( std::size_t size, const VariantStruct* a1, const VariantStruct* a2)
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
	Type tt = type();
	Type ot = o.type();
	if (tt == array_)
	{
		if (o.type() != array_) return -2;
		if (m_data.dim.size != o.m_data.dim.size)
		{
			return (int)(m_data.dim.size >= o.m_data.dim.size) + (int)(m_data.dim.size > o.m_data.dim.size) -1;
		}
		return compareArray( m_data.dim.size, (const VariantStruct*)m_data.value.ref_ + 1, (const VariantStruct*)o.m_data.value.ref_ + 1);
		//... ref +1, because prototype is not compared
	}
	else if (tt == struct_)
	{
		if (o.type() != struct_) return -2;
		int size1 = (int)(description()->size());
		int size2 = (int)(o.description()->size());
		if (size1 != size2) return size1 - size2;
		return compareArray( size1, (const VariantStruct*)m_data.value.ref_, (const VariantStruct*)o.m_data.value.ref_);
	}
	else if (tt == indirection_)
	{
		if (o.type() != indirection_) return -2;
		const VariantStructDescription* dd = description();
		const VariantStructDescription* od = o.description();
		if (!dd) return 1;
		if (!od) return -2;
		return dd->compare( *od);
	}
	else if (ot == array_ || ot == struct_ || ot == indirection_)
	{
		return +2;
	}
	return Variant::compare( o);
}

const VariantStruct* VariantStruct::at( std::size_t idx) const
{
	Type tt = type();
	if (tt == array_)
	{
		if (idx >= m_data.dim.size) throw std::logic_error( "array bound read");
		return ((const VariantStruct*)m_data.value.ref_ + 1 + idx);
	}
	else if (tt == struct_)
	{
		if (idx >= description()->size()) throw std::logic_error( "array bound read");
		return ((const VariantStruct*)m_data.value.ref_ + idx);
	}
	else
	{
		return 0;
	}
}

VariantStruct* VariantStruct::at( std::size_t idx)
{
	Type tt = type();
	if (tt == array_)
	{
		if (idx >= m_data.dim.size) throw std::logic_error( "array bound read");
		return ((VariantStruct*)m_data.value.ref_ + 1 + idx);
	}
	else if (tt == struct_)
	{
		if (idx >= description()->size()) throw std::logic_error( "array bound read");
		return ((VariantStruct*)m_data.value.ref_ + idx);
	}
	else
	{
		return 0;
	}
}

const VariantStruct* VariantStruct::select( const std::string& name) const
{
	const VariantStructDescription* descr = description();
	if (!descr) return 0;
	int fi = descr->findidx( name);
	return (fi < 0)?0:at(fi);
}

VariantStruct* VariantStruct::select( const std::string& name)
{
	const VariantStructDescription* descr = description();
	if (!descr) return 0;
	int fi = descr->findidx( name);
	return (fi < 0)?0:at(fi);
}

const VariantStruct& VariantStruct::operator[]( std::size_t idx) const
{
	const VariantStruct* rt = at( idx);
	if (!rt)
	{
		throw std::logic_error( std::string("random access not supported for type ") + Variant::typeName( (Variant::Type)rt->type()));
	}
	return *rt;
}

VariantStruct& VariantStruct::operator[]( std::size_t idx)
{
	VariantStruct* rt = at( idx);
	if (!rt)
	{
		throw std::logic_error( std::string("random access not supported for type ") + Variant::typeName( (Variant::Type)rt->type()));
	}
	return *rt;
}

VariantStruct::const_iterator VariantStruct::find( const std::string& name_) const
{
	const VariantStructDescription* descr = description();
	if (!descr) return const_iterator();
	int findidx_ = descr->findidx( name_);
	if (findidx_ < 0) return end();
	return VariantStruct::const_iterator( at( findidx_));
}

VariantStruct::iterator VariantStruct::find( const std::string& name_)
{
	const VariantStructDescription* descr = description();
	if (!descr) return iterator();
	int findidx_ = descr->findidx( name_);
	if (findidx_ < 0) return end();
	return VariantStruct::iterator( at( findidx_));
}

std::size_t VariantStruct::nof_elements() const
{
	if ((Type)type()==array_) return m_data.dim.size;
	if ((Type)type()==struct_) return description()->size();
	return 0;
}


int VariantStruct::const_iterator::compare( const const_iterator& o) const
{
	if (!o.m_itr) return m_itr?-1:0;
	if (!m_itr) return o.m_itr?+1:0;
	if (m_itr > o.m_itr) return +1;
	if (m_itr < o.m_itr) return -1;
	return 0;
}

int VariantStruct::iterator::compare( const iterator& o) const
{
	if (!o.m_itr) return m_itr?-1:0;
	if (!m_itr) return o.m_itr?+1:0;
	if (m_itr > o.m_itr) return +1;
	if (m_itr < o.m_itr) return -1;
	return 0;
}

void VariantStruct::print( std::ostream& out, const std::string& indent, const std::string& newitem, std::size_t level) const
{
	Type tt = type();
	if (tt == array_)
	{
		std::size_t ii=0, nn=m_data.dim.size;
		out << '{';
		for (; ii<nn; ++ii)
		{
			if (ii>0) out << ',';
			out << newitem;
			for (std::size_t ll=0; ll<level; ++ll) out << indent;
			((VariantStruct*)m_data.value.ref_ + 1 + ii)->print( out, indent, newitem, level+1);
		}
		if (nn>0)
		{
			out << newitem;
			for (std::size_t ll=0; ll<level; ++ll) out << indent;
		}
		out << '}';
	}
	else if (tt == struct_)
	{
		const VariantStructDescription* descr = description();
		VariantStructDescription::const_iterator di = descr->begin();
		const_iterator ii = begin(), ee = end();
		for (; ii!=ee; ++ii,++di)
		{
			out << newitem;
			for (std::size_t ll=0; ll<level; ++ll) out << indent;

			out << di->name << '=';
			ii->print( out, indent, newitem, level+1);
			out << ';';
		}
	}
	else if (tt == indirection_)
	{
	}
	else
	{
		out << '\'';
		out << Variant::tostring();
		out << '\'';
	}
}

std::string VariantStruct::tostring() const
{
	std::ostringstream buf;
	print( buf, "", " ", 0);
	return buf.str();
}




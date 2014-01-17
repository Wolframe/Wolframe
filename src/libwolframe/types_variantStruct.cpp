/************************************************************************
 Copyright (C) 2011 - 2013 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
#include "types/variantStruct.hpp"
#include "types/variantStructDescription.hpp"
#include "types/malloc.hpp"
#include <limits>
#include <stdexcept>
#include <cstdlib>
#include <sstream>
#include <stdint.h>	//... for uintptr_t

using namespace _Wolframe;
using namespace _Wolframe::types;

#undef _Wolframe_LOWLEVEL_DEBUG

void VariantStruct::makeArray()
{
	VariantStruct* prototype_ = (VariantStruct*)wolframe_malloc( sizeof( *this));
	if (!prototype_) throw std::bad_alloc();
	std::memcpy( prototype_, this, sizeof( *this));
	init();
	setType( Array);
	m_data.dim.size = 0;
	m_data.value.Ref = prototype_;
}

VariantIndirection::VariantIndirection( const VariantStructDescription* descr)
{
	m_type = (unsigned char)( VariantStruct::Indirection);
	m_data.dim.metadata = (const void*)descr;
	m_data.value.Ref = 0;
}

VariantUnresolved::VariantUnresolved( const std::string& name_)
{
	initUnresolved( name_);
}

void VariantStruct::initIndirection( const VariantStructDescription* descr)
{
	m_type = (unsigned char)( Indirection);
	m_data.dim.metadata = (const void*)descr;
	m_data.value.Ref = 0;
}

void VariantStruct::expandIndirection()
{
	bool init_ = initialized();
	if (type() != Indirection) return;
	const VariantStructDescription* descr = description();
	release();
	initStruct( descr);
	setInitialized( init_);
}

void VariantStruct::initStruct( const VariantStructDescription* descr)
{
	setType( Struct);
	m_data.dim.metadata = (const void*)descr;

	if (descr->size())
	{
		m_data.value.Ref = (VariantStruct*)wolframe_calloc( descr->size(), sizeof(VariantStruct));
		if (!m_data.value.Ref) throw std::bad_alloc();

		VariantStructDescription::const_iterator si = descr->begin(), se = descr->end();
		std::size_t idx=0;
		try
		{
			for (; si!=se; ++si,++idx)
			{
				VariantStruct* elem = (VariantStruct*)m_data.value.Ref + idx;
				elem->initCopy( *si->initvalue);
			}
		}
		catch (const std::bad_alloc& e)
		{
			for (; idx>0; --idx) ((VariantStruct*)m_data.value.Ref)[idx-1].release();
			wolframe_free( m_data.value.Ref);
			throw e;
		}
		catch (const std::runtime_error& e)
		{
			for (; idx>0; --idx) ((VariantStruct*)m_data.value.Ref)[idx-1].release();
			wolframe_free( m_data.value.Ref);
			throw e;
		}
	}
	else
	{
		m_data.value.Ref = 0;
	}
#ifdef _Wolframe_LOWLEVEL_DEBUG
	check();
#endif
}

void VariantStruct::initUnresolved( const std::string& name_)
{
	setType( Unresolved);
	char* nm = (char*)wolframe_malloc( name_.size()+1);
	if (!nm) throw std::bad_alloc();
	std::memcpy( nm, name_.c_str(), name_.size());
	nm[ name_.size()] = 0;
	m_data.value.Ref = nm;
	m_data.dim.size = name_.size();
}

void VariantStruct::resolve( const ResolveMap& rmap)
{
	if (type() == Unresolved)
	{
		const std::string rname( std::string( (const char*)m_data.value.Ref, m_data.dim.size));
		ResolveMap::const_iterator ri = rmap.find( rname);
		if (ri == rmap.end()) throw std::runtime_error( std::string( "failed to resolve reference to '") + rname + "'");
		wolframe_free( m_data.value.Ref);
		init();
		initIndirection( ri->second);
	}
	else if (type() == Struct)
	{
		VariantStruct::iterator vi = begin(), ve = end();
		for (; vi != ve; ++vi) vi->resolve( rmap);
	}
	else if (type() == Array)
	{
		prototype()->resolve( rmap);
		VariantStruct::iterator vi = begin(), ve = end();
		for (; vi != ve; ++vi) vi->resolve( rmap);
	}
	else if (type() == Indirection)
	{
		prototype()->resolve( rmap);
	}
}

void VariantStruct::initConstCopy( const VariantStruct& o)
{
	std::memcpy( this, &o, sizeof(*this));
	setConstant();
}

void VariantStruct::initConstCopy( const Variant& o)
{
	std::memcpy( this, &o, sizeof(*this));
	setConstant();
}

void VariantStruct::initCopy( const VariantStruct& o)
{
	std::size_t ii, nn;
	bool init_ = o.initialized();

	switch (o.type())
	{
		case VariantStruct::Null:
		case VariantStruct::Bool:
		case VariantStruct::Double:
		case VariantStruct::Int:
		case VariantStruct::UInt:
		case VariantStruct::String:
		case VariantStruct::ADT:
			Variant::initCopy( o);
			break;

		case VariantStruct::Struct:
		case VariantStruct::Array:
			init();
			if (o.type() == VariantStruct::Struct)
			{
				nn = o.description()->size();
				setType( Struct);
				m_data.dim.metadata = o.m_data.dim.metadata;
			}
			else
			{
				nn = o.m_data.dim.size+1;
				setType( Array);
				m_data.dim.size = o.m_data.dim.size;
			}
			if (nn)
			{
				m_data.value.Ref = wolframe_calloc( nn, sizeof( VariantStruct));
				if (!m_data.value.Ref) throw std::bad_alloc();
			}
			else
			{
				m_data.value.Ref = 0;
			}
			ii = 0;
			try
			{
				for (; ii<nn; ++ii)
				{
					VariantStruct* elem = (VariantStruct*)m_data.value.Ref + ii;
					elem->initCopy( ((VariantStruct*)o.m_data.value.Ref)[ ii]);
				}
			}
			catch (const std::bad_alloc& e)
			{
				for (; ii>0; --ii) ((VariantStruct*)m_data.value.Ref)[ ii-1].release();
				wolframe_free( m_data.value.Ref);
				throw e;
			}
			catch (const std::runtime_error& e)
			{
				for (; ii>0; --ii) ((VariantStruct*)m_data.value.Ref)[ ii-1].release();
				wolframe_free( m_data.value.Ref);
				throw e;
			}
			setInitialized( init_);
			break;

		case VariantStruct::Indirection:
			init();
			setType( Indirection);
			m_data.dim.metadata = o.m_data.dim.metadata;
			setInitialized( init_);
			break;

		case VariantStruct::Unresolved:
			init();
			setType( Unresolved);
			char* nm = (char*)wolframe_malloc( o.m_data.dim.size+1);
			if (!nm) throw std::bad_alloc();
			std::memcpy( nm, o.m_data.value.Ref, o.m_data.dim.size);
			nm[ o.m_data.dim.size] = 0;
			m_data.value.Ref = nm;
			m_data.dim.size = o.m_data.dim.size;
			setInitialized( init_);
			break;
	}
}

void VariantStruct::push()
{
	if (type() != Array) throw std::logic_error("illegal operation push on non array");
	void* ref = wolframe_realloc( m_data.value.Ref, (m_data.dim.size+2) * sizeof( VariantStruct));
	if (!ref) throw std::bad_alloc();
	m_data.value.Ref = ref;
	std::size_t idx = m_data.dim.size;
	VariantStruct* elem = (VariantStruct*)m_data.value.Ref + idx+1;
	elem->init();
	elem->initCopy( ((VariantStruct*)m_data.value.Ref)[ 0]); //... copy prototype as value of the new element pushed
	++m_data.dim.size; //... increment size
}

const VariantStruct& VariantStruct::back() const
{
	if (type() != Array) throw std::logic_error("illegal operation push on non array");
	if (!m_data.dim.size) throw std::logic_error( "array bound read");
	return ((VariantStruct*)m_data.value.Ref)[ m_data.dim.size];
}

VariantStruct& VariantStruct::back()
{
	if (type() != Array) throw std::logic_error("illegal operation push on non array");
	if (!m_data.dim.size) throw std::logic_error( "array bound write");
	return ((VariantStruct*)m_data.value.Ref)[ m_data.dim.size];
}

void VariantStruct::release()
{
#ifdef _Wolframe_LOWLEVEL_DEBUG
	check();
#endif
	if (constant()) return;
	std::size_t ii, nn;

	switch (type())
	{
		case VariantStruct::Null:
		case VariantStruct::Bool:
		case VariantStruct::Double:
		case VariantStruct::Int:
		case VariantStruct::UInt:
			break;
		case VariantStruct::ADT:
		case VariantStruct::String:
			Variant::release();
			break;

		case VariantStruct::Struct:
			ii=0; nn = description()->size();
			for (; ii<nn; ++ii)
			{
				((VariantStruct*)m_data.value.Ref)[ ii].release();
			}
			wolframe_free( m_data.value.Ref);
			init();
			break;

		case VariantStruct::Array:
			ii=0; nn=m_data.dim.size+1;
			for (; ii<nn; ++ii)
			{
				((VariantStruct*)m_data.value.Ref)[ ii].release();
			}
			wolframe_free( m_data.value.Ref);
			init();
			break;

		case VariantStruct::Indirection:
			init();
			break;

		case VariantStruct::Unresolved:
			wolframe_free( m_data.value.Ref);
			init();
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
	if (tt == Array)
	{
		if (o.type() != Array) return -2;
		if (m_data.dim.size != o.m_data.dim.size)
		{
			return (int)(m_data.dim.size >= o.m_data.dim.size) + (int)(m_data.dim.size > o.m_data.dim.size) -1;
		}
		return compareArray( m_data.dim.size, (const VariantStruct*)m_data.value.Ref + 1, (const VariantStruct*)o.m_data.value.Ref + 1);
		//... ref +1, because prototype is not compared
	}
	else if (tt == Struct)
	{
		if (o.type() != Struct) return -2;
		int size1 = (int)(description()->size());
		int size2 = (int)(o.description()->size());
		if (size1 != size2) return size1 - size2;
		return compareArray( size1, (const VariantStruct*)m_data.value.Ref, (const VariantStruct*)o.m_data.value.Ref);
	}
	else if (tt == Indirection)
	{
		if (o.type() != Indirection) return -2;
		const VariantStructDescription* dd = description();
		const VariantStructDescription* od = o.description();
		if (!dd) return 1;
		if (!od) return -2;
		return dd->compare( *od);
	}
	else if (tt == Unresolved)
	{
		if (o.type() != Unresolved) return -2;
		return std::strcmp( (char*)m_data.value.Ref, (char*)o.m_data.value.Ref);
	}
	else if (ot == Array || ot == Struct || ot == Indirection || ot == Unresolved)
	{
		return +2;
	}
	return Variant::compare( o);
}

void VariantStruct::setDescription( const VariantStructDescription* descr)
{
	Type tp = (Type)type();
	if (tp == Array)
	{
		if (descr)
		{
			prototype()->setDescription( descr);
			VariantStruct::iterator vi = begin(), ve = end();
			for (; vi != ve; vi++)
			{
				vi->setDescription( descr);
			}
		}
		else
		{
			bool ok = true;
			if (prototype()->description()) ok = false;
			VariantStruct::iterator vi = begin(), ve = end();
			for (; vi != ve; vi++)
			{
				if (vi->description()) ok = false;
			}
			if (!ok)
			{
				throw std::logic_error("illegal operation (set description does not match to atomic type)");
			}
		}
	}
	else if (tp == Struct)
	{
		VariantStruct::iterator vi = begin(), ve = end();
		VariantStructDescription::const_iterator di = descr->begin(), de = descr->end();
		for (; vi != ve && di != de; vi++,di++)
		{
			if (di->substruct)
			{
				vi->setDescription( di->substruct);
			}
			else
			{
				if (vi->type() != di->initvalue->type()) break;
			}
		}
		if (vi != ve || di != de)
		{
			throw std::logic_error("illegal operation (set description does not match to structure type)");
		}
		m_data.dim.metadata = descr;
	}
	else if (descr)
	{
		throw std::logic_error("illegal operation (try to set description of an atomic type)");
	}
}

void VariantStruct::setIndirectionDescription( const DescriptionAssignmentMap& assignmentmap)
{
	Type tp = (Type)type();
	if (tp == Indirection)
	{
		DescriptionAssignmentMap::const_iterator ai = assignmentmap.find( (const VariantStructDescription*)m_data.dim.metadata);
		if (ai != assignmentmap.end())
		{
			m_data.dim.metadata = ai->second;
		}
	}
	else if (tp == Array)
	{
		prototype()->setIndirectionDescription( assignmentmap);
		VariantStruct::iterator vi = begin(), ve = end();
		for (; vi != ve; vi++)
		{
			vi->setIndirectionDescription( assignmentmap);
		}
	}
	else if (tp == Struct)
	{
		VariantStruct::iterator vi = begin(), ve = end();
		for (; vi != ve; vi++)
		{
			vi->setIndirectionDescription( assignmentmap);
		}
	}
}

const VariantStruct* VariantStruct::at( std::size_t idx) const
{
	Type tt = type();
	if (tt == Array)
	{
		if (idx >= m_data.dim.size) throw std::logic_error( "array bound read");
		return ((const VariantStruct*)m_data.value.Ref + 1 + idx);
	}
	else if (tt == Struct)
	{
		if (idx >= description()->size()) throw std::logic_error( "array bound read");
		return ((const VariantStruct*)m_data.value.Ref + idx);
	}
	else
	{
		return 0;
	}
}

VariantStruct* VariantStruct::at( std::size_t idx)
{
	Type tt = type();
	if (tt == Array)
	{
		if (idx >= m_data.dim.size) throw std::logic_error( "array bound read");
		return ((VariantStruct*)m_data.value.Ref + 1 + idx);
	}
	else if (tt == Struct)
	{
		if (idx >= description()->size()) throw std::logic_error( "array bound read");
		return ((VariantStruct*)m_data.value.Ref + idx);
	}
	else
	{
		return 0;
	}
}

const VariantStruct* VariantStruct::elementptr( std::size_t idx) const
{
	Type tt = type();
	if (tt == Array)
	{
		return ((const VariantStruct*)m_data.value.Ref + 1 + idx);
	}
	else if (tt == Struct)
	{
		return ((const VariantStruct*)m_data.value.Ref + idx);
	}
	else
	{
		return 0;
	}
}

VariantStruct* VariantStruct::elementptr( std::size_t idx)
{
	Type tt = type();
	if (tt == Array)
	{
		return ((VariantStruct*)m_data.value.Ref + 1 + idx);
	}
	else if (tt == Struct)
	{
		return ((VariantStruct*)m_data.value.Ref + idx);
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
		throw std::logic_error( std::string("random access not supported for type ") + typeName( rt->type()));
	}
	return *rt;
}

VariantStruct& VariantStruct::operator[]( std::size_t idx)
{
	VariantStruct* rt = at( idx);
	if (!rt)
	{
		throw std::logic_error( std::string("random access not supported for type ") + typeName( rt->type()));
	}
	return *rt;
}

VariantStruct::const_iterator VariantStruct::find( const std::string& name_) const
{
	const VariantStructDescription* descr = description();
	if (!descr) return end();
	int findidx_ = descr->findidx( name_);
	if (findidx_ < 0) return end();
	return VariantStruct::const_iterator( at( findidx_));
}

VariantStruct::iterator VariantStruct::find( const std::string& name_)
{
	const VariantStructDescription* descr = description();
	if (!descr) return end();
	int findidx_ = descr->findidx( name_);
	if (findidx_ < 0) return end();
	return VariantStruct::iterator( at( findidx_));
}

VariantStruct::const_iterator VariantStruct::find_cis( const std::string& name_) const
{
	const VariantStructDescription* descr = description();
	if (!descr) return end();
	int findidx_ = descr->findidx_cis( name_);
	if (findidx_ < 0) return end();
	return VariantStruct::const_iterator( at( findidx_));
}

VariantStruct::iterator VariantStruct::find_cis( const std::string& name_)
{
	const VariantStructDescription* descr = description();
	if (!descr) return end();
	int findidx_ = descr->findidx_cis( name_);
	if (findidx_ < 0) return end();
	return VariantStruct::iterator( at( findidx_));
}

std::size_t VariantStruct::nof_elements() const
{
	if ((Type)type()==Array) return m_data.dim.size;
	if ((Type)type()==Struct) return description()->size();
	return 0;
}


int VariantStruct::const_iterator::compare( const const_iterator& o) const
{
	if (m_itr > o.m_itr) return +1;
	if (m_itr < o.m_itr) return -1;
	return 0;
}

int VariantStruct::iterator::compare( const iterator& o) const
{
	if (m_itr > o.m_itr) return +1;
	if (m_itr < o.m_itr) return -1;
	return 0;
}

static void print_newitem( std::ostream& out, const utils::PrintFormat* pformat, std::size_t level)
{
	out << pformat->newitem;
	for (std::size_t ll=0; ll<level; ++ll) out << pformat->indent;
}

void VariantStruct::print( std::ostream& out, const utils::PrintFormat* pformat, std::size_t level) const
{
	Type tt = type();
	if (tt == Array)
	{
		std::size_t ii=0, nn=m_data.dim.size;
		out << pformat->openstruct;
		for (; ii<nn; ++ii)
		{
			if (ii>0) out << pformat->decldelimiter;
			print_newitem( out, pformat, level);
			((VariantStruct*)m_data.value.Ref + 1 + ii)->print( out, pformat, level+1);
		}
		if (nn>0)
		{
			print_newitem( out, pformat, level);
		}
		out << pformat->closestruct;
	}
	else if (tt == Struct)
	{
		out << pformat->openstruct;
		const VariantStructDescription* descr = description();
		VariantStructDescription::const_iterator di = descr->begin();
		const_iterator ii = begin(), ee = end();
		for (int idx=0; ii!=ee; ++ii,++di,++idx)
		{
			if (idx) out << pformat->decldelimiter;
			print_newitem( out, pformat, level);

			out << di->name << pformat->assign;
			ii->print( out, pformat, level+1);
		}
		out << pformat->closestruct;
	}
	else if (tt == Indirection)
	{
		out << "*";
	}
	else if (tt == Unresolved)
	{
		out << "->" << unresolvedName();
	}
	else
	{
		out << pformat->startvalue;
		if (pformat->maxitemsize)
		{
			out << utils::getLogString( *this, pformat->maxitemsize);
		}
		else
		{
			out << Variant::tostring();
		}
		out << pformat->endvalue;
	}
}

std::string VariantStruct::tostring( const utils::PrintFormat* pformat) const
{
	std::ostringstream buf;
	print( buf, pformat, 0);
	return buf.str();
}


std::ostream& std::operator << (std::ostream &os, const _Wolframe::types::VariantStruct& o)
{
	return os << o.tostring();
}

void VariantStruct::check() const
{
#ifdef _Wolframe_LOWLEVEL_DEBUG
	if ((Type)m_type == Struct || (Type)m_type == Indirection)
	{
		const VariantStructDescription* descr = (const VariantStructDescription*)m_data.dim.metadata;
		if (descr->size() > 1000)
		{
			throw std::logic_error( "structure referencing deallocated description");
		}
	}
#endif
}


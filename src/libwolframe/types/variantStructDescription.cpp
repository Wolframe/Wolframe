/************************************************************************
 Copyright (C) 2011 - 2014 Project Wolframe.
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
#include "types/variantStructDescription.hpp"
#include "types/variantStruct.hpp"
#include "types/customDataType.hpp"
#include "utils/printFormats.hpp"
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <stdint.h>	//... for uintptr_t

#undef _Wolframe_LOWLEVEL_DEBUG

using namespace _Wolframe;
using namespace _Wolframe::types;

void VariantStructDescription::Element::makeArray()
{
	if (array()) throw std::logic_error( "illegal operation (make array called on array)");
	initvalue->makeArray();
	flags |= (unsigned char)Array;
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
		if (elem.name) std::free( elem.name);
		throw e;
	}
	catch (const std::runtime_error& e)
	{
		if (elem.name) std::free( elem.name);
		throw e;
	}
	if (o.substruct) try
	{
		elem.substruct = new VariantStructDescription( *o.substruct);
	}
	catch (const std::bad_alloc& e)
	{
		if (elem.initvalue) delete elem.initvalue;
		if (elem.name) std::free( elem.name);
		throw e;
	}
	catch (const std::runtime_error& e)
	{
		if (elem.initvalue) delete elem.initvalue;
		if (elem.name) std::free( elem.name);
		throw e;
	}
	elem.normalizer = o.normalizer;
	elem.flags = o.flags;
	if (elem.initvalue)
	{
		elem.initvalue->setDescription( elem.substruct);
	}
	if (initvalue) delete initvalue;
	if (substruct) delete substruct;
	if (name) std::free( name);
	std::memcpy( this, &elem, sizeof( elem));
}

Variant::Type VariantStructDescription::Element::type() const
{
	return (Variant::Type)initvalue->type();
}

VariantStructDescription::VariantStructDescription()
	:m_size(0),m_nofattributes(0),m_ar(0){}

VariantStructDescription::VariantStructDescription( const VariantStructDescription& o)
	:m_size(o.m_size),m_nofattributes(o.m_nofattributes),m_ar(0)
{
	const char* errmsg = 0;
	std::size_t ii=0;
	if (m_size)
	{
		m_ar = (Element*)std::calloc( m_size, sizeof(Element));
		if (!m_ar) goto BAD_ALLOC;
		for (; ii<m_size; ++ii)
		{
			if (o.m_ar[ ii].name)
			{
				std::size_t len = std::strlen( o.m_ar[ ii].name);
				m_ar[ ii].name = (char*)std::malloc( len+1);
				if (!m_ar[ ii].name) goto BAD_ALLOC;
				std::memcpy( m_ar[ ii].name, o.m_ar[ ii].name, len+1);
			}
			if (o.m_ar[ ii].substruct) try
			{
				m_ar[ ii].substruct = new VariantStructDescription( *o.m_ar[ ii].substruct);
			}
			catch (const std::bad_alloc&)
			{
				goto BAD_ALLOC;
			}
			catch (const std::runtime_error& e)
			{
				errmsg = e.what();
				goto BAD_ALLOC;
			}
			if (o.m_ar[ ii].initvalue) try
			{
				m_ar[ ii].initvalue = new VariantStruct( *o.m_ar[ ii].initvalue);
				m_ar[ ii].initvalue->setDescription( m_ar[ ii].substruct);
			}
			catch (const std::bad_alloc&)
			{
				goto BAD_ALLOC;
			}
			catch (const std::runtime_error& e)
			{
				errmsg = e.what();
				goto BAD_ALLOC;
			}
			m_ar[ ii].normalizer = o.m_ar[ ii].normalizer;
			m_ar[ ii].flags = o.m_ar[ ii].flags;
		}
	}
#ifdef _Wolframe_LOWLEVEL_DEBUG
	check();
#endif
	return;
	BAD_ALLOC:
		if (m_ar)
		{
			for (; ii>0; --ii)
			{
				if (m_ar[ ii-1].initvalue) delete m_ar[ ii-1].initvalue;
				if (m_ar[ ii-1].substruct) delete m_ar[ ii-1].substruct;
				if (m_ar[ ii-1].name) std::free( m_ar[ ii-1].name);
			}
			std::free( m_ar);
		}
		if (errmsg)
		{
			throw std::runtime_error( errmsg);
		}
		else
		{
			throw std::bad_alloc();
		}
}

VariantStructDescription::~VariantStructDescription()
{
	std::size_t ii;
	for (ii=0; ii<m_size; ++ii)
	{
		Element* ee = m_ar+ii;
		if (ee->initvalue) delete ee->initvalue;
		if (ee->substruct) delete ee->substruct;
		if (ee->name) std::free( ee->name);
	}
	if (m_ar) std::free( m_ar);
}

void VariantStructDescription::resolve( const ResolveMap& rmap)
{
	std::size_t ii;
	for (ii=0; ii<m_size; ++ii)
	{
		Element* ee = m_ar+ii;
		if (ee->initvalue) ee->initvalue->resolve( rmap);
		if (ee->substruct) ee->substruct->resolve( rmap);
	}
}

int VariantStructDescription::addAtom( const std::string& name_, const Variant& initvalue, const NormalizeFunction* normalizer_)

{
	if (findidx( name_) >= 0) throw std::runtime_error( std::string("try to add duplicate element '") + name_ + "' to structure description");

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
		std::free( m_ar[ m_size].name);
		m_ar[ m_size].name = 0;
		throw e;
	}
	catch (const std::runtime_error& e)
	{
		std::free( m_ar[ m_size].name);
		m_ar[ m_size].name = 0;
		throw e;
	}
	m_ar[ m_size].normalizer = normalizer_;
	m_ar[ m_size].substruct = 0;
	m_ar[ m_size].flags = 0;
#ifdef _Wolframe_LOWLEVEL_DEBUG
	check();
#endif
	return m_size++;
}

int VariantStructDescription::addStructure( const std::string& name_, const VariantStructDescription& substruct_)
{
	if (findidx( name_) >= 0) throw std::runtime_error( std::string("try to add duplicate element '") + name_ + "' to structure description");

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
		std::free( m_ar[ m_size].name);
		m_ar[ m_size].name = 0;
		throw e;
	}
	catch (const std::runtime_error& e)
	{
		std::free( m_ar[ m_size].name);
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
		std::free( m_ar[ m_size].name);
		m_ar[ m_size].name = 0;
		throw e;
	}
	catch (const std::runtime_error& e)
	{
		delete m_ar[m_size].substruct;
		std::free( m_ar[ m_size].name);
		m_ar[ m_size].name = 0;
		throw e;
	}
	m_ar[ m_size].normalizer = 0;
	m_ar[ m_size].flags = 0;
#ifdef _Wolframe_LOWLEVEL_DEBUG
	check();
#endif
	return m_size++;
}

int VariantStructDescription::addIndirection( const std::string& name_, const VariantStructDescription* descr)
{
	int rt = addAtom( name_, VariantIndirection( descr), 0);
	back().setOptional();
#ifdef _Wolframe_LOWLEVEL_DEBUG
	check();
#endif
	return rt;
}

int VariantStructDescription::addUnresolved( const std::string& name_, const std::string& symbol_)
{
	int rt = addAtom( name_, VariantUnresolved( symbol_), 0);
	back().setOptional();
#ifdef _Wolframe_LOWLEVEL_DEBUG
	check();
#endif
	return rt;
}

int VariantStructDescription::addElement( const Element& elem)
{
	int rt;
	if (!elem.name) throw std::runtime_error( "try to add element without name in structure description");
	if (findidx( elem.name) >= 0) throw std::runtime_error( std::string("try to add duplicate element '") + elem.name + "' to structure description");

	if (elem.attribute())
	{
		if (!elem.initvalue) throw std::runtime_error( "try to add incomplete element (null value)");
		rt = addAttribute( elem.name, *elem.initvalue, elem.normalizer);
		m_ar[ rt].flags = elem.flags;
	}
	else
	{
		if ((std::size_t)std::numeric_limits<int>::max() <= m_size+1) throw std::bad_alloc();
		Element* ar_ = (Element*)std::realloc( m_ar, sizeof(Element) * (m_size+1));
		if (!ar_) throw std::bad_alloc();
		m_ar = ar_;
		std::memset( m_ar + m_size, 0, sizeof( *m_ar));
		m_ar[ m_size].copy( elem);
		rt = m_size++;
	}
#ifdef _Wolframe_LOWLEVEL_DEBUG
	check();
#endif
	return rt;
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
		std::memmove( &m_ar[ m_nofattributes], &m_ar[ m_nofattributes-1], mm);	//... shift elements up one position to get the slot to swap 'attr' with
		std::memcpy( &m_ar[ m_nofattributes-1], &attr, sizeof( attr));		//... move 'attr' to the free slot
	}
#ifdef _Wolframe_LOWLEVEL_DEBUG
	check();
#endif
	return m_nofattributes-1;
}

void VariantStructDescription::inherit( const VariantStructDescription& parent)
{
	const_iterator pi = parent.begin(), pe = parent.end();
	for (; pi != pe; ++pi) addElement( *pi);
#ifdef _Wolframe_LOWLEVEL_DEBUG
	check();
#endif
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
	if (findidx_ < 0) return VariantStructDescription::end();
	return VariantStructDescription::const_iterator( m_ar+findidx_);
}

VariantStructDescription::iterator VariantStructDescription::find( const std::string& name_)
{
	int findidx_ = findidx( name_);
	if (findidx_ < 0) return VariantStructDescription::end();
	return VariantStructDescription::iterator( m_ar+findidx_);
}

int VariantStructDescription::findidx_cis( const std::string& name_) const
{
	std::size_t ii = 0;
	for (; ii<m_size; ++ii)
	{
		const char* aa = name_.c_str();
		const char* bb = m_ar[ii].name;
		std::size_t kk = 0;
		for (; aa[kk]; kk++)
		{
			if ((unsigned char)aa[kk] <= 127)
			{
				if ((aa[kk]|32) != (bb[kk]|32)) break;
			}
			else
			{
				if (aa[kk] != bb[kk]) break;
			}
		}
		if (!aa[kk] && !bb[kk]) return ii;
	}
	return -1;
}

VariantStructDescription::const_iterator VariantStructDescription::find_cis( const std::string& name_) const
{
	int findidx_ = findidx( name_);
	if (findidx_ < 0) return VariantStructDescription::end();
	return VariantStructDescription::const_iterator( m_ar+findidx_);
}

VariantStructDescription::iterator VariantStructDescription::find_cis( const std::string& name_)
{
	int findidx_ = findidx( name_);
	if (findidx_ < 0) return VariantStructDescription::end();
	return VariantStructDescription::iterator( m_ar+findidx_);
}

int VariantStructDescription::const_iterator::compare( const const_iterator& o) const
{
	if (m_itr > o.m_itr) return +1;
	if (m_itr < o.m_itr) return -1;
	return 0;
}

int VariantStructDescription::iterator::compare( const iterator& o) const
{
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

static void print_newitem( std::ostream& out, const utils::PrintFormat* pformat, std::size_t level)
{
	out << pformat->newitem;
	for (std::size_t ll=0; ll<level; ++ll) out << pformat->indent;
}

static void print_( const VariantStructDescription* this_, std::ostream& out, const utils::PrintFormat* pformat, std::size_t level, std::vector<const VariantStructDescription*>& stk)
{
	static Variant default_bool( Variant::Bool);
	static Variant default_int( Variant::Int);
	static Variant default_uint( Variant::UInt);
	static Variant default_double( Variant::Double);
	static Variant default_string( Variant::String);
	static Variant default_bignumber( Variant::BigNumber);
	static Variant default_timestamp( Variant::Timestamp);

	VariantStructDescription::const_iterator di = this_->begin(), de = this_->end();
	for (; di!=de; ++di)
	{
		print_newitem( out, pformat, level);
		const VariantStruct* value = di->initvalue;
		enum ElemType {ET_Atomic,ET_Struct,ET_VisitedIndirection,ET_ExpandedIndirection};
		ElemType elemType = ET_Atomic;
		bool isArray = false;

		// [A] Get the visited element value and type:
		if (value)
		{
			if (di->array())
			{
				isArray = true;
				value = value->prototype();
			}
			if (value->type() == VariantStruct::Indirection)
			{
				const VariantStructDescription* descr = value->description();
				if (!descr) throw std::runtime_error("internal: corrupt variant data type (indirection)");

				std::vector<const VariantStructDescription*>::const_iterator si = stk.begin(), se = stk.end();
				for (; si != se && descr != *si; ++si){}
				if (si == se)
				{
					elemType = ET_ExpandedIndirection;
					stk.push_back( descr);
				}
				else
				{
					elemType = ET_VisitedIndirection;
				}
			}
			else if (value->atomic())
			{
				elemType = ET_Atomic;
			}
			else
			{
				elemType = ET_Struct;
			}
		}
		// [B] Print the visited element:
		switch (elemType)
		{
			case ET_Atomic:
			{
				if (!value) throw std::runtime_error("internal: corrupt variant data type (atomic value)");
				out << di->name;
				out << pformat->itemdelimiter;
				if (di->optional() && !di->array()) out << "?";
				if (di->mandatory()) out << "!";
				if (di->attribute()) out << "@";

				int cmp = 0;
				Variant::Type tp = (Variant::Type)value->type();
				out << Variant::typeName( tp);
				if (isArray) out << "[]";

				switch (tp)
				{
					case Variant::Null: cmp = 0; break;
					case Variant::Bool: cmp = value->compare( default_bool); break;
					case Variant::Int: cmp = value->compare( default_int); break;
					case Variant::UInt: cmp = value->compare( default_uint); break;
					case Variant::Double: cmp = value->compare( default_double); break;
					case Variant::String: cmp = value->compare( default_string); break;
					case VariantStruct::BigNumber: cmp = value->compare( default_bignumber); break;
					case VariantStruct::Timestamp: cmp = value->compare( default_timestamp); break;
						
					case Variant::Custom:
					{
						types::Variant default_custom( value->data().value.Custom->type(), value->data().value.Custom->initializer());
						cmp = value->compare( default_custom);
						break;
					}
				}
				if (cmp != 0)
				{
					out << " (" << value->tostring() << ")";
				}
				out << pformat->decldelimiter;
				break;
			}
			case ET_Struct:
			case ET_ExpandedIndirection:
			{
				const VariantStructDescription* descr = value->description();
				if (!descr) throw std::runtime_error("internal: corrupt variant data type (structure)");
				out << di->name;
				out << pformat->itemdelimiter;
				if (di->optional() && !di->array()) out << "?";
				if (di->mandatory()) out << "!";
				if (di->array()) out << "[]";
				if (elemType == ET_ExpandedIndirection) out << "->";
				out << pformat->openstruct;
				if (descr->size())
				{
					print_( descr, out, pformat, level+1, stk);
					print_newitem( out, pformat, level);
				}
				out << pformat->closestruct;
				out << pformat->decldelimiter;
				if (elemType == ET_ExpandedIndirection)
				{
					stk.pop_back();
				}
				break;
			}
			case ET_VisitedIndirection:
				out << "^" << di->name;
				out << pformat->decldelimiter;
		}
	}
}

void VariantStructDescription::print( std::ostream& out, const utils::PrintFormat* pformat, std::size_t level) const
{
	std::vector<const VariantStructDescription*> stk;
	print_( this, out, pformat?pformat:utils::logPrintFormat(), level, stk);
}

std::string VariantStructDescription::tostring( const utils::PrintFormat* pformat) const
{
	std::stringstream out;
	const utils::PrintFormat* pf = pformat?pformat:utils::logPrintFormat();
	out << pf->openstruct;
	print( out, pf, 0);
	out << pf->closestruct;
	return out.str();
}

void VariantStructDescription::check() const
{
#ifdef _Wolframe_LOWLEVEL_DEBUG
	const_iterator di = begin(), de = end();
	for (; di != de; ++di)
	{
		if (di->substruct)
		{
			const VariantStructDescription* sd = di->initvalue->description();
			if (sd && sd != di->substruct)
			{
				throw std::logic_error( "structure referencing wrong description");
			}
			di->substruct->check();
		}
	}
#endif
}



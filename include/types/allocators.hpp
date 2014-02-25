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
///\brief Interface for special purpose memory allocators
///\file types/allocators.hpp
#ifndef _TYPES_ALLOCATORS_HPP_INCLUDED
#define _TYPES_ALLOCATORS_HPP_INCLUDED
#include <cstddef>
#include <stdexcept>

namespace _Wolframe {
namespace types {

//\class GreedySmallChunkAllocator
//\brief Interface for an allocator for small chunks without a free. Memory is freed in the destructor of the allocator
class GreedySmallChunkAllocator
{
public:
	GreedySmallChunkAllocator();
	~GreedySmallChunkAllocator();
	void* alloc( std::size_t nofBytes);

private:
	class MemChunk;
	MemChunk* m_chunk;
};


//\class ArrayDoublingAllocator
//\brief Interface for an allocator implemented with the strategy of array doubling
class ArrayDoublingAllocator
{
public:
	ArrayDoublingAllocator();
	~ArrayDoublingAllocator();

	std::size_t alloc( std::size_t nofBytes);
	const void* base() const			{return m_ar;}
	void* base()					{return m_ar;}
	std::size_t size() const			{return m_pos;}

private:
	enum {InitBlockSize=(1<<14)};
	char* m_ar;
	std::size_t m_size;
	std::size_t m_pos;
};


//\class TypedArrayDoublingAllocator
//\brief Interface for an array doubling allocator that allocates only one fixed size type of element
template <typename Type>
struct TypedArrayDoublingAllocator :public types::ArrayDoublingAllocator
{
	const Type* base() const
	{
		return (const Type*)types::ArrayDoublingAllocator::base();
	}

	Type* base()
	{
		return (Type*)types::ArrayDoublingAllocator::base();
	}

	const Type& operator[]( std::size_t idx) const
	{
		if (idx > size()/sizeof(Type))
		{
			throw std::logic_error( "Array bounds access");
		}
		return ((const Type*)types::ArrayDoublingAllocator::base())[ idx];
	}

	Type& operator[]( std::size_t idx)
	{
		if (idx > size()/sizeof(Type))
		{
			throw std::logic_error( "Array bounds access");
		}
		return ((Type*)types::ArrayDoublingAllocator::base())[ idx];
	}

	std::size_t alloc( unsigned int nof)
	{
		std::size_t mm = nof * sizeof(Type);
		if (mm < nof) throw std::bad_alloc();
		std::size_t idx = types::ArrayDoublingAllocator::alloc( mm);
		return idx / sizeof(Type);
	}
};

}}//namespace
#endif


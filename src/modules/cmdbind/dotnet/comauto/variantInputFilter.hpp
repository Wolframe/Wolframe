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
//\file comauto/variantInputFilter.hpp
//\brief InputFilter implementation for MSDN variant type
#ifndef _Wolframe_COM_AUTOMATION_VARIANT_INPUT_FILTER_HPP_INCLUDED
#define _Wolframe_COM_AUTOMATION_VARIANT_INPUT_FILTER_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "serialize/mapContext.hpp"
#include <vector>
#include <objbase.h>

struct ITypeInfo;
struct IRecordInfo;
struct ITypeLib;
struct tagTYPEATTR;

namespace _Wolframe {
namespace comauto {

//\brief Forward declarations
class TypeLib;

class VariantInputFilter
	:public langbind::TypedInputFilter
{
public:
	VariantInputFilter( const comauto::TypeLib* typelib_, const ITypeInfo* typeinfo_, VARIANT data_, serialize::Context::Flags flags_);
	VariantInputFilter( const VariantInputFilter& o);

	virtual ~VariantInputFilter();
	virtual TypedInputFilter* copy() const;
	virtual bool getNext( ElementType& type, types::VariantConst& element);

private:
	enum State
	{
		VarOpen,
		VarClose
	};
	struct StackElem
	{
		State state;
		ITypeInfo* typeinfo;
		const IRecordInfo* recinfo;
		tagTYPEATTR* typeattr;
		VARIANT data;
		std::string name;
		std::size_t idx;

		StackElem( const StackElem& o);
		StackElem( const std::string& name_, const IRecordInfo* recinfo_, ITypeInfo* typeinfo_, VARIANT data_);
		~StackElem();
	};
	std::vector<StackElem> m_stk;
	std::string m_elembuf;
	const comauto::TypeLib* m_typelib;
	serialize::Context::Flags m_flags;
	bool m_done;
};

}} //namespace
#endif


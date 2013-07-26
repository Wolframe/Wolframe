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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file filter/filterbase.hpp
///\brief Common definitions of input and output filter

#ifndef _Wolframe_FILTER_TYPED_FILTERBASE_HPP_INCLUDED
#define _Wolframe_FILTER_TYPED_FILTERBASE_HPP_INCLUDED
#error DEPRECATED
#include <string>
#include <cstring>
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include "filter/filterbase.hpp"
#include "types/variant.hpp"

namespace _Wolframe {
namespace langbind {

//\class TypedFilterBase
///\brief Base class for input/output filters with typed elements
class TypedFilterBase :public FilterBase
{
public:
	TypedFilterBase()
		:types::TypeSignature("langbind::TypedFilterBase", __LINE__){}
	TypedFilterBase( const TypedFilterBase& o)
		:types::TypeSignature(o)
		,FilterBase(o){}

	///\class Element
	///\brief Element of the filter
	struct Element
	{
		Element()
			:type(string_)
		{
			value.string_.ptr = "";
			value.string_.size = 0;
		}

		explicit Element( const types::Variant& val)
			:type(string_)
		{
			switch (val.type())
			{
				case types::Variant::bool_:
					type = bool_;
					value.bool_ = val.tobool();
					return;
				case types::Variant::double_:
					type = double_;
					value.double_ = val.todouble();
					return;
				case types::Variant::int_:
					type = int_;
					value.int_ = val.toint();
					return;
				case types::Variant::uint_:
					type = uint_;
					value.uint_ = val.touint();
					return;
				case types::Variant::string_:
					type = string_;
					value.string_.ptr = val.charptr();
					value.string_.size = val.charsize();
					return;
			}
			throw std::logic_error( "illegal assignment (non atomic type)");
		}

		Element( const std::string& str)
			:type(string_)
		{
			value.string_.ptr = str.c_str();
			value.string_.size = str.size();
		}

		Element( const char* str, std::size_t len)
			:type(string_)
		{
			value.string_.ptr = str;
			value.string_.size = len;
		}

		Element( const char* str)
			:type(string_)
		{
			value.string_.ptr = str;
			value.string_.size = std::strlen(str);
		}

		Element( bool e)
			:type(bool_)
		{
			value.bool_ = e;
		}

		Element( double e)
			:type(double_)
		{
			value.double_ = e;
		}

		Element( int e)
			:type(int_)
		{
			value.int_ = e;
		}

		Element( unsigned int e)
			:type(uint_)
		{
			value.uint_ = e;
		}

		Element( const Element& o)
			:type(o.type)
			,value(o.value){}

		///\enum Type
		///\brief Typed filter element type
		enum Type
		{
			bool_,double_,int_,uint_,string_,blob_
		};
		///\struct String
		///\brief POD data type for string as part of an union
		struct String
		{
			std::size_t size;
			const char* ptr;
		};
		///\struct Blob
		///\brief POD data type for blob as part of an union
		struct Blob
		{
			std::size_t size;
			const void* ptr;
		};
		///\union Value
		///\brief Different incarnations of values depending of the elements type
		union Value
		{
			bool bool_;
			double double_;
			int int_;
			unsigned int uint_;
			String string_;
			Blob blob_;
		};
		Type type;			//< element type
		Value value;			//< element value

		const std::string tostring() const
		{
			switch (type)
			{
				case string_: return std::string( value.string_.ptr, value.string_.size);
				case blob_: return std::string( (const char *)value.blob_.ptr, value.blob_.size);
				case bool_: return value.bool_?"true":"false";
				case double_: return boost::lexical_cast<std::string>( value.double_);
				case int_: return boost::lexical_cast<std::string>( value.int_);
				case uint_: return boost::lexical_cast<std::string>( value.uint_);
			}
			throw std::runtime_error("illegal state");
		}

		bool empty() const
		{
			return (type == string_ && value.string_.size == 0);
		}

		bool emptycontent() const
		{
			if (type != string_) return false;
			for (std::size_t ii=0; ii<value.string_.size; ++ii)
			{
				if ((unsigned char)value.string_.ptr[ii] > 32) return false;
			}
			return true;
		}

		void clear()
		{
			type = string_;
			value.string_.size = 0;
			value.string_.ptr = "";
		}
	};
};

}}//namespace
#endif



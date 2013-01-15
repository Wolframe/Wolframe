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
///\file filter_null_filter.cpp
///\brief Filter implementation failing on read and write, but buffering one input chunk
#include "filter/null_filter.hpp"

using namespace _Wolframe;
using namespace langbind;

namespace {

///\class InputFilterImpl
///\brief input filter failing on read, but buffering one input chunk
struct InputFilterImpl :public InputFilter
{
	///\brief Constructor
	InputFilterImpl()
		:types::TypeSignature("langbind::InputFilterImpl (null_filter)", __LINE__)
		,m_src(0)
		,m_srcsize(0)
		,m_srcend(false){}

	///\brief Copy constructor
	///\param [in] o output filter to copy
	InputFilterImpl( const InputFilterImpl& o)
		:types::TypeSignature("langbind::InputFilterImpl (null_filter)", __LINE__)
		,InputFilter( o)
		,m_src(o.m_src)
		,m_srcsize(o.m_srcsize)
		,m_srcend(o.m_srcend){}

	///\brief self copy
	///\return copy of this
	virtual InputFilter* copy() const
	{
		return new InputFilterImpl( *this);
	}

	///\brief implement interface member InputFilter::putInput(const void*,std::size_t,bool)
	virtual void putInput( const void* ptr, std::size_t size, bool end)
	{
		m_src = (const char*)ptr;
		m_srcend = end;
		m_srcsize = size;
	}

	virtual void getRest( const void*& ptr, std::size_t& size, bool& end)
	{
		ptr = m_src;
		size = m_srcsize;
		end = m_srcend;
	}

	///\brief implement interface member InputFilter::getNext( typename InputFilter::ElementType&,const void*&,std::size_t&)
	virtual bool getNext( InputFilter::ElementType&, const void*&, std::size_t&)
	{
		setState( Error, "input filter not defined");
		return false;
	}

private:
	const char* m_src;			//< pointer to current chunk parsed
	std::size_t m_srcsize;			//< size of the current chunk parsed in bytes
	bool m_srcend;				//< true if end of message is in current chunk parsed
};

///\class OutputFilterImpl
///\brief output filter filter failing on write
struct OutputFilterImpl :public OutputFilter
{
	///\brief Constructor
	OutputFilterImpl()
		:types::TypeSignature("langbind::OutputFilterImpl (null_filter)", __LINE__){}

	///\brief Copy constructor
	///\param [in] o output filter to copy
	OutputFilterImpl( const OutputFilterImpl& o)
		:types::TypeSignature("langbind::OutputFilterImpl (null_filter)", __LINE__)
		,OutputFilter(o){}

	///\brief self copy
	///\return copy of this
	virtual OutputFilter* copy() const
	{
		return new OutputFilterImpl( *this);
	}

	///\brief Implementation of OutputFilter::print(OutputFilter::ElementType,const void*,std::size_t)
	///\return false with error always
	bool print( OutputFilter::ElementType, const void*, std::size_t)
	{
		setState( Error, "output filter not defined");
		return false;
	}
};
}//end anonymous namespace


struct NullFilter :public Filter
{
	NullFilter()
	{
		m_inputfilter.reset( new InputFilterImpl());
		m_outputfilter.reset( new OutputFilterImpl());
	}
};

Filter _Wolframe::langbind::createNullFilter( const std::string&, const std::string&)
{
	return NullFilter();
}

Filter* _Wolframe::langbind::createNullFilterPtr( const std::string& name, const std::string& arg)
{
	return new Filter( createNullFilter( name, arg));
}

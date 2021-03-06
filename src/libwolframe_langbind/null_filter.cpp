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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
//\file filter_null_filter.cpp
//\brief Filter implementation failing on read and write, but buffering one input chunk
#include "filter/null_filter.hpp"

using namespace _Wolframe;
using namespace langbind;

namespace {

//\class InputFilterImpl
//\brief input filter failing on read, but buffering one input chunk
struct InputFilterImpl :public InputFilter
{
	//\brief Constructor
	InputFilterImpl()
		:InputFilter("null")
		,m_src(0)
		,m_srcsize(0)
		,m_srcend(false){}

	//\brief Copy constructor
	//\param [in] o output filter to copy
	InputFilterImpl( const InputFilterImpl& o)
		:InputFilter( o)
		,m_src(o.m_src)
		,m_srcsize(o.m_srcsize)
		,m_srcend(o.m_srcend){}

	//\brief Implement InputFilter::copy()
	virtual InputFilter* copy() const
	{
		return new InputFilterImpl( *this);
	}

	//\brief Implement InputFilter::putInput(const void*,std::size_t,bool)
	virtual void putInput( const void* ptr, std::size_t size, bool end)
	{
		m_src = (const char*)ptr;
		m_srcend = end;
		m_srcsize = size;
	}

	//\brief Implement InputFilter::getNext( typename InputFilter::ElementType&,const void*&,std::size_t&)
	virtual bool getNext( InputFilter::ElementType&, const void*&, std::size_t&)
	{
		setState( Error, "input filter not defined");
		return false;
	}

	virtual const types::DocMetaData* getMetaData()
	{
		return getMetaDataRef().get();
	}

private:
	const char* m_src;			//< pointer to current chunk parsed
	std::size_t m_srcsize;			//< size of the current chunk parsed in bytes
	bool m_srcend;				//< true if end of message is in current chunk parsed
};

//\class OutputFilterImpl
//\brief output filter filter failing on write
struct OutputFilterImpl :public OutputFilter
{
	//\brief Constructor
	OutputFilterImpl()
		:OutputFilter("null"){}

	//\brief Copy constructor
	//\param [in] o output filter to copy
	OutputFilterImpl( const OutputFilterImpl& o)
		:OutputFilter(o){}

	//\brief self copy
	//\return copy of this
	virtual OutputFilter* copy() const
	{
		return new OutputFilterImpl( *this);
	}

	//\brief Implementation of OutputFilter::print(OutputFilter::ElementType,const void*,std::size_t)
	//\return false with error always
	bool print( OutputFilter::ElementType, const void*, std::size_t)
	{
		setState( Error, "output filter not defined");
		return false;
	}
	virtual void getOutput( const void*& buf, std::size_t& bufsize)
	{
		buf = 0;
		bufsize = 0;
	}

	virtual bool close(){return true;}
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

Filter _Wolframe::langbind::createNullFilter()
{
	return NullFilter();
}

Filter* _Wolframe::langbind::createNullFilterPtr( const std::string&, const std::vector<FilterArgument>& arg)
{
	if (!arg.empty())
	{
		throw std::runtime_error("no arguments expected for null filter");
	}
	return new Filter( createNullFilter());
}


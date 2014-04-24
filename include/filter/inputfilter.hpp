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
///\file filter/inputfilter.hpp
///\brief Interface for input filter

#ifndef _Wolframe_FILTER_INPUTFILTER_INTERFACE_HPP_INCLUDED
#define _Wolframe_FILTER_INPUTFILTER_INTERFACE_HPP_INCLUDED
#include "types/countedReference.hpp"
#include "types/doctype.hpp"
#include "filter/filterbase.hpp"
#include "filter/contentfilterAttributes.hpp"
#include <string>
#include <stdexcept>
#include <cstring>
#include <cstdlib>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

///\class InputFilter
///\brief Input filter
class InputFilter
	:public FilterBase, public ContentFilterAttributes
{
public:
	///\enum State
	///\brief State of the input filter
	enum State
	{
		Open,		//< serving data - normal input processing
		EndOfMessage,	//< have to yield processing because end of message reached
		Error		//< have to stop processing with an error
	};

	///\brief Default constructor
	explicit InputFilter( const char* name_)
		:utils::TypeSignature("langbind::InputFilter", __LINE__)
		,FilterBase(name_)
		,m_state(Open){}

	///\brief Copy constructor
	///\param[in] o input filter to copy
	InputFilter( const InputFilter& o)
		:utils::TypeSignature(o)
		,FilterBase(o)
		,ContentFilterAttributes()
		,m_state(o.m_state){}

	///\brief Destructor
	virtual ~InputFilter(){}

	///\brief Get a self copy
	///\return allocated pointer to copy of this
	virtual InputFilter* copy() const=0;

	///\brief Get an instance copy of this in its initial state
	///\return allocated pointer to an instance copy in its initial state
	virtual InputFilter* initcopy() const=0;

	///\brief Declare the next input chunk to the filter
	///\param [in] ptr the start of the input chunk
	///\param [in] size the size of the input chunk in bytes
	///\param [in] end true, if end of input (the last chunk) has been reached.
	virtual void putInput( const void* ptr, std::size_t size, bool end)=0;

	///\brief Get the rest of the input chunk left unparsed yet (defaults to nothing left)
	///\param [out] ptr the start of the input chunk left to parse
	///\param [out] size the size of the input chunk left to parse in bytes
	///\param [out] end true, if end of input (the last chunk) has been reached.
	virtual void getRest( const void*& ptr, std::size_t& size, bool& end)
	{
		ptr = 0;
		size = 0;
		end = false;
	}

	///\brief Implementation of FilterBase::getValue( const char*, std::string&)
	virtual bool getValue( const char* id, std::string& val) const
	{
		if (std::strcmp( id, "encoding") == 0)
		{
			const char* ee = getEncoding();
			if (ee)
			{
				val = ee;
				return true;
			}
			else
			{
				return false;
			}
		}
		return FilterBase::getValue( id, val);
	}

	///\brief Get next element
	///\param [out] type element type parsed
	///\param [out] element pointer to element returned
	///\param [out] elementsize size of the element returned
	///\return true, if success, false, if not.
	///\remark Check the filter state when false is returned
	virtual bool getNext( ElementType& type, const void*& element, std::size_t& elementsize)=0;

	///\brief Get type of the document
	///\param [out] doctype the document type returned
	///\return true, if success, false, if not.
	///\remark Check the filter state when false is returned
	virtual bool getDocType( types::DocType& doctype)
	{
		doctype = types::DocType();
		return true;
	}

	///\brief Evaluate if the document metadata are avalable and set state for fetching them if needed
	virtual bool getMetadata()
	{
		return true;
	}

	///\brief Get the current state
	///\return the current state
	State state() const				{return m_state;}

	///\brief Set input filter state with error message
	///\param [in] s new state
	///\param [in] msg (optional) error to set
	void setState( State s, const char* msg=0)	{m_state=s; setError(msg);}

private:
	State m_state;				//< state
};

///\typedef InputFilterR
///\brief Shared input filter reference
typedef types::CountedReference<InputFilter> InputFilterR;


}}//namespace
#endif



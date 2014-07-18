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
/// \file filter/filter.hpp
/// \brief Interface for filter class

#ifndef _Wolframe_FILTER_FILTER_INTERFACE_HPP_INCLUDED
#define _Wolframe_FILTER_FILTER_INTERFACE_HPP_INCLUDED
#include "types/countedReference.hpp"
#include "filter/inputfilter.hpp"
#include "filter/outputfilter.hpp"
#include <string>
#include <vector>
#include <utility>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

/// \class Filter
/// \brief Structure defining a filter for input and output as unit
class Filter
{
public:
	/// \brief Constructor
	Filter( const InputFilterR& i_, const OutputFilterR& o_)
		:m_inputfilter(i_),m_outputfilter(o_)
	{
		if (i_.get())
		{
			m_outputfilter->inheritMetaData( m_inputfilter->getMetaDataRef());
		}
	}
	/// \brief Copy constructor
	Filter( const Filter& o)
		:m_inputfilter(o.m_inputfilter),m_outputfilter(o.m_outputfilter){}
	/// \brief Default constructor
	Filter(){}

	/// \brief Get the associated input filter reference
	const InputFilterR& inputfilter() const		{return m_inputfilter;}
	/// \brief Get the associated output filter reference
	const OutputFilterR& outputfilter() const	{return m_outputfilter;}
	/// \brief Get the associated input filter reference
	InputFilterR& inputfilter()			{return m_inputfilter;}
	/// \brief Get the associated output filter reference
	OutputFilterR& outputfilter()			{return m_outputfilter;}

	/// \brief Get a member value of the filter
	/// \param [in] name case sensitive name of the variable
	/// \param [in] val the value returned
	/// \return true on success, false, if the variable does not exist or the operation failed
	bool getValue( const char* name, std::string& val) const
	{
		if (m_inputfilter.get() && m_inputfilter->getValue( name, val)) return true;
		if (m_outputfilter.get() && m_outputfilter->getValue( name, val)) return true;
		return false;
	}

	/// \brief Set a member value of the filter
	/// \param [in] name case sensitive name of the variable
	/// \param [in] value new value of the variable to set
	/// \return true on success, false, if the variable does not exist or the operation failed
	bool setValue( const char* name, const std::string& value)
	{
		bool rt = false;
		if (m_inputfilter.get() && m_inputfilter->setValue( name, value)) rt = true;
		if (m_outputfilter.get() && m_outputfilter->setValue( name, value)) rt = true;
		return rt;
	}

protected:
	InputFilterR m_inputfilter;
	OutputFilterR m_outputfilter;
};

typedef boost::shared_ptr<Filter> FilterR;


typedef std::pair<std::string,std::string> FilterArgument;

/// \class FilterType
/// \brief Structure defining a type of a filter (used as virtual constructor to create filter instances)
class FilterType
{
public:
	/// \brief Constructor
	explicit FilterType( const char* name_)
		:m_name(name_){}
	/// \brief Copy constructor
	FilterType( const FilterType& o)
		:m_name(o.m_name){}

	/// \brief Destructor
	virtual ~FilterType(){}
	/// \brief Get a new filter instance
	virtual Filter* create( const std::vector<FilterArgument>& arg = std::vector<FilterArgument>()) const=0;

	/// \brief Get the name of the filter
	const char* name() const
		{return m_name;}
private:
	const char* m_name;
};

typedef boost::shared_ptr<FilterType> FilterTypeR;

/// \brief Filter type constructor function
typedef FilterType* (*CreateFilterType)();

}}//namespace
#endif



/************************************************************************
Copyright (C) 2011 Project Wolframe.
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

#ifndef _Wolframe_FILTER_FILTERBASE_HPP_INCLUDED
#define _Wolframe_FILTER_FILTERBASE_HPP_INCLUDED
#include <string>

namespace _Wolframe {
namespace langbind {

///\class FilterBase
///\brief Base of input/ouput filter
class FilterBase
{
public:
	///\enum ElementType
	///\brief Content element type that describes the role of the element in the structured input
	enum ElementType
	{
		OpenTag,	//< open new hierarchy level
		Attribute,	//< attribute name
		Value,		//< content or attribute value
		CloseTag	//< close current hierarchy level
	};
	///\brief Get the name of an ElementType as string
	///\param[in] i an ElementType identifier
	///\return the name of an ElementType as string
	static const char* elementTypeName( ElementType i);

	///\brief Get the las error in case of error state
	///\return the error string or 0
	const char* getError() const;

	///\brief Set input filter error message
	///\param [in] msg (optional) error to set
	void setError( const char* msg=0);

	///\brief Get a member value of the filter
	///\param [in] name case sensitive name of the variable
	///\param [in] val buffer for the value returned
	///\return true on success, false, if the variable does not exist or the operation failed
	virtual bool getValue( const char* /*name*/, std::string& /*val*/) const	{return false;}

	///\brief Set a member value of the filter
	///\param [in] name case sensitive name of the variable
	///\param [in] val new value of the variable to set
	///\return true on success, false, if the variable does not exist or the operation failed
	virtual bool setValue( const char* /*name*/, const std::string& /*val*/)	{return false;}

private:
	enum {ErrorBufSize=128};		//< maximum size of error string
	char m_errorbuf[ ErrorBufSize];		//< error string
};

}}//namespace
#endif

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
///\file types/doctype.hpp
///\brief ADT for document type definition handling

#ifndef _Wolframe_TYPES_DOCTYPE_HPP_INCLUDED
#define _Wolframe_TYPES_DOCTYPE_HPP_INCLUDED
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace types {

struct DocType
{
	const char* rootid;
	const char* publicid;
	const char* systemid;

	DocType();
	DocType( const char* r, const char* p, const char* s)
		:rootid(r),publicid(p),systemid(s){}
	explicit DocType( const std::string& value);
	~DocType(){}

	std::string tostring() const;
private:
	boost::shared_ptr<void> mem;
};

std::string getIdFromDoctype( const std::string& doctype);

}}//namespace
#endif



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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file langbind/pipe.hpp
///\brief Interface for a pipe (istream|ostream) through wolframe mappings like filters, forms, functions
#ifndef _Wolframe_langbind_IOSTREAMFILTER_HPP_INCLUDED
#define _Wolframe_langbind_IOSTREAMFILTER_HPP_INCLUDED
#include <string>
#include <iostream>

namespace _Wolframe {
namespace langbind {

///\brief Maps input to output through a filter
///\param[in] ifl
///\param[in] ib
///\param[in] ofl
///\param[in] ob
///\param[in,out] in
///\param[in,out] out
int iostreamfilter( const std::string& ifl, std::size_t ib, const std::string& ofl, std::size_t ob, std::istream& in, std::ostream& out);

///\brief Maps input to output by a cmdbind::CommandHandler
///\param[in] ifl
///\param[in] ib
///\param[in] ofl
///\param[in] ob
///\param[in] proc
///\param[in,out] in
///\param[in,out] out
int iostreamfilter( const std::string& ifl, std::size_t ib, const std::string& ofl, std::size_t ob, const std::string& proc, std::istream& in, std::ostream& out);

}}//namespace
#endif

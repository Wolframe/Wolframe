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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file langbind/iostreamfilter.hpp
///\brief Interface for mapping istream to ostream by filters, forms, functions
#ifndef _Wolframe_langbind_IOSTREAMFILTER_HPP_INCLUDED
#define _Wolframe_langbind_IOSTREAMFILTER_HPP_INCLUDED
#include <string>
#include <iostream>
#include "processor/procProvider.hpp"

namespace _Wolframe {
namespace langbind {

///\brief Maps input to output through a filter and a conversion procedure.
///\remark throws exception on error
///\param[in] provider processor provider for the execution context
///\param[in] proc conversion procedure name. emty for no conversion
///\param[in] ifl input filter name
///\param[in] ib input buffer size
///\param[in] ofl output filter name
///\param[in] ob output buffer size
///\param[in,out] is input stream
///\param[in,out] os output stream
void iostreamfilter( proc::ProcessorProvider* provider, const std::string& proc, const std::string& ifl, std::size_t ib, const std::string& ofl, std::size_t ob, std::istream& is, std::ostream& os);

}}//namespace
#endif

/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
///\brief Interface for normalization programs
///\file langbind/normalizeProgram.hpp
#ifndef _LANGBIND_NORMALIZE_PROGRAM_HPP_INCLUDED
#define _LANGBIND_NORMALIZE_PROGRAM_HPP_INCLUDED
#include "langbind/normalizeFunction.hpp"
#include "types/keymap.hpp"
#include "module/normalizeFunctionBuilder.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

class NormalizeProgram
	:public types::keymap<NormalizeFunctionR>
{
public:
	typedef types::keymap<NormalizeFunctionR> Parent;

	NormalizeProgram(){}
	NormalizeProgram( const NormalizeProgram& o)
		:Parent(o){}
	~NormalizeProgram(){}

	bool is_mine( const std::string& filename) const;

	std::vector<std::pair<std::string,NormalizeFunctionR> >
		load( const std::string& source, const types::keymap<module::NormalizeFunctionConstructorR>& constructormap);

	std::vector<std::pair<std::string,NormalizeFunctionR> >
		loadfile( const std::string& filename, const types::keymap<module::NormalizeFunctionConstructorR>& constructormap);

	const NormalizeFunction* get( const std::string& name) const
	{
		Parent::const_iterator rt = Parent::find( name);
		return (rt == Parent::end())?0:rt->second.get();
	}

private:
	NormalizeFunction* createBaseFunction( const std::string& domain, const std::string& name, const std::string& arg, const types::keymap<module::NormalizeFunctionConstructorR>& constructormap);
};

}}
#endif



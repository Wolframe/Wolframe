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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
//\brief TEST REPLACEMENT OF Processor Provider
//\file processor/procProvider.hpp

#ifndef _PROCESSOR_PROVIDER_HPP_INCLUDED
#define _PROCESSOR_PROVIDER_HPP_INCLUDED
#include "langbind/formFunction.hpp"
#include "filter/typedfilter.hpp"
#include "filter/filter.hpp"
#include "types/normalizeFunction.hpp"
#include "types/form.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace proc {

/// Processor provider
class ProcessorProvider
{
public:
	ProcessorProvider(){}
	~ProcessorProvider(){}

	const types::NormalizeFunction* normalizeFunction( const std::string&) const
	{
		return 0;
	}

	const langbind::FormFunction* formFunction( const std::string& name) const
	{
		std::map<std::string,langbind::FormFunctionR>::const_iterator fi = m_functionmap.find( name);
		if (fi == m_functionmap.end()) return 0;
		return fi->second.get();
	}

	void defineFormFunction( const std::string& name_, const langbind::FormFunctionR& func_)
	{
		m_functionmap[ name_] = func_;
	}

	const types::FormDescription* formDescription( const std::string&) const
	{
		return 0;
	}

	langbind::Filter* filter( const std::string& , const std::vector<langbind::FilterArgument>&) const
	{
		return 0;
	}

	langbind::Filter* filter( const std::string&) const
	{
		return 0;
	}

private:
	std::map<std::string,langbind::FormFunctionR> m_functionmap;
};

}} // namespace _Wolframe::proc

#endif


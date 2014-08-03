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
///\brief Description structure of directmap programs
///\file directmapCommandDescription.hpp
#ifndef _LANGBIND_DIRECTMAP_COMMAND_DESCRIPTION_HPP_INCLUDED
#define _LANGBIND_DIRECTMAP_COMMAND_DESCRIPTION_HPP_INCLUDED
#include "filter/filterdef.hpp"
#include "types/keymap.hpp"
#include "types/docmetadata.hpp"
#include "types/form.hpp"
#include "processor/procProviderInterface.hpp"
#include "processor/execContext.hpp"
#include "filter/execContextInputFilter.hpp"
#include <string>

namespace _Wolframe {
namespace langbind {

struct DirectmapCommandDescription
{
	DirectmapCommandDescription()
		:function(0)
		,inputform(0)
		,outputform(0)
		,has_result(false)
		{}
	DirectmapCommandDescription( const DirectmapCommandDescription& o)
		:cmdname(o.cmdname)
		,functionname(o.functionname)
		,function(o.function)
		,inputfilterdef(o.inputfilterdef)
		,outputfilterdef(o.outputfilterdef)
		,inputform(o.inputform)
		,outputform(o.outputform)
		,outputmetadata(o.outputmetadata)
		,execContextElements(o.execContextElements)
		,authfunction(o.authfunction)
		,authresource(o.authresource)
		,has_result(o.has_result)
		{}

	std::string cmdname;						///< name of command (prefix + doctype)
	std::string functionname;					///< name of form function to execute
	const langbind::FormFunction* function;				///< the form function to execute
	langbind::FilterDef inputfilterdef;				///< the input filter definition
	langbind::FilterDef outputfilterdef;				///< the output filter definition
	const types::FormDescription* inputform;			///< the input form description
	const types::FormDescription* outputform;			///< the output form description
	types::DocMetaData outputmetadata;				///< metadata of the output 
	std::vector<langbind::ExecContextElement> execContextElements;	///< elements of the execution context passed to the function executed
	std::string authfunction;					///< authorization function name
	std::string authresource;					///< authorization resource name
	bool has_result;						///< true, if this command has a result

	std::string tostring() const;
};

typedef boost::shared_ptr<DirectmapCommandDescription> DirectmapCommandDescriptionR;

}}
#endif



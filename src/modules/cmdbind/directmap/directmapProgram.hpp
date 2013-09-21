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
///\brief Interface for directmap programs
///\file directmapProgram.hpp
#ifndef _LANGBIND_DIRECTMAP_PROGRAM_HPP_INCLUDED
#define _LANGBIND_DIRECTMAP_PROGRAM_HPP_INCLUDED
#include "types/keymap.hpp"
#include "processor/procProvider.hpp"
#include <string>

namespace _Wolframe {
namespace langbind {

struct DirectmapCommandDescription
{
	DirectmapCommandDescription(){}
	DirectmapCommandDescription( const DirectmapCommandDescription& o)
		:name(o.name)
		,call(o.call)
		,inputfilter(o.inputfilter)
		,outputfilter(o.outputfilter)
		,inputform(o.inputform)
		,outputform(o.outputform){}

	std::string name;						//< name of program
	std::string call;						//< name of the transaction or form function
	std::string inputfilter;					//< name of the input filter
	std::string outputfilter;					//< name of the output filter
	std::string inputform;						//< name of the input form
	std::string outputform;						//< name of the output form

	std::string tostring() const;
};

///\class DirectmapProgram
///\brief Program describing direct mappings (many function descriptions per source file)
//	Function description syntax: identifier = call( filter :inputform) :outputform;
//
class DirectmapProgram
	:public types::keymap<DirectmapCommandDescription>
{
public:
	typedef types::keymap<DirectmapCommandDescription> Parent;

	DirectmapProgram(){}
	DirectmapProgram( const DirectmapProgram& o)
		:Parent(o){}
	~DirectmapProgram(){}

	bool is_mine( const std::string& filename) const;
	void addProgram( const std::string& source);
	void loadProgram( const std::string& filename);

	///\brief Check that all functions configured exists
	bool check( const proc::ProcessorProvider* provider) const;

	const DirectmapCommandDescription* get( const std::string& name) const
	{
		Parent::const_iterator rt = Parent::find( name);
		return (rt == Parent::end())?0:&rt->second;
	}
};

}}
#endif



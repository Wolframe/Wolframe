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
///\brief Implementation of preprocessing function calls
///\file transactionfunction/PreProcessCommand.cpp
#include "transactionfunction/PreProcessCommand.hpp"

using namespace _Wolframe;
using namespace _Wolframe::db;

void PreProcessCommand::call( const proc::ProcessorProvider* provider, TransactionFunctionInput::Structure& structure) const
{
	// Select the nodes to execute the command with:
	typedef TransactionFunctionInput::Structure::Node Node;
	typedef TransactionFunctionInput::Structure::NodeAssignment NodeAssignment;
	std::vector<const Node*> nodearray;
	m_selector.selectNodes( structure, structure.root(), nodearray);

	std::vector<const Node*>::const_iterator ni = nodearray.begin(), ne = nodearray.end();
	for (; ni != ne; ++ni)
	{
		// Build the parameter structure:
		std::vector<NodeAssignment> parameterassign;
		std::vector<const Node*> parameter;
		std::vector<Argument>::const_iterator ai = m_args.begin(), ae = m_args.end();
		std::size_t aidx = 0;
		for (; ai != ae; ++ai,++aidx)
		{
			ai->selector.selectNodes( structure, *ni, parameter);
			if (parameter.size() != aidx)
			{
				if (parameter.size() < aidx)
				{
					parameter.push_back( 0);
				}
				else
				{
					throw std::runtime_error( std::string( "referenced parameter in preprocessor call is not unique for parameter '") + ai->name + "' in call of '" + m_name +"'");
				}
			}
			parameterassign.push_back( NodeAssignment( ai->name, parameter.back()));
		}
		langbind::TypedInputFilterR argfilter( structure.createFilter( parameterassign));

		// Call the function:
	}
}


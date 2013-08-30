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
#include "langbind/appObjects.hpp"
#include "logger-v1.hpp"
#include <map>

using namespace _Wolframe;
using namespace _Wolframe::db;

static types::VariantConst
	expectArg( langbind::TypedInputFilter* i, langbind::InputFilter::ElementType et, const char* errmsg)
{
	langbind::InputFilter::ElementType it;
	types::VariantConst iv;

	if (i->getNext( it, iv))
	{
		if (it != et) throw std::runtime_error( errmsg);
	}
	else
	{
		const char* err = i->getError();
		throw std::runtime_error( err?err:"unexpected end of input");
	}
	return iv;
}

static void mapResult( langbind::TypedInputFilter* in, langbind::TypedOutputFilter* out)
{
	int taglevel = 0;
	langbind::InputFilter::ElementType et;
	types::VariantConst ev;

	for (;;)
	{
		if (!in->getNext( et, ev))
		{
			const char* err = in->getError();
			throw std::runtime_error( err?err:"unexpected end of message in result");
		}
		if (et == langbind::InputFilter::OpenTag)
		{
			++taglevel;
		}
		else if (et == langbind::InputFilter::CloseTag)
		{
			--taglevel;
			if (taglevel < 0) return;
		}
		if (!out->print( et, ev))
		{
			const char* err = out->getError();
			throw std::runtime_error( err?err:"unknown error in print result");
		}
	}
}

void PreProcessCommand::call( const proc::ProcessorProvider* provider, TransactionFunctionInput::Structure& structure) const
{
	// Select the nodes to execute the command with:
	typedef TransactionFunctionInput::Structure Structure;
	typedef Structure::Node Node;
	typedef Structure::NodeAssignment NodeAssignment;
	typedef Structure::NodeVisitor NodeVisitor;
	std::map<const Node*, int> selectmap;
	std::map<int, bool> sourccetagmap;

	const types::NormalizeFunction* nf = 0;
	const langbind::FormFunction* ff = 0;
	try
	{
		ff = provider->formFunction( m_name);
		if (!ff) nf = provider->normalizeFunction( m_name);

		std::vector<const Node*> nodearray;
		m_selector.selectNodes( structure, structure.root(), nodearray);

		std::vector<const Node*>::const_iterator ni = nodearray.begin(), ne = nodearray.end();
		for (; ni != ne; ++ni)
		{
			if (selectmap[*ni]++ > 0)
			{
				LOG_WARNING << "duplicate selection of node '" << structure.nodepath( *ni) << "' (processing only first selection)";
				continue;
			}
			// [1A] Create the destination node for the result:
			NodeVisitor resultnode = structure.visitor( *ni);
			if (!m_resultpath.empty())
			{
				if (m_resultpath.size() > 1)
				{
					std::vector<std::string>::const_iterator ri = m_resultpath.begin(), re = m_resultpath.begin() + (m_resultpath.size() - 1);
					for (; ri != re; ++ri)
					{
						resultnode = structure.visitTag( resultnode, *ri);
					}
					resultnode = structure.visitOrOpenUniqTag( resultnode, *ri);
				}
			}
			// [1B] Create the map of illegal result tags (result with tag names occurring in the input are not allowed to avoid anomalies)
			const Node* rn = structure.node( resultnode);
			if (rn->m_firstchild)
			{
				rn = structure.node( rn->m_firstchild);
				for (;;)
				{
					sourccetagmap[ rn->m_tagstr] = true;
					if (!rn->m_next) break;
					rn = structure.node( rn->m_next);
				}
			}
			// [2] Build the parameter structure:
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
						throw std::runtime_error( std::string( "referenced parameter '") + ai->name + "' is not unique");
					}
				}
				parameterassign.push_back( NodeAssignment( ai->name, parameter.back()));
			}
			langbind::TypedInputFilterR argfilter( structure.createInputFilter( parameterassign));

			// [3] Call the function:
			if (nf)
			{
				// call normalize function:
				langbind::InputFilter::ElementType et;
				types::VariantConst ev;
				bool haveInput = true;
				const char* errmsg = "atomic value (pure or with single tag) expected as input of normalization function (got structure instead)";

				if (argfilter->getNext( et, ev))
				{
					if (et == langbind::InputFilter::OpenTag)
					{
						// we have a structure with one tag with content value, so we run the function with the value as argument:
						ev = expectArg( argfilter.get(), langbind::InputFilter::Value, errmsg);
						expectArg( argfilter.get(), langbind::InputFilter::CloseTag, errmsg);
					}
					else if (et == langbind::InputFilter::Attribute)
					{
						// we have a structure with one attribute, so we run the function with the value as argument:
						ev = expectArg( argfilter.get(), langbind::InputFilter::Value, errmsg);
					}
					else if (et == langbind::InputFilter::Value)
					{
						// we have an atomic value ....
					}
					else if (et == langbind::InputFilter::CloseTag)
					{
						// we have a NULL value, so we do not execute the function:
						haveInput = false;
					}
				}
				if (haveInput)
				{
					expectArg( argfilter.get(), langbind::InputFilter::CloseTag, errmsg);
					structure.pushValue( resultnode, nf->execute( ev));
				}
			}
			else if (ff)
			{
				// call form function:
				langbind::FormFunctionClosureR fc( ff->createClosure());
				serialize::Context::Flags f = serialize::Context::None;

				if (!structure.case_sensitive())
				{
					f = (serialize::Context::Flags)((int)f | (int)serialize::Context::CaseInsensitiveCompare);
				}
				fc->init( provider, argfilter, f);
				if (!fc->call())
				{
					const char* err = argfilter->getError();
					if (!err) throw std::logic_error( "internal: incomplete input");
					throw std::runtime_error( err);
				}
				// assign form function result to destination in input structure for further processing:
				langbind::TypedOutputFilterR resfilter( structure.createOutputFilter( resultnode, sourccetagmap));
				langbind::TypedInputFilterR result = fc->result();

				result->setFlags( langbind::TypedInputFilter::SerializeWithIndices);
				// ... result should provide indices of arrays is possible (for further preprocessing function calls)

				mapResult( result.get(), resfilter.get());
			}
			else
			{
				throw std::runtime_error( "function not defined");
			}
		}
	}
	catch (std::runtime_error& e)
	{
		throw std::runtime_error( std::string( "failed to call transaction preprocessing function '") + m_name +"': " + e.what());
	}
}


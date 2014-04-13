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
///\brief Implementing of the methods of the transaction function preprocessing step based on TDL
///\file tdlTransactionPreprocStep.cpp
#include "tdlTransactionPreprocStep.hpp"
#include "vm/tagTable.hpp"
#include "vm/inputStructure.hpp"
#include "langbind/formFunction.hpp"
#include "filter/typedfilter.hpp"
#include "types/normalizeFunction.hpp"
#include "types/variant.hpp"
#include "processor/procProviderInterface.hpp"
#include "serialize/mapContext.hpp"
#include "logger-v1.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using namespace _Wolframe;
using namespace _Wolframe::db;

std::string TdlTransactionPreprocStep::tostring( const vm::TagTable* tagmap) const
{
	std::ostringstream out;
	print( out, tagmap);
	return out.str();
}

void TdlTransactionPreprocStep::print( std::ostream& out, const vm::TagTable* tagmap) const
{
	out << "INTO " ;
	std::vector<std::string>::const_iterator ri = m_resultpath.begin(), re = m_resultpath.end();
	if (m_resultpath.empty()) out << ".";
	for (; ri != re; ++ri)
	{
		if (ri != m_resultpath.begin()) out << "/";
		out << *ri;
	}
	out << " FOREACH ";
	selector().print( out, tagmap);
	out << " DO " << function() << "(";
	std::size_t ai = 0, ae = m_arguments.size();
	for (; ai != ae; ++ai)
	{
		if (ai != 0) out << ", ";
		switch (arg_type( ai))
		{
			case Argument::SelectorPath:
				if (arg_name(ai)) out << arg_name(ai) << "=";
				arg_selector(ai).print( out, tagmap);
				break;
			case Argument::LoopCounter:
				if (arg_name(ai)) out << arg_name(ai) << "=";
				out << "#";
				break;
			case Argument::Constant:
				if (arg_name(ai)) out << arg_name(ai) << "=";
				out << "\"" << arg_constant(ai) << "\"";
				break;
		}
	}
	out << ")";
}

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

void TdlTransactionPreprocStep::call( const proc::ProcessorProviderInterface* provider, vm::InputStructure& structure) const
{
	// Select the nodes to execute the command with:
	std::map<vm::InputNodeIndex, int> selectmap;
	std::map<int, bool> sourccetagmap;

	const types::NormalizeFunction* nf = 0;
	const langbind::FormFunction* ff = 0;
	try
	{
		ff = provider->formFunction( m_function);
		if (!ff) nf = provider->normalizeFunction( m_function);

		std::vector<vm::InputNodeIndex> nodearray;
		selector().selectNodes( structure, structure.rootindex(), nodearray);
		LOG_DATA << "[transaction preprocess] input structure: " << structure.tostring( structure.rootvisitor(), utils::logPrintFormat());
		if (nodearray.size())
		{
			LOG_DATA << "[transaction preprocess] execute function " << m_function << " on " << nodearray.size() << " nodes of selection '" << structure.nodepath( *nodearray.begin()) << "'";
		}
		else
		{
			LOG_DATA << "[transaction preprocess] empty selection (no execution) for function " << m_function;
		}
		std::vector<vm::InputNodeIndex>::const_iterator ni = nodearray.begin(), ne = nodearray.end();
		for (; ni != ne; ++ni)
		{
			if (selectmap[*ni]++ > 0)
			{
				LOG_WARNING << "duplicate selection of node '" << structure.nodepath( *ni) << "' (processing only first selection)";
				continue;
			}
			// [1A] Create the destination node for the result:
			vm::InputNodeVisitor resultnode( *ni);
			if (!m_resultpath.empty())
			{
				if (m_resultpath.size() > 1)
				{
					std::vector<std::string>::const_iterator ri = m_resultpath.begin(), re = m_resultpath.begin() + (m_resultpath.size() - 1);
					for (; ri != re; ++ri)
					{
						resultnode = structure.visitTag( resultnode, *ri);
					}
				}
				if (m_resultpath.back() != ".")
				{
					resultnode = structure.visitOrOpenUniqTag( resultnode, m_resultpath.back());
				}
			}
			// [1B] Create the map of illegal result tags (result with tag names occurring in the input are not allowed to avoid anomalies)
			const vm::InputNode* rn = structure.node( resultnode);
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
			std::vector<vm::InputStructure::NodeAssignment> parameterassign;
			std::vector<vm::InputNodeIndex> parameter;
			std::vector<Argument>::const_iterator ai = m_arguments.begin(), ae = m_arguments.end();
			std::size_t aidx = 0;

			for (; ai != ae; ++ai,++aidx)
			{
				switch (ai->type)
				{
					case Argument::SelectorPath:
						m_selectors.at( m_arguments.at(aidx).value).selectNodes( structure, *ni, parameter);
						if (parameter.size() == aidx+1)
						{
							LOG_DATA << "[transaction preprocess] argument '" << (m_strings.c_str()+ai->name) << "' in '" << structure.nodepath( parameter.back()) << "' = " << structure.tostring( parameter.back(), utils::logPrintFormat());
							parameterassign.push_back( vm::InputStructure::NodeAssignment( (m_strings.c_str()+ai->name), parameter.back()));
						}
						else
						{
							if (parameter.size() < aidx+1)
							{
								LOG_DATA << "[transaction preprocess] argument '" << ai->name << "' = NULL";
								parameter.push_back( 0); //... NULL node
							}
							else
							{
								throw std::runtime_error( std::string( "referenced parameter '") + (m_strings.c_str()+ai->name) + "' is not unique");
							}
						}
						break;
					case Argument::LoopCounter:
						throw std::runtime_error("not implemented yet: Loop counter as parameter of TDL preprocessing step");
					case Argument::Constant:
						throw std::runtime_error("not implemented yet: Constant as parameter of TDL preprocessing step");
				}
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
					const types::Variant res = nf->execute( ev);
					if (m_resultpath.empty())
					{
						LOG_DATA << "[transaction preprocess] call validator '" << m_function << "'";
					}
					else
					{
						LOG_DATA << "[transaction preprocess] call normalizer '" << m_function << "' into '" << structure.nodepath( resultnode) << "' => " << res.typeName() << " '" << res.tostring() << "'";
						structure.pushValue( resultnode, res);
					}
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
				if (m_resultpath.empty())
				{
					LOG_DATA << "[transaction preprocess] call function " << m_function << " ignoring the result (no INTO declaration)";
				}
				else
				{
					// assign form function result to destination in input structure for further processing:
					langbind::TypedOutputFilterR resfilter( structure.createOutputFilter( resultnode, sourccetagmap));
					langbind::TypedInputFilterR result = fc->result();

					result->setFlags( langbind::TypedInputFilter::SerializeWithIndices);
					// ... result should provide indices of arrays is possible (for further preprocessing function calls)

					mapResult( result.get(), resfilter.get());
					LOG_DATA << "[transaction preprocess] call function " << m_function << " => " << structure.tostring( resultnode, utils::logPrintFormat());
				}
			}
			else
			{
				throw std::runtime_error( "function not defined");
			}
		}
	}
	catch (std::runtime_error& e)
	{
		throw std::runtime_error( std::string( "failed to call TDL preprocessing function '") + m_function +"': " + e.what());
	}
}


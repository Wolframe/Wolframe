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
///\file mylangFunctionProgramType.cpp
///\brief Implementation of the function to create a form function program type object for mylang scripts
#include "mylangFunctionProgramType.hpp"
#include "mylangFunctionCall.hpp"
#include "langbind/formFunction.hpp"
#include "processor/procProvider.hpp"
#include "logger/logger-v1.hpp"
#include "types/countedReference.hpp"
#include "types/variant.hpp"
#include <vector>
#include <string>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

namespace {

///\class MyLangResult
///\brief Structure representing the result of a function call
class MyLangResult
	:public TypedInputFilter
{
public:
	MyLangResult( const mylang::StructureR& data_)
		:types::TypeSignature("langbind::MyLangResult", __LINE__)
		,m_data(data_){}

	MyLangResult( const MyLangResult& o)
		:types::TypeSignature("langbind::MyLangResult", __LINE__)
		,TypedInputFilter(o)
		,m_data(o.m_data){}

	virtual TypedInputFilter* copy() const
	{
		return new MyLangResult( *this);
	}

	///\brief Get next element
	///\param [out] type element type parsed
	///\param [out] element reference to element returned
	///\return true, throws on error
	virtual bool getNext( TypedInputFilter::ElementType& type, types::VariantConst& element)
	{
		if (!m_stk.empty() && m_stk.back().itr == m_stk.back().end)
		{
			m_stk.pop_back();
			if (!m_stk.back().tag.defined())
			{
				// ... if tag is defined then Open/Close was printed with every (array-) element
				type = InputFilter::CloseTag;
				element.clear();
			}
			return true;
		}
		if (m_stk.empty())
		{
			type = InputFilter::CloseTag;
			element.clear();
			return true;
		}
		for (;;)
		{
			if (m_bufidx < m_buf.size())
			{
				type = m_buf.at( m_bufidx).first;
				element = m_buf.at( m_bufidx).second;
				++m_bufidx;
			}
			m_bufidx = 0;
			m_buf.clear();
			if (m_stk.back().itr->second->atomic())
			{
				if (m_stk.back().tag.defined())
				{
					m_buf.push_back( BufElem( InputFilter::OpenTag, m_stk.back().tag));
				}
				else
				{
					m_buf.push_back( BufElem( InputFilter::OpenTag, m_stk.back().itr->first));
				}
				m_buf.push_back( BufElem( InputFilter::Value, m_stk.back().itr->second->getValue()));
				m_buf.push_back( BufElem( InputFilter::CloseTag, types::Variant()));
				m_stk.back().itr++;
			}
			else
			{
				types::Variant tag;
				if (m_stk.back().itr->second->array() && !flag( TypedInputFilter::SerializeWithIndices))
				{
					if (m_stk.back().tag.defined())
					{
						throw std::runtime_error("illegal structure: array of array");
					}
					tag = m_stk.back().itr->first;
					// ... if tag is defined then Open/Close will be printed with every (array-) element
					//	and here we do not print an 'Open'
				}
				else
				{
					if (m_stk.back().tag.defined())
					{
						m_buf.push_back( BufElem( InputFilter::OpenTag, m_stk.back().tag));
					}
					else
					{
						m_buf.push_back( BufElem( InputFilter::OpenTag, m_stk.back().itr->first));
					}
					// ... if tag is not defined then an 'Open' is printed
					//	and a final 'Close' will be printed when the structure is popped from the stack.
				}
				mylang::Structure::const_iterator citr = m_stk.back().itr++;
				m_stk.push_back( StackElem( citr, m_stk.back().itr->second->end(), tag));
			}
		}
	}

private:
	const mylang::StructureR m_data;
	typedef std::pair<TypedInputFilter::ElementType, types::Variant> BufElem;
	std::vector<BufElem> m_buf;
	std::size_t m_bufidx;
	struct StackElem
	{
		mylang::Structure::const_iterator itr;
		mylang::Structure::const_iterator end;
		types::Variant tag;

		StackElem(){}
		StackElem( const mylang::Structure::const_iterator& itr_, const mylang::Structure::const_iterator& end_, const types::Variant& tag_)
			:itr(itr_),end(end_),tag(tag_){}
		StackElem( const StackElem& o)
			:itr(o.itr),end(o.end),tag(o.tag){}
	};
	std::vector<StackElem> m_stk;
};


///\class MylangFormFunctionClosure
///\brief Wrapper to make an interpreter call appear as form function call
class MylangFormFunctionClosure
	:public langbind::FormFunctionClosure
{
public:
	MylangFormFunctionClosure( const std::string& name_, const mylang::InstanceR& interp_)
		:m_name(name_),m_initialized(false),m_interp(interp_),m_provider(0){}

	virtual ~MylangFormFunctionClosure(){}

	std::string currentElementPath()
	{
		std::string rt;
		std::vector<InitStackElem>::const_iterator ci = m_initStmStack.begin(), ce = m_initStmStack.end();
		for (; ci != ce; ++ci)
		{
			if (rt.size()) rt.append( "/");
			if (ci->first.defined()) rt.append( ci->first.tostring());
		}
		return rt;
	}

	virtual bool call()
	{
		if (m_initialized)
		{
			try
			{
				InputFilter::ElementType type;
				types::VariantConst elem;
				while (!m_initialized && m_arg->getNext( type, elem))
				{
					switch (type)
					{
						case InputFilter::OpenTag:
						{
							mylang::Structure* substruct = m_initStmStack.back().second->addSubstruct( elem);
							m_initStmStack.push_back( InitStackElem( elem, substruct));
							break;
						}
						case InputFilter::Attribute:
							m_tagbuf = elem;
							break;

						case InputFilter::Value:
							if (m_tagbuf.defined())
							{
								mylang::Structure* substruct = m_initStmStack.back().second->addSubstruct( m_tagbuf);
								substruct->setValue( elem);
							}
							else
							{
								m_initStmStack.back().second->setValue( elem);
							}
							m_tagbuf.clear();
							break;

						case InputFilter::CloseTag:
							m_initStmStack.pop_back();
							if (m_initStmStack.empty())
							{
								m_initialized = true;
							}
							break;
					}
				}
			}
			catch (const std::runtime_error& e)
			{
				throw std::runtime_error( std::string( "error function call ") + m_name + " in parameter at " + currentElementPath() + ": " + e.what());
			}
			if (!m_initialized) return false;
		}
		m_output = mylang::call( m_provider, m_input);
		m_result.reset( new MyLangResult( m_output));
		return true;
	}

	virtual void init( const proc::ProcessorProvider* provider, const TypedInputFilterR& arg, serialize::Context::Flags /*f*/)
	{
		m_provider = provider;
		m_arg = arg;
		if (m_interp->needsArrayIndices())
		{
			m_arg->setFlags( TypedInputFilter::SerializeWithIndices);
			//... call this for languages that need arrays to be serialized with indices if available
		}
		m_initialized = false;
		m_initStmStack.clear();
		m_input.reset( new mylang::Structure( m_interp));
		m_initStmStack.push_back( InitStackElem( types::Variant(), m_input.get()));
	}

	virtual TypedInputFilterR result() const
	{
		return m_result;
	}

private:
	TypedInputFilterR m_result;			//< result of the function call
	std::string m_name;				//< name of the function called for error messages
	TypedInputFilterR m_arg;			//< call argument as input filter
	bool m_initialized;				//< true, if the input has been initialized
	typedef std::pair<types::Variant,mylang::Structure*> InitStackElem;
	std::vector<InitStackElem> m_initStmStack;	//< Stack for substructure initialization
	types::Variant m_tagbuf;			//< buffer for attribute name to handle Attribute,Value pair
	mylang::StructureR m_input;			//< pointer to input structure
	mylang::StructureR m_output;			//< pointer to output structure
	mylang::InstanceR m_interp;			//< interpreter instance
	const proc::ProcessorProvider* m_provider;	//< pointer to processor provider
};


///\class MylangFormFunction
///\brief Wrapper to make an mylang function appear as function that can be called by any other language binding
class MylangFormFunction
	:public langbind::FormFunction
{
public:
	MylangFormFunction( const mylang::Context* context_, const std::string& name_)
		:m_context(context_),m_name(name_){}

	virtual ~MylangFormFunction(){}

	virtual FormFunctionClosure* createClosure() const
	{
		mylang::InstanceR interp = m_context->getInstance( m_name);
		if (!interp.get()) return 0;
		return new MylangFormFunctionClosure( m_name, interp);
	}

private:
	const mylang::Context* m_context;
	std::string m_name;
};

///\class MylangProgramType
///\brief Program type of mylang programs
class MylangProgramType
	:public prgbind::Program
{
public:
	MylangProgramType()
		:prgbind::Program( prgbind::Program::Function){}

	virtual ~MylangProgramType(){}

	virtual bool is_mine( const std::string& filename) const
	{
		boost::filesystem::path p( filename);
		return p.extension().string() == ".mlg";
	}

	virtual void loadProgram( prgbind::ProgramLibrary& library, db::Database* /*transactionDB*/, const std::string& filename)
	{
		std::vector<std::string> funcs = m_context.loadProgram( filename);
		std::vector<std::string>::const_iterator fi = funcs.begin(), fe = funcs.end();
		for (; fi != fe; ++fi)
		{
			langbind::FormFunctionR ff( new MylangFormFunction( &m_context, *fi));
			library.defineFormFunction( *fi, ff);
		}
	}

private:
	mylang::Context m_context;
};
}//anonymous namespace

prgbind::Program* langbind::createMylangProgramType()
{
	return new MylangProgramType();
}



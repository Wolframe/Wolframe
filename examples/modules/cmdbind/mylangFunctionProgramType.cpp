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

class MyLangInstance;

///\class MyLangResult
///\brief Structure representing the result of a function call
class MyLangResult
	:public TypedInputFilterR
{
public:
	MyLangResult( const MyLangInstance* i);
	MyLangResult( const MyLangResult& o);

	virtual TypedInputFilter* copy() const
	{
		return new MyLangResult( *this);
	}

	///\brief Get next element
	///\param [out] type element type parsed
	///\param [out] element reference to element returned
	///\return true, throws on error
	virtual bool getNext( ElementType& type, types::VariantConst& element)
	{
		/// IMPLEMENT ITERATOR ON THE RESULT HERE
	}
};

///\class MyLangResult
///\brief Structure representing one instance of an intepreter built to execute one function call
class MyLangInstance
{
public:
	MyLangInstance()
		:m_structptr(0){}

	MyLangInstance( const MyLangInstance&){}


	///\brief Methods to build the input structure native for the called language (pass parameters)
	void init( const proc::ProcessorProvider* provider);
	void setValue( StructPointer* obj, const types::Variant& value);
	void setValue( StructPointer* obj, const types::Variant& tag, const types::Variant& value);

	///\brief Function call with all parameters initialized
	void call()
	{
		m_output = callMylangFunction( m_provider, m_input);
	}

	///\brief Fetch the function call result object
	TypedInputFilterR result()
	{
		return TypedInputFilterR( new MyLangResult( m_output));
	}

	StructReference inputdata() const
	{
		return StructReference( types::Variant(), m_input);
	}

private:
	const proc::ProcessorProvider* m_provider;	//< pointer to provider
	StructReference* m_input;			//< pointer to input structure
	StructReference* m_output;			//< pointer to output structure
};

typedef types::CountedReference<MyLangInstance> MyLangInstanceR;

///\class MyLangContext
///\brief Global context with all data structures needed to create interpreter instances addressed by function names
class MyLangContext
{
	MyLangContext();
	std::vector<std::string> loadProgram( const std::string& name);
	MyLangInstanceR getInstance( const std::string& name);
};


///\class MylangFormFunctionClosure
///\brief Wrapper to make an interpreter call appear as form function call
class MylangFormFunctionClosure
	:public langbind::FormFunctionClosure
{
public:
	MylangFormFunctionClosure( const MyLangInstanceR& interp_, const std::string& name_)
		:m_interp(interp_),m_name(name_),m_initialized(false),m_taglevel(0){}

	virtual ~MylangFormFunctionClosure(){}

	std::string currentElementPath()
	{
		std::string rt;
		std::vector<StructReference>::const_iterator ci = m_initStmStack.begin(), ce = m_initStmStack.end();
		for (; ci != ce; ++ci)
		{
			if (rt.size()) rt.append("/");
			if (ci->id.defined()) rt.append( ci->id.tostring());
		}
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
							StructPointer* substruct = m_initStmStack.back().getSubstruct( elem);
							if (!substruct) throw std::runtime_error( std::string("input element not defined '") + elem.tostring() + "'");
							m_initStmStack.push_back( MyLangInstance::StructReference( elem, substruct));
							break;
						}
						case InputFilter::Attribute:
							m_tagbuf = elem;
							break;

						case InputFilter::Value:
							if (m_tagbuf.defined())
							{
								m_interp->setValue( m_initStmStack.back().ptr, m_tagbuf, elem);
							}
							else
							{
								m_interp->setValue( m_initStmStack.back().ptr, elem);
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
				throw std::runtime_error( std::string( "error in parameter at ") + currentElementPath() + ": " + e.what());
			}
			if (!m_initialized) return false;
		}
		m_interp->call();
		m_result = m_interp->result();
		return true;
	}

	virtual void init( const proc::ProcessorProvider* provider, const TypedInputFilterR& arg, serialize::Context::Flags /*f*/)
	{
		m_interp->init( provider);
		m_arg = arg;
		// m_arg->setFlags( TypedInputFilter::SerializeWithIndices);
		//... call this for languages that need arrays to be serialized with indices if available
		m_initialized = false;
		m_initStmStack.clear();
		m_initStmStack.push_back( m_interp->inputdata());
	}

	virtual TypedInputFilterR result() const
	{
		return m_result;
	}

private:
	MyLangInstanceR m_interp;
	TypedInputFilterR m_result;
	std::string m_name;
	TypedInputFilterR m_arg;
	bool m_initialized;
	std::vector<StructReference> m_initStmStack;
	types::Variant m_tagbuf;
};


///\class MylangFormFunction
///\brief Wrapper to make an mylang function appear as function that can be called by any other language binding
class MylangFormFunction
	:public langbind::FormFunction
{
public:
	MylangFormFunction( const MylangContext* context_, const std::string& name_)
		:m_context(context_),m_name(name_){}

	virtual ~MylangFormFunction(){}

	virtual FormFunctionClosure* createClosure() const
	{
		MyLangInstanceR interp = getInstance( m_name);
		if (!interp->get()) return 0;
		return new MylangFormFunctionClosure( interp, m_name);
	}

private:
	const MylangContext* m_context;
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
	MylangContext m_context;
};
}//anonymous namespace

prgbind::Program* langbind::createMylangProgramType()
{
	return new MylangProgramType();
}



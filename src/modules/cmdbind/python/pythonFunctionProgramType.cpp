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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file pythonFunctionProgramType.cpp
///\brief Implementation of the function to create a form function program type object for python scripts
#include "pythonInterpreter.hpp"
#include "pythonFunctionProgramType.hpp"
#include "pythonStructureBuilder.hpp"
#include "pythonObject.hpp"
#include "langbind/formFunction.hpp"
#include "prgbind/programLibrary.hpp"
#include "processor/procProviderInterface.hpp"
#include "logger-v1.hpp"
#include "types/countedReference.hpp"
#include "types/variant.hpp"
#include <vector>
#include <string>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

namespace {

///\class PythonResult
///\brief Structure representing the result of a function call
class PythonResult
	:public TypedInputFilter
{
public:
	PythonResult( const python::StructureR& data_)
		:utils::TypeSignature("langbind::PythonResult", __LINE__)
		,TypedInputFilter("pythonresult")
		,m_data(data_)
		,m_bufidx(0)
	{
		if (m_data.get())
		{
			m_stk.push_back( StackElem( *m_data, -1));
		}
		else
		{
			m_buf.push_back( BufElem( InputFilter::CloseTag, types::Variant()));
		}
	}

	PythonResult( const PythonResult& o)
		:utils::TypeSignature("langbind::PythonResult", __LINE__)
		,TypedInputFilter(o)
		,m_data(o.m_data)
		,m_buf(o.m_buf)
		,m_bufidx(o.m_bufidx)
		,m_stk(o.m_stk){}

	virtual TypedInputFilter* copy() const
	{
		return new PythonResult( *this);
	}

	///\brief Get next element
	///\param [out] type element type parsed
	///\param [out] element reference to element returned
	///\return true, throws on error
	virtual bool getNext( TypedInputFilter::ElementType& type, types::VariantConst& element)
	{
		for (;;)
		{
			if (m_bufidx && m_bufidx == m_buf.size())
			{
				m_bufidx = 0;
				m_buf.clear();
			}
			else if (m_bufidx < m_buf.size())
			{
				type = m_buf.at( m_bufidx).type;
				element = m_buf.at( m_bufidx).value;
				++m_bufidx;
				return true;
			}
			if (m_stk.empty())
			{
				return false;
			}
			if (m_stk.back().itr == m_stk.back().end)
			{
				m_stk.pop_back();
				if (m_stk.empty())
				{
					// ... if tag is defined then Open/Close was printed with every (array-) element
					type = InputFilter::CloseTag;
					element.clear();
					return true;
				}
				++m_stk.back().itr;
				if (m_stk.back().idx >= 0)
				{
					m_buf.push_back( BufElem( InputFilter::CloseTag, types::Variant()));
				}
				type = InputFilter::CloseTag;
				element.clear();
				return true;
			}
			if (m_stk.back().itr->atomic())
			{
				if (m_stk.back().tag.defined())
				{
					m_buf.push_back( BufElem( InputFilter::OpenTag, m_stk.back().tag));
				}
				else if (m_stk.back().idx >= 0)
				{
					m_buf.push_back( BufElem( InputFilter::OpenTag, types::Variant(++m_stk.back().idx)));
				}
				else
				{
					m_buf.push_back( BufElem( InputFilter::OpenTag, m_stk.back().itr->key));
				}
				m_buf.push_back( BufElem( InputFilter::Value, m_stk.back().itr->getValue()));
				m_buf.push_back( BufElem( InputFilter::CloseTag, types::Variant()));
				m_stk.back().itr++;
			}
			else
			{
				if (m_stk.back().itr->array())
				{
					if (m_stk.back().tag.defined() || m_stk.back().idx >= 0)
					{
						throw std::runtime_error("illegal structure: array of array");
					}
					if (flag( TypedInputFilter::SerializeWithIndices))
					{
						m_stk.push_back( StackElem( m_stk.back().itr->val, types::Variant(), 0));
						m_buf.push_back( BufElem( InputFilter::OpenTag, m_stk.back().itr->key));
					}
					else
					{
						types::Variant tag = m_stk.back().itr->key;
						// ... if tag is defined then Open/Close will be printed with every (array-) element
						//	and here we do not print an 'Open'
						m_stk.push_back( StackElem( m_stk.back().itr->val, tag, -1));
					}
				}
				else
				{
					if (m_stk.back().tag.defined())
					{
						m_buf.push_back( BufElem( InputFilter::OpenTag, m_stk.back().tag));
					}
					else if (m_stk.back().idx >= 0)
					{
						m_buf.push_back( BufElem( InputFilter::OpenTag, types::Variant(++m_stk.back().idx)));
					}
					else
					{
						m_buf.push_back( BufElem( InputFilter::OpenTag, m_stk.back().itr->key));
					}
					m_stk.push_back( StackElem( m_stk.back().itr->val, types::Variant(), -1));
				}
			}
		}
	}

private:
	const python::StructureR m_data;
	struct BufElem
	{
		TypedInputFilter::ElementType type;
		types::Variant value;

		BufElem()
			:type(TypedInputFilter::Value){}
		BufElem( const TypedInputFilter::ElementType& type_, const types::Variant& value_)
			:type(type_)
			,value(value_){}
		BufElem( const BufElem& o)
			:type(o.type)
			,value(o.value){}
	};
	std::vector<BufElem> m_buf;
	std::size_t m_bufidx;
	struct StackElem
	{
		python::Structure::const_iterator itr;
		python::Structure::const_iterator end;
		types::Variant tag;
		python::Structure obj;
		int idx;

		StackElem(){}
		StackElem( const python::Structure& obj_, int idx_)
			:obj(obj_),idx(idx_)
		{
			itr = obj.begin(); end = obj.end();
		}
		StackElem( PyObject* obj_, const types::Variant& tag_, int idx_)
			:tag(tag_),obj(obj_),idx(idx_)
		{
			itr = obj.begin(); end = obj.end();
		}
		StackElem( const StackElem& o)
			:itr(o.itr),end(o.end),tag(o.tag),obj(o.obj),idx(o.idx){}
	};
	std::vector<StackElem> m_stk;
};


///\class PythonFormFunctionClosure
///\brief Wrapper to make an interpreter call appear as form function call
class PythonFormFunctionClosure
	:public langbind::FormFunctionClosure
{
public:
	PythonFormFunctionClosure( const std::string& name_, const python::InterpreterInstanceR& instance_)
		:m_name(name_),m_initialized(false),m_instance(instance_),m_context(0){}

	virtual ~PythonFormFunctionClosure(){}

	virtual bool call()
	{
		if (!m_initialized)
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
							if (elem.type() == types::Variant::UInt || elem.type() == types::Variant::Int)
							{
								unsigned int idx = elem.touint();
								unsigned int lastidx = m_inputbuilder.lastArrayIndex();
								if (lastidx == 0 && idx != 1) throw std::runtime_error( "elements in array not starting from 1");
								if (idx <= lastidx) throw std::runtime_error( "elements in array not ascending");
								for (++lastidx; lastidx < idx; ++lastidx)
								{
									// add elements for holes in the array
									m_inputbuilder.openArrayElement();
									m_inputbuilder.setValue( types::Variant());
									m_inputbuilder.closeElement();
								}
								m_inputbuilder.openArrayElement();
							}
							else
							{
								m_inputbuilder.openElement( elem.tostring());
							}
							break;
						}
						case InputFilter::Attribute:
							m_tagbuf = elem.tostring();
							if (m_tagbuf.empty()) throw std::runtime_error( "illegal attribute name ''");
							break;

						case InputFilter::Value:
							if (!m_tagbuf.empty())
							{
								m_inputbuilder.openElement( m_tagbuf);
								m_inputbuilder.setValue( elem);
								m_inputbuilder.closeElement();
							}
							else if (!m_inputbuilder.defined() || m_inputbuilder.atomic())
							{
								m_inputbuilder.setValue( elem);
							}
							else
							{
								m_inputbuilder.openElement( "");
								m_inputbuilder.setValue( elem);
								m_inputbuilder.closeElement();
							}
							m_tagbuf.clear();
							break;

						case InputFilter::CloseTag:
							if (m_inputbuilder.taglevel() == 0)
							{
								m_input = m_inputbuilder.get();
								m_initialized = true;
							}
							else
							{
								m_inputbuilder.closeElement();
							}
							break;
					}
				}
			}
			catch (const std::runtime_error& e)
			{
				throw std::runtime_error( std::string( "error function call ") + m_name + " in parameter at " + m_inputbuilder.currentElementPath() + ": " + e.what());
			}
			if (!m_initialized) return false;
		}
		try
		{
			m_output = m_instance->call( m_context, m_input);
			LOG_TRACE << "Calling function '" << m_name << "' with argument: " << m_input->tostring() << "returns " << m_output->tostring();
			m_result.reset( new PythonResult( m_output));
		}
		catch (const std::runtime_error& e)
		{
			throw std::runtime_error( std::string( "error calling function '") + m_name + "': " + e.what());
		}
		return true;
	}

	virtual void init( proc::ExecContext* ctx, const TypedInputFilterR& arg, serialize::Flags::Enum /*f*/)
	{
		m_context = ctx;
		m_arg = arg;
		if (!m_arg->setFlags( TypedInputFilter::SerializeWithIndices))
		{
			throw std::runtime_error( "calling python without input structure info");
		}
		m_initialized = false;
		m_inputbuilder.clear();
		m_input.reset();
	}

	virtual TypedInputFilterR result() const
	{
		return m_result;
	}

private:
	TypedInputFilterR m_result;				//< result of the function call
	std::string m_name;					//< name of the function called for error messages
	TypedInputFilterR m_arg;				//< call argument as input filter
	bool m_initialized;					//< true, if the input has been initialized
	std::string m_tagbuf;					//< buffer for attribute name to handle Attribute+Value pair
	python::StructureBuilder m_inputbuilder;		//< structure input builder object
	python::StructureR m_input;				//< pointer to input structure
	python::StructureR m_output;				//< pointer to output structure
	python::InterpreterInstanceR m_instance;		//< interpreter instance
	proc::ExecContext* m_context;				//< pointer to execution context
};


///\class PythonFormFunction
///\brief Wrapper to make an python function appear as function that can be called by any other language binding
class PythonFormFunction
	:public langbind::FormFunction
{
public:
	PythonFormFunction( const python::Interpreter* interpreter_, const std::string& name_)
		:m_interpreter(interpreter_),m_name(name_){}

	virtual ~PythonFormFunction(){}

	virtual FormFunctionClosure* createClosure() const
	{
		python::InterpreterInstanceR instance = m_interpreter->getInstance( m_name);
		if (!instance.get()) return 0;
		return new PythonFormFunctionClosure( m_name, instance);
	}

private:
	const python::Interpreter* m_interpreter;
	std::string m_name;
};

///\class PythonProgramType
///\brief Program type of python programs
class PythonProgramType
	:public prgbind::Program
{
public:
	PythonProgramType()
		:prgbind::Program( prgbind::Program::Function){}

	virtual ~PythonProgramType(){}

	virtual bool is_mine( const std::string& filename) const
	{
		boost::filesystem::path p( filename);
		return p.extension().string() == ".mlg";
	}

	virtual void loadProgram( prgbind::ProgramLibrary& library, db::Database* /*transactionDB*/, const std::string& filename)
	{
		std::vector<std::string> funcs = m_interpreter.loadProgram( filename);
		std::vector<std::string>::const_iterator fi = funcs.begin(), fe = funcs.end();
		for (; fi != fe; ++fi)
		{
			langbind::FormFunctionR ff( new PythonFormFunction( &m_interpreter, *fi));
			library.defineFormFunction( *fi, ff);
		}
	}

private:
	python::Interpreter m_interpreter;
};
}//anonymous namespace

prgbind::Program* langbind::createPythonProgramType()
{
	return new PythonProgramType();
}



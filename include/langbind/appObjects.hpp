/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
///\file appObjects.hpp
///\brief interface for application processor scripting language to system objects

#ifndef _Wolframe_langbind_APPOBJECTS_HPP_INCLUDED
#define _Wolframe_langbind_APPOBJECTS_HPP_INCLUDED
#include "filter.hpp"
#include "ddl/structType.hpp"
#include <stack>
#include <string>
#include <algorithm>

namespace _Wolframe {
namespace langbind {

///\class FilterMap
///\brief Map of available filter seen from scripting language binding
class FilterMap
{
public:
	FilterMap();
	template <class FilterFactoryClass>
	void defineFilter( const char* name, const FilterFactoryClass& f)
	{
		std::string nam( name);
		std::transform( nam.begin(), nam.end(), nam.begin(), (int(*)(int)) std::tolower);
		m_map[ std::string(nam)] = new FilterFactoryClass(f);
	}
	bool getFilter( const char* arg, Filter& rt);
private:
	std::map<std::string,FilterFactory*> m_map;
};

///\class GlobalContext
///\brief Reference to all available processing resources seen from scripting language binding
class GlobalContext :public FilterMap
{
public:
	GlobalContext(){}
};

typedef CountedReference<GlobalContext> GlobalContextR;


///\class Output
///\brief Output as seen from scripting language binding
struct Output
{
	///\enum ItemType
	///\brief Output state
	enum ItemType
	{
		Data,		///< normal processing
		DoYield,	///< yield because rest of buffer not sufficient to complete operation
		Error		///< logic error in output. Operation is not possible
	};
	///\brief Constructor
	Output() :m_state(0){}
	///\brief Copy constructor
	///\param[in] o copied item
	Output( const Output& o) :m_outputfilter(o.m_outputfilter),m_state(0){}
	///\brief Destructor
	~Output(){}

	///\brief Print the next element
	///\param[in] e1 first element
	///\param[in] e1size first element size
	///\param[in] e2 second element
	///\param[in] e2size second element size
	///\return state returned
	ItemType print( const char* e1, unsigned int e1size, const char* e2, unsigned int e2size);

	const protocol::OutputFilterR& outputfilter() const		{return m_outputfilter;}
	protocol::OutputFilterR& outputfilter()				{return m_outputfilter;}

protected:
	protocol::OutputFilterR m_outputfilter;	///< output filter reference

private:
	unsigned int m_state;						///< current state for outputs with more than one elements
};

///\class Input
///\brief input as seen from the application processor program
struct Input
{
	///\brief Constructor
	Input(){}
	///\brief Copy constructor
	///\param[in] o copied item
	Input( const Input& o) :m_inputfilter(o.m_inputfilter){}
	///\brief Destructor
	~Input(){}

	const protocol::InputFilterR& inputfilter() const		{return m_inputfilter;}
	protocol::InputFilterR& inputfilter()				{return m_inputfilter;}
protected:
	protocol::InputFilterR m_inputfilter;			///< input is defined by the associated input filter
};

struct DDLForm
{
	ddl::StructType m_struct;

	///\brief Default constructor
	DDLForm() {}

	///\brief Copy constructor
	///\param[in] o copied item
	DDLForm( const DDLForm& o)
		:m_struct(o.m_struct){}
	///\brief Constructor
	///\param[in] st form data
	DDLForm( const ddl::StructType& st)
		:m_struct(st){}

	///\brief Destructor
	~DDLForm(){}
};

///\class InputFilterClosure
///\brief Closure for the input iterator (in Lua returned by 'input.get()')
class InputFilterClosure
{
public:
	///\enum ItemType
	///\brief Input loop state
	enum ItemType
	{
		EndOfData,	///< End of processed content reached
		Data,		///< normal processing, loop can continue
		DoYield,	///< have to yield and request more network input
		Error		///< have to stop processing because of an error
	};

	///\brief Constructor
	///\param[in] ig input filter reference from input
	InputFilterClosure( const protocol::InputFilterR& ig)
		:m_inputfilter(ig)
		,m_type(protocol::InputFilter::Value)
		,m_gotattr(false)
		,m_taglevel(0){}

	InputFilterClosure( const InputFilterClosure& o)
		:m_inputfilter(o.m_inputfilter)
		,m_type(o.m_type)
		,m_attrbuf(o.m_attrbuf)
		,m_gotattr(o.m_gotattr)
		,m_taglevel(o.m_taglevel){}

	///\brief Get the next pair of elements
	///\param[out] e1 first element
	///\param[out] e1size first element size
	///\param[out] e2 second element
	///\param[out] e2size second element size
	///\return state returned
	ItemType fetch( const char*& e1, unsigned int& e1size, const char*& e2, unsigned int& e2size);

private:
	protocol::InputFilterR m_inputfilter;			//< rerefence to input with filter
	protocol::InputFilter::ElementType m_type;		//< current state (last value type parsed)
	std::string m_attrbuf;					//< buffer for attribute name
	bool m_gotattr;						//< true, if the following value belongs to an attribute
	std::size_t m_taglevel;					//< current level in tag hierarchy
};

class AppContext
{
public:
private:
	std::map<std::string,Filter> m_filtermap;
	std::map<std::string,DDLForm> m_formmap;
};

}}//namespace
#endif


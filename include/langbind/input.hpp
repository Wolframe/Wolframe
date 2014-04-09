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
//\file langbind/input.hpp
//\brief Interface to network output for processor language bindings
#ifndef _Wolframe_langbind_INPUT_HPP_INCLUDED
#define _Wolframe_langbind_INPUT_HPP_INCLUDED
#include "filter/filter.hpp"
#include "utils/typeSignature.hpp"
#include <cstdlib>
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

//\class Input
//\brief Input as seen from the application processor program
class Input
	:public virtual utils::TypeSignature
{
public:
	//\brief Constructor
	Input()
		:utils::TypeSignature("langbind::Input", __LINE__)
		,m_used(false){}

	//\brief Copy constructor
	//\param[in] o copied item
	Input( const Input& o);

	//\brief Constructor by input filter
	//\param[in] inputfilter_ input filter reference
	//\param[in] docformat_ document format
	Input( const InputFilterR& inputfilter_, const std::string& docformat_);

	//\brief Constructor by content
	//\param[in] docformat_ document format
	//\param[in] content_ content string
	Input( const std::string& docformat_, const std::string& content_);

	//\brief Destructor
	~Input(){}

	//\brief Get the input filter attached to input
	//\return the input filter reference
	const InputFilterR& inputfilter() const		{return m_inputfilter;}

	//\brief Get the input filter attached to input
	//\return the input filter reference
	InputFilterR& inputfilter()			{return m_inputfilter;}

	//\brief Get the document format as recognized by the document type detection as string {"xml","json",...}
	//\return the document format as string
	const std::string& docformat() const		{return m_docformat;}

	//\brief Get the input filter attached to input and check for duplicate access
	//\return the input filter reference
	InputFilterR& getIterator();

	//\brief Eval if this represents a document
	//\return true, if yes
	bool isDocument() const				{return !m_isProcessorInput;}
	//\brief Get content source string if input is not from network
	//\return the content string pointer or NULL, if not defined
	const char* documentptr() const			{return m_isProcessorInput?0:m_content.get();}
	//\brief Get size of the content source string if input is not from network
	//\return the content string size or 0, if not defined
	std::size_t documentsize() const		{return m_isProcessorInput?0:m_contentsize;}

	//\brief Allocate a copy of the content to the input for a filter attached
	//\return the copy (managed by the input object)
	const void* allocContentCopy( const void* ptr, std::size_t size);

private:
	bool m_used;					//< only one iterator can be created from input. This is the guard for checking this.
	InputFilterR m_inputfilter;			//< input is defined by the associated input filter
	std::string m_docformat;			//< document format as recognized by the document type detection as string {"xml","json",...}
	boost::shared_ptr<char> m_content;		//< content source string is input is not from network
	std::size_t m_contentsize;			//< size of content in bytes
	bool m_isProcessorInput;			//< true, if this Input object represents a document and not the processor input
};

}}//namespace
#endif

